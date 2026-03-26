// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "RageInMageLeapAbility.generated.h"

class UNiagaraSystem;

/**
 * Leap ability - jumps a short distance, then explodes on landing dealing AoE damage and pushback.
 * Used for Ignition Leap (fire movement ability).
 */
UCLASS()
class RAGEINMAGE_API URageInMageLeapAbility : public URageInMageDamageGameplayAbility
{
	GENERATED_BODY()

public:
	/** Apply explosion damage and pushback at the landing location. Called from Blueprint (e.g. on landing anim notify). */
	UFUNCTION(BlueprintCallable, Category = "Leap")
	void ApplyLandingExplosion();

	/** Calculate the launch velocity needed to land exactly at the target location using a parabolic arc.
	 *  LeapHeight controls the peak height of the arc. */
	UFUNCTION(BlueprintCallable, Category = "Leap")
	FVector CalculateLaunchVelocityToTarget(const FVector& TargetLocation) const;

	/** Calculate preview points along the leap arc for visualization.
	 *  Uses the same physics as CalculateLaunchVelocityToTarget. */
	UFUNCTION(BlueprintCallable, Category = "Leap")
	TArray<FVector> GetLeapArcPreviewPoints(const FVector& TargetLocation, int32 NumPoints = 30) const;

protected:
	/** Horizontal speed multiplier applied to the leap direction. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Leap")
	FScalableFloat LeapSpeed;

	/** Upward launch velocity for the leap arc. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Leap")
	FScalableFloat LeapHeight;

	/** Radius of the AoE explosion on landing. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Leap|Explosion")
	FScalableFloat ExplosionRadius;

	/** How far enemies are pushed back from the center. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Leap|Explosion")
	FScalableFloat PushbackStrength;

	/** Niagara effect to spawn at the landing explosion. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Leap|VFX")
	TObjectPtr<UNiagaraSystem> ExplosionEffect;

	/** Sound to play on landing explosion. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Leap|SFX")
	TObjectPtr<USoundBase> ExplosionSound;
};
