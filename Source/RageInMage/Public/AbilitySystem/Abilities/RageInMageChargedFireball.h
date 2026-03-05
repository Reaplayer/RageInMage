// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageProjectileSpell.h"
#include "RageInMageChargedFireball.generated.h"

class ARageInMageFireZone;
class UCurveFloat;

/**
 * Charged lobbed fireball ability (Blazing Flame - Fire CC).
 * Hold to aim: arc is determined by distance to aim point. Charge time scales damage.
 * On impact: AoE explosion + spawns a lingering fire zone.
 *
 * Blueprint workflow (with AimMode task):
 * 1. ActivateAbility -> CommitAbility -> BeginCharge() -> CreateAimModeTask
 * 2. OnAimUpdate: UpdateChargeAndIndicator(DeltaTime, AimLocation, SocketTag)
 * 3. OnInputReleased: ReleaseChargedFireball(TargetLocation, SocketTag) -> EndAbility
 */
UCLASS()
class RAGEINMAGE_API URageInMageChargedFireball : public URageInMageProjectileSpell
{
	GENERATED_BODY()

public:
	URageInMageChargedFireball();

	// ── Blueprint-callable charge API ──

	/** Call once when charging begins. Resets charge state. */
	UFUNCTION(BlueprintCallable, Category = "ChargedFireball")
	void BeginCharge();

	/**
	 * Call every frame while the button is held.
	 * Updates charge progress and recalculates the predicted landing location.
	 * Returns the current predicted landing location for the indicator.
	 */
	UFUNCTION(BlueprintCallable, Category = "ChargedFireball")
	FVector UpdateChargeAndIndicator(float DeltaTime, const FVector& AimLocation, const FGameplayTag& SocketTag);

	/** Call when the button is released. Spawns the fireball with the current charge level. */
	UFUNCTION(BlueprintCallable, Category = "ChargedFireball")
	void ReleaseChargedFireball(const FVector& TargetLocation, const FGameplayTag& SocketTag);

	/** Cancel the charge without firing. */
	UFUNCTION(BlueprintCallable, Category = "ChargedFireball")
	void CancelCharge();

	/** Get the current charge as a 0-1 percentage. */
	UFUNCTION(BlueprintPure, Category = "ChargedFireball")
	float GetChargePercent() const;

	// ── Blueprint-implementable indicator events ──

	/** Called when charge begins. Override in BP to spawn indicator VFX. */
	UFUNCTION(BlueprintImplementableEvent, Category = "ChargedFireball|Indicator")
	void OnChargeBegin();

	/**
	 * Called each frame with the updated landing location and charge percent.
	 * Override in BP to update indicator decal/VFX position, scale, and arc preview.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "ChargedFireball|Indicator")
	void OnChargeUpdate(const FVector& LandingLocation, float ChargePercent, const TArray<FVector>& ArcPoints);

	/** Called when charge ends (either by release or cancel). Override in BP to destroy indicator. */
	UFUNCTION(BlueprintImplementableEvent, Category = "ChargedFireball|Indicator")
	void OnChargeEnd();

protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// ── Charge tuning ──

	/** Maximum time to reach full charge (seconds). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargedFireball|Charge")
	float MaxChargeTime = 2.0f;

	/** Arc at close range (near caster). 1.0 = near-flat, 0.5 = 45-degree. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargedFireball|Charge")
	float MinChargeArc = 0.9f;

	/** Arc at max range (far from caster). Lower = higher lob. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargedFireball|Charge")
	float MaxChargeArc = 0.3f;

	/** Optional damage multiplier curve. X: charge percent (0-1), Y: damage multiplier. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargedFireball|Charge")
	TObjectPtr<UCurveFloat> ChargeDamageMultiplierCurve;

	// ── Projectile config ──

	/** The projectile class to spawn. Should be a child of ARageInMageSphereProjectile. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargedFireball|Projectile")
	TSubclassOf<AActor> ChargedProjectileClass;

	// ── Impact explosion config ──

	/** Radius of AoE explosion on impact. 0 = single-target only. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargedFireball|Explosion")
	FScalableFloat ImpactExplosionRadius;

	// ── Fire zone config ──

	/** Fire zone class to spawn on impact. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargedFireball|FireZone")
	TSubclassOf<ARageInMageFireZone> FireZoneClass;

	/** Radius of the fire zone. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargedFireball|FireZone")
	FScalableFloat FireZoneRadius;

	/** Duration of the fire zone. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargedFireball|FireZone")
	FScalableFloat FireZoneDuration;

	/** Damage tick interval for the fire zone. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargedFireball|FireZone")
	float FireZoneTickInterval = 1.0f;

	/** Separate damage GE for fire zone DoT (optional; if null, uses DamageEffectClass). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargedFireball|FireZone")
	TSubclassOf<UGameplayEffect> FireZoneDamageEffectClass;

	/** Separate damage tags for fire zone DoT (optional; if empty, uses main DamageTypeTags). */
	UPROPERTY(EditDefaultsOnly, Category = "ChargedFireball|FireZone")
	TMap<FGameplayTag, FScalableFloat> FireZoneDamageTypeTags;

	// ── Arc preview ──

	/** Number of points along the predicted arc trajectory for the indicator spline. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargedFireball|Indicator")
	int32 ArcPreviewPointCount = 30;

private:
	float CurrentChargeTime = 0.f;
	bool bIsCharging = false;

	float GetCurrentArc() const;
	float GetArcForDistance(const FVector& LaunchLocation, const FVector& TargetLocation) const;
	TArray<FVector> CalculateArcPoints(const FVector& LaunchLocation, const FVector& TargetLocation, float Arc) const;
	FVector CalculateLandingLocation(const FVector& LaunchLocation, const FVector& AimLocation) const;
	FGameplayEffectSpecHandle MakeImpactDamageSpec() const;
	FGameplayEffectSpecHandle MakeFireZoneDamageSpec() const;
};
