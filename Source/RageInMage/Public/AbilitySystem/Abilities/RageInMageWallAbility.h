// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "RageInMageWallAbility.generated.h"

class ARageInMageFireWall;

/**
 * Wall ability - spawns a stationary wall in front of the caster.
 * The wall destroys enemy projectiles and damages enemies inside.
 * Used for Rising Flame Wave (fire defensive ability).
 */
UCLASS()
class RAGEINMAGE_API URageInMageWallAbility : public URageInMageDamageGameplayAbility
{
	GENERATED_BODY()

public:
	/** Spawn the wall in front of the caster. Called from Blueprint. */
	UFUNCTION(BlueprintCallable, Category = "Wall")
	void SpawnWall();

protected:
	/** The fire wall class to spawn. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall")
	TSubclassOf<ARageInMageFireWall> FireWallClass;

	/** How far in front of the caster the wall spawns. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall")
	float SpawnDistanceInFront = 200.f;

	/** How long the wall lasts. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall")
	FScalableFloat WallDuration;

	/** How often the wall ticks damage. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall")
	float WallDamageTickInterval = 1.f;

	/** Whether this wall destroys enemy projectiles on contact. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall")
	bool bWallDestroysProjectiles = true;
};
