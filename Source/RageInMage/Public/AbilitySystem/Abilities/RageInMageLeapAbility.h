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

protected:
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
