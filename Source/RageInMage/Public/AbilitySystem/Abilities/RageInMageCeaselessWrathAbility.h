// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "RageInMageCeaselessWrathAbility.generated.h"

class UNiagaraSystem;

/**
 * Ceaseless Wrath (Lightning Ultimate) — every WaveInterval seconds, for WrathDuration seconds total,
 * strikes StrikeCount lightning bolts simultaneously (no stagger — lightning hits straight down at
 * the target point, unlike Meteor's caster-to-impact travel) within ZoneRadius of the cast point.
 * Each wave re-tracks currently-living enemies fresh (no repeats within that wave, falls back to
 * random points once enemies run out — same idea as FallOfTheScorchingSun, simplified: no travel
 * time, just one shared StrikeImpactDelay per wave for VFX sync). Each strike deals AoE damage like
 * a meteor explosion, but leaves no lingering zone/burning area behind.
 */
UCLASS()
class RAGEINMAGE_API URageInMageCeaselessWrathAbility : public URageInMageDamageGameplayAbility
{
	GENERATED_BODY()

public:
	/** Begins the ultimate at TargetCenter. Called from Blueprint (e.g. on input released). */
	UFUNCTION(BlueprintCallable, Category = "CeaselessWrath")
	void BeginCeaselessWrath(const FVector& TargetCenter);

protected:
	/** Toggle debug visualization of the zone and per-strike explosion radii. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CeaselessWrath|Debug")
	bool bShowDebug = false;

	/** Number of lightning strikes per wave. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CeaselessWrath")
	FScalableFloat StrikeCount;

	/** Radius of the overall zone strikes can land in. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CeaselessWrath")
	FScalableFloat ZoneRadius;

	/** AoE radius of each individual strike's explosion (no lingering zone left behind). Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CeaselessWrath")
	FScalableFloat StrikeExplosionRadius;

	/** Seconds between waves of strikes. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CeaselessWrath")
	FScalableFloat WaveInterval;

	/** Total duration the ultimate keeps striking for. Scales with ability level.
	 *  Total waves = floor(WrathDuration / WaveInterval) + 1 (first wave fires immediately at t=0). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CeaselessWrath")
	FScalableFloat WrathDuration;

	/** Fixed delay between a wave's strikes spawning and their damage applying — for VFX sync only,
	 *  not distance-based like Meteor's travel time, since strikes don't travel from the caster. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CeaselessWrath")
	float StrikeImpactDelay = 0.2f;

	/** Niagara effect spawned at each strike's impact point (e.g. NS_Lightning_Strike_Blue). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CeaselessWrath|VFX")
	TObjectPtr<UNiagaraSystem> StrikeEffect;

	/** Sound played at each strike's impact point. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CeaselessWrath|SFX")
	TObjectPtr<USoundBase> StrikeSound;

private:
	void LaunchWave();

	FVector WrathCenter;
	int32 WavesRemaining = 0;
	FTimerHandle WaveTimerHandle;
};
