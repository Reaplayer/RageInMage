// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageProjectileSpell.h"
#include "RageInMageChargedSpell.generated.h"

class ARageInMageZone;
class UAudioComponent;
class UCurveFloat;
class UNiagaraSystem;
class UNiagaraComponent;

/**
 * Base class for charged lobbed projectile spells.
 * Hold to aim: arc is determined by distance to aim point. Charge time scales damage.
 * On impact: AoE explosion + optionally spawns a lingering zone.
 *
 * Blueprint workflow (with AimMode task):
 * 1. ActivateAbility -> CommitAbility -> BeginCharge() -> CreateAimModeTask
 * 2. OnAimUpdate: UpdateChargeAndIndicator(DeltaTime, AimLocation, SocketTag)
 * 3. OnInputReleased: ReleaseChargedSpell(TargetLocation, SocketTag) -> EndAbility
 *
 * Subclass for element-specific charged spells (ChargedFireball, ChargedIceball, etc.).
 */
UCLASS()
class RAGEINMAGE_API URageInMageChargedSpell : public URageInMageProjectileSpell
{
	GENERATED_BODY()

public:
	URageInMageChargedSpell();

	// ── Blueprint-callable charge API ──

	/** Call once when charging begins. Resets charge state. SocketTag determines where charge VFX attaches. */
	UFUNCTION(BlueprintCallable, Category = "ChargedSpell")
	void BeginCharge(const FGameplayTag& SocketTag);

	/**
	 * Call every frame while the button is held.
	 * Updates charge progress and recalculates the predicted landing location.
	 * Returns the current predicted landing location for the indicator.
	 */
	UFUNCTION(BlueprintCallable, Category = "ChargedSpell")
	FVector UpdateChargeAndIndicator(float DeltaTime, const FVector& AimLocation, const FGameplayTag& SocketTag);

	/** Call when the button is released. Spawns the projectile with the current charge level. */
	UFUNCTION(BlueprintCallable, Category = "ChargedSpell")
	void ReleaseChargedSpell(const FVector& TargetLocation, const FGameplayTag& SocketTag);

	/** Cancel the charge without firing. */
	UFUNCTION(BlueprintCallable, Category = "ChargedSpell")
	void CancelCharge();

	/** Get the current charge as a 0-1 percentage. */
	UFUNCTION(BlueprintPure, Category = "ChargedSpell")
	float GetChargePercent() const;

	// ── Blueprint-implementable indicator events ──

	/** Called when charge begins. Override in BP to spawn indicator VFX. */
	UFUNCTION(BlueprintImplementableEvent, Category = "ChargedSpell|Indicator")
	void OnChargeBegin();

	/**
	 * Called each frame with the updated landing location, charge percent, and indicator radius.
	 * IndicatorRadius = ImpactExplosionRadius * (1 + ChargePercent), matching the actual AoE at release.
	 * Override in BP to update indicator decal/VFX position, scale, and arc preview.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "ChargedSpell|Indicator")
	void OnChargeUpdate(const FVector& LandingLocation, float ChargePercent, float IndicatorRadius, const TArray<FVector>& ArcPoints);

	/** Called when charge ends (either by release or cancel). Override in BP to destroy indicator. */
	UFUNCTION(BlueprintImplementableEvent, Category = "ChargedSpell|Indicator")
	void OnChargeEnd();

protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// ── Charge tuning ──

	/** Maximum time to reach full charge (seconds). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChargedSpell|Charge")
	float MaxChargeTime = 2.0f;

	/** Delay before charge starts ramping (seconds). Charge stays at 0% during this window. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChargedSpell|Charge")
	float ChargeDelay = 1.0f;

	/** Maximum charge percent cap (0-1). Items can modify this. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChargedSpell|Charge")
	float MaxChargePercent = 1.0f;

	/** Arc at close range (near caster). 1.0 = near-flat, 0.5 = 45-degree. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChargedSpell|Charge")
	float MinChargeArc = 0.9f;

	/** Arc at max range (far from caster). Lower = higher lob. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChargedSpell|Charge")
	float MaxChargeArc = 0.3f;

	/** Optional damage multiplier curve. X: charge percent (0-1), Y: damage multiplier. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChargedSpell|Charge")
	TObjectPtr<UCurveFloat> ChargeDamageMultiplierCurve;

	// ── Projectile config ──

	/** The projectile class to spawn. Should be a child of ARageInMageSphereProjectile. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChargedSpell|Projectile")
	TSubclassOf<AActor> ChargedProjectileClass;

	// ── Impact explosion config ──

	/** Radius of AoE explosion on impact. 0 = single-target only. Scales with ability level. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChargedSpell|Explosion")
	FScalableFloat ImpactExplosionRadius;

	// ── Zone config ──

	/** Zone class to spawn on impact (FireZone, PoisonZone, etc.). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChargedSpell|Zone")
	TSubclassOf<ARageInMageZone> ZoneClass;

	/** Radius of the zone. Scales with ability level. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChargedSpell|Zone")
	FScalableFloat ZoneRadius;

	/** Duration of the zone. Scales with ability level. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChargedSpell|Zone")
	FScalableFloat ZoneDuration;

	/** Damage tick interval for the zone. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChargedSpell|Zone")
	float ZoneTickInterval = 1.0f;

	/** Separate damage GE for zone DoT (optional; if null, uses DamageEffectClass). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChargedSpell|Zone")
	TSubclassOf<UGameplayEffect> ZoneDamageEffectClass;

	/** Separate damage tags for zone DoT (optional; if empty, uses main DamageTypeTags). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChargedSpell|Zone")
	TMap<FGameplayTag, FScalableFloat> ZoneDamageTypeTags;

	// ── Arc preview ──

	/** Number of points along the predicted arc trajectory for the indicator spline. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChargedSpell|Indicator")
	int32 ArcPreviewPointCount = 30;

	// ── VFX / SFX (set on the GA, passed down to projectile/zone) ──

	/** VFX that plays while charging (spawned at socket, destroyed on release/cancel). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChargedSpell|VFX")
	TObjectPtr<UNiagaraSystem> ChargeEffect;

	/** Sound that plays while charging. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChargedSpell|SFX")
	TObjectPtr<USoundBase> ChargeSound;

	/** VFX for the AoE explosion on projectile impact. Passed to the projectile. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChargedSpell|VFX")
	TObjectPtr<UNiagaraSystem> ImpactExplosionEffect;

	/** Sound on projectile impact. Passed to the projectile. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChargedSpell|SFX")
	TObjectPtr<USoundBase> ImpactSound;

	/** VFX for the lingering zone. Passed through projectile to zone. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChargedSpell|VFX")
	TObjectPtr<UNiagaraSystem> ZoneEffect;

	/** Looping sound for the zone. Passed through projectile to zone. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChargedSpell|SFX")
	TObjectPtr<USoundBase> ZoneLoopSound;

private:
	float CurrentChargeTime = 0.f;
	bool bIsCharging = false;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ActiveChargeEffectComponent;

	UPROPERTY()
	TObjectPtr<UAudioComponent> ActiveChargeSoundComponent;

	float GetCurrentArc() const;
	float GetArcForDistance(const FVector& LaunchLocation, const FVector& TargetLocation) const;
	TArray<FVector> CalculateArcPoints(const FVector& LaunchLocation, const FVector& TargetLocation, float Arc) const;
	FVector CalculateLandingLocation(const FVector& LaunchLocation, const FVector& AimLocation) const;
	FGameplayEffectSpecHandle MakeImpactDamageSpec() const;
	FGameplayEffectSpecHandle MakeZoneDamageSpec() const;
};
