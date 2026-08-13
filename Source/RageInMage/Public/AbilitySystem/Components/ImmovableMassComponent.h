// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "ImmovableMassComponent.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;
class ACharacter;
struct FGameplayEffectSpec;
struct FActiveGameplayEffect;

/** Result of routing an incoming physics knockback/airborne through the stance. */
UENUM()
enum class EImmovableKnockbackDecision : uint8
{
	Passthrough,	// stance inactive — apply the launch unchanged
	Reduced,		// light knockback — apply, scaled by the stage's knockback multiplier
	Blocked			// heavy knockback / airborne — absorbed (immune) or converted to a slow; no launch
};

/** Result of routing an incoming hard CC (GE-based or heavy-knockback) through the stance. */
UENUM()
enum class EImmovableCCResult : uint8
{
	NotHandled,		// stance inactive (Stage 0) — let the CC apply normally
	Ignored,		// Stage 3 "ignore first hard CC" charge consumed — CC fully negated, no slow
	Converted		// CC replaced by the stage's slow (added to the overload pool)
};

/**
 * Immovable Mass — the Earth mage's passive juggernaut stance.
 *
 * Standing still charges a 0-120 gauge; moving drains it (distance-based, so being self-slowed
 * naturally bleeds the gauge slower); dashing drains it faster. The gauge maps to 4 stages, each
 * trading mobility (self-slow + shorter dash, applied by the stage GameplayEffect) for defense
 * (hard-CC -> slow conversion, knockback resistance, damage reduction).
 *
 * Counterplay: separate-cast enemy slows (and converted hard CCs) stack additively in an overload
 * pool; if the total exceeds 100% the stance SHATTERS back to Stage 0, the accrued slows are
 * cleared, and a brief vulnerability window locks out recharging.
 *
 * Server-authoritative: only the authority ticks the gauge and swaps stage effects; the gauge is
 * mirrored to the replicated ImmovableMass attribute so clients/UI can read it. The component is
 * created on every player character but only ActivateStance()'d for the Earth mage.
 */
UCLASS(ClassGroup = (RageInMage), meta = (BlueprintSpawnableComponent))
class RAGEINMAGE_API UImmovableMassComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UImmovableMassComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Caches the ASC/owner, binds interception delegates, and enables the gauge tick. Call once the
	 *  ASC is initialized, and only for the Earth mage (the caller gates on character class). */
	void ActivateStance();

	/* ---- Queries used by ARageInMageCharacterBase::ApplyKnockbackImpulse ---- */

	/** True once the stance is at Stage 1 or higher (defensive effects live). */
	bool IsStanceActive() const { return CurrentStage >= 1; }

	/** Current stance stage (0-3). Earth abilities/actors read this to scale themselves off the stance. */
	int32 GetCurrentStage() const { return CurrentStage; }

	/** Decides what happens to an incoming physics launch. For heavy knockback / airborne this also
	 *  performs the hard-CC handling (ignore charge or slow conversion) as a side effect. */
	EImmovableKnockbackDecision EvaluateIncomingKnockback(const FVector& Impulse);

	/** Multiplier a light (non-hard-CC) knockback is scaled by at the current stage (1 = no resist). */
	float GetKnockbackMultiplier() const;

