// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageProjectileSpell.h"
#include "RageInMageSpreadProjectileSpell.generated.h"

/**
 * Spread projectile spell — fires multiple projectiles in a cone pattern.
 * Each projectile is a standard ARageInMageSphereProjectile spawned via the parent's SpawnProjectile().
 * Used for Simple Ice (Water Primary), and reusable for ThornSpread (Nature), etc.
 *
 * Blueprint workflow:
 * 1. ActivateAbility -> CommitAbility
 * 2. AimMode task -> OnInputReleased
 * 3. SpawnSpreadProjectiles(TargetLocation, SocketTag, ProjectileClass)
 * 4. EndAbility
 */
UCLASS()
class RAGEINMAGE_API URageInMageSpreadProjectileSpell : public URageInMageProjectileSpell
{
	GENERATED_BODY()

public:
	/**
	 * Spawn multiple projectiles in a cone pattern toward the target location.
	 * Each projectile gets its own damage spec via the parent's SpawnProjectile().
	 */
	UFUNCTION(BlueprintCallable, Category = "SpreadProjectile")
	void SpawnSpreadProjectiles(const FVector& TargetLocation, const FGameplayTag& SocketTag,
		TSubclassOf<AActor> ProjectileClass, bool bCalculatePitch = false);

protected:
	/** Number of projectiles to fire per salvo. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpreadProjectile")
	FScalableFloat ProjectileCount;

	/** Half-angle of the cone in degrees. 15 = 30-degree total spread. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpreadProjectile")
	float ConeHalfAngle = 15.f;
};
