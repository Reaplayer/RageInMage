// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "RageInMageConeBlastAbility.generated.h"

class UNiagaraSystem;

/**
 * Cone blast ability — deals instant damage, applies effects, and pushes back enemies in a cone.
 * Used for Drop Ripple Blast (Water CC), and reusable for any cone-shaped attack.
 *
 * Blueprint workflow:
 * 1. ActivateAbility -> CommitAbility
 * 2. AimMode task -> OnInputReleased
 * 3. ApplyConeBlast(TargetLocation)
 * 4. EndAbility
 */
UCLASS()
class RAGEINMAGE_API URageInMageConeBlastAbility : public URageInMageDamageGameplayAbility
{
	GENERATED_BODY()

public:
	/** Fire the cone blast toward the target location from the caster's position. */
	UFUNCTION(BlueprintCallable, Category = "ConeBlast")
	void ApplyConeBlast(const FVector& TargetLocation);

protected:
	/** Half-angle of the cone in degrees. 45 = 90-degree total cone. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ConeBlast")
	float ConeHalfAngle = 45.f;

	/** Range of the cone from the caster. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ConeBlast")
	FScalableFloat ConeRange;

	/** Pushback force applied to enemies in the cone. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ConeBlast|Pushback")
	FScalableFloat PushbackStrength;

	/** Upward component of pushback (0-1). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ConeBlast|Pushback")
	float PushbackUpwardForce = 0.2f;

	/** GE applied to each enemy hit (e.g. slow, wet). Optional — ignored if null. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ConeBlast|Effects")
	TSubclassOf<UGameplayEffect> OnHitEffectClass;

	/** VFX for the cone blast. Spawned at caster location facing blast direction.
	 *  Niagara user params: ConeRange (float), ConeAngle (float). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ConeBlast|VFX")
	TObjectPtr<UNiagaraSystem> ConeBlastEffect;

	/** Sound on blast. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ConeBlast|SFX")
	TObjectPtr<USoundBase> ConeBlastSound;
};