protected:
	virtual void BeginPlay() override;

	/* ================= Tunables (all editable on the component in BP) ================= */

	/** Gauge ceiling. Stages read against this. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|Gauge")
	float MaxGauge = 120.f;

	/** Gauge value at/above which each stage begins. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|Gauge")
	float Stage1Threshold = 30.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|Gauge")
	float Stage2Threshold = 60.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|Gauge")
	float Stage3Threshold = 90.f;

	/** Gauge gained per second while effectively stationary. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|Gauge")
	float ChargePerSecond = 15.f;

	/** Horizontal speed (cm/s) below which the mage counts as "standing still" and charges. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|Gauge")
	float StillnessSpeedThreshold = 20.f;

	/** Seconds of continuous stillness required before charging begins. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|Gauge")
	float StillnessDelay = 1.f;

	/** Gauge lost per cm travelled while moving normally (distance-based so slow -> less drain). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|Gauge")
	float DecayPerUnitMoved = 0.05f;

	/** Multiplier applied to DecayPerUnitMoved while the State.Dashing tag is present. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|Gauge")
	float DashDecayMultiplier = 2.f;

	/** Damage-reduction / self-slow are applied by these per-stage GameplayEffects (Stage 1/2/3). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|Stages")
	TSubclassOf<UGameplayEffect> Stage1Effect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|Stages")
	TSubclassOf<UGameplayEffect> Stage2Effect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|Stages")
	TSubclassOf<UGameplayEffect> Stage3Effect;

	/** Incoming light knockback keeps (1 - resist/100) of its impulse at each stage. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|Knockback", meta = (ClampMin = "0", ClampMax = "100"))
	float KnockbackResistStage1 = 30.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|Knockback", meta = (ClampMin = "0", ClampMax = "100"))
	float KnockbackResistStage2 = 60.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|Knockback", meta = (ClampMin = "0", ClampMax = "100"))
	float KnockbackResistStage3 = 100.f;

	/** Launch impulse magnitude (cm/s) at/above which a knockback counts as a hard CC. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|Knockback")
	float HeavyKnockbackThreshold = 900.f;

	/** Upward impulse (cm/s) at/above which a launch counts as an airborne/knock-up — always a hard
	 *  CC while the stance is active (Stage 1 converts it to a slow; Stage 2+ is the "cannot be made
	 *  airborne" case, still resolved through the shared hard-CC path). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|Knockback")
	float AirborneImpulseThreshold = 300.f;

	/** A hard CC at each stage is replaced with a slow of this magnitude (percent). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|CC", meta = (ClampMin = "0", ClampMax = "100"))
	float CCConvertSlowStage1 = 60.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|CC", meta = (ClampMin = "0", ClampMax = "100"))
	float CCConvertSlowStage2 = 40.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|CC", meta = (ClampMin = "0", ClampMax = "100"))
	float CCConvertSlowStage3 = 30.f;

	/** Effects granting any of these tags are treated as hard CC (convertible to a slow).
	 *  Anything that removes free movement. Defaulted in the constructor; editable per project. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|CC")
	TArray<FGameplayTag> HardCCTags;

	/** The slow applied when converting a hard CC. Reads its magnitude from a SetByCaller keyed to
	 *  Condition.Slowed (percent) and must modify the Slow attribute (additive) + grant Condition.Slowed,
	 *  so it accrues on the overload channel exactly like any enemy-cast slow. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|CC")
	TSubclassOf<UGameplayEffect> ConvertedSlowEffect;

	/** Slow-percent ceiling. When the Slow attribute reaches it, the stance shatters. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|Overload")
	float SlowOverloadThreshold = 100.f;

	/** After a shatter, seconds during which the gauge cannot charge (forced repositioning). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|Overload")
	float VulnerabilityDuration = 5.f;

	/** Real-time cooldown of the Stage 3 "ignore first hard CC" charge. Runs regardless of stage and
	 *  is never reset by dropping out of / back into Stage 3. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ImmovableMass|Stage3")
	float IgnoreHardCCCooldown = 20.f;

private:
	/* ================= Runtime state ================= */

	bool bActivated = false;
	int32 CurrentStage = 0;
	float TimeStill = 0.f;
	FVector LastLocation = FVector::ZeroVector;

	bool bInVulnerability = false;
	FTimerHandle VulnerabilityTimer;

	/** Stage 3 ignore charge — starts available; consumed on a hard CC at Stage 3. */
	bool bStage3IgnoreCharged = true;
	FTimerHandle IgnoreCooldownTimer;

	/** Handle to the currently-applied stage GameplayEffect (removed on stage change/shatter). */
	FActiveGameplayEffectHandle CurrentStageEffectHandle;

	TWeakObjectPtr<UAbilitySystemComponent> OwnerASC;
	TWeakObjectPtr<ACharacter> OwnerCharacter;

	/* ---- Gauge / staging ---- */
	void UpdateGauge(float DeltaTime);
	void SetGauge(float NewGauge);
	float GetGauge() const;
	int32 StageForGauge(float Gauge) const;
	void EvaluateStage();
	void ApplyStageEffect(int32 NewStage);

	/* ---- Hard CC handling (shared by GE and knockback paths) ---- */
	float CurrentCCConvertSlow() const;
	EImmovableCCResult HandleHardCC();
	void ConsumeStage3Ignore();
	void OnIgnoreCooldownExpired();

	void OnHardCCTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	/* ---- Overload ---- */
	/** Reads the accrued Slow attribute; shatters once it reaches SlowOverloadThreshold. */
	void CheckOverload();
	void Shatter();
	void StartVulnerability();
	void OnVulnerabilityExpired();

	/** Bound to the Slow attribute — re-checks the overload threshold whenever a slow lands or expires. */
	void OnSlowChanged(const FOnAttributeChangeData& Data);
};
