// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageLightningDamageAbility.h"
#include "RageInMageLightningFlashAbility.generated.h"

class UNiagaraSystem;

/**
 * Lightning Flash — fires a burst of lightning rods into a cone in front of the caster.
 * Each rod is its own CauseChainDamage hit (own crit roll, own Charge gain, own chain run) aimed at
 * a random living target found in the cone, without replacement — once everyone's been hit once,
 * the pool refills and random selection starts over. Instead of one flat blast hitting everyone at once.
 *
 * Blueprint workflow:
 * 1. ActivateAbility -> CommitAbility
 * 2. AimMode task -> OnInputReleased
 * 3. ApplyLightningFlash(TargetLocation)
 * 4. EndAbility
 */
UCLASS()
class RAGEINMAGE_API URageInMageLightningFlashAbility : public URageInMageLightningDamageAbility
{
	GENERATED_BODY()

public:
	/** Fire the lightning rods toward the target location from the caster's position. */
	UFUNCTION(BlueprintCallable, Category = "LightningFlash")
	void ApplyLightningFlash(const FVector& TargetLocation);

protected:
	/** Half-angle of the cone in degrees. 45 = 90-degree total cone. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LightningFlash")
	float ConeHalfAngle = 20.f;

	/** Range of the cone from the caster. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LightningFlash")
	FScalableFloat ConeRange;

	/** Max vertical distance (above and below caster) that the cone can hit. 0 = unlimited. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LightningFlash")
	float ConeHeightLimit = 0.f;

	/** Number of lightning rods fired. Each rod picks a random living target found in the cone
	 *  without replacement — if there are fewer targets than rods, the pool of who's eligible
	 *  refills once everyone's been hit once. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LightningFlash")
	FScalableFloat RodCount;

	/** VFX spawned at each rod's impact point — one spawn per rod fired, so the spawn count equals
	 *  the attack count. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LightningFlash|VFX")
	TObjectPtr<UNiagaraSystem> RodImpactEffect;

	/** Sound played once on cast. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LightningFlash|SFX")
	TObjectPtr<USoundBase> LightningFlashSound;
};
