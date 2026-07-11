// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "ScalableFloat.h"
#include "AbilitySystem/Abilities/RageInMageGameplayAbility.h"
#include "RageInMageMountainVortexAbility.generated.h"

/**
 * Mountain Vortex - Air Defensive ability. For VortexDuration seconds a wind vortex surrounds the caster and
 * reflects incoming enemy projectiles back at whoever fired them. It doesn't deal damage itself - the
 * reflected projectiles carry their original damage back to their shooter's team.
 *
 * Implementation: while active, a scan timer periodically sweeps for ARageInMageProjectile actors within
 * VortexRadius whose instigator is hostile to the caster, and calls ARageInMageProjectile::ReflectFrom(caster)
 * on each - that reverses the projectile toward its shooter and hands ownership to the caster so it now damages
 * the shooter's team. Reflected projectiles self-exclude from later scans (their instigator becomes the caster,
 * so the IsFriendly check skips them). Server-authoritative; the scan only runs with authority.
 */
UCLASS()
class RAGEINMAGE_API URageInMageMountainVortexAbility : public URageInMageGameplayAbility
{
	GENERATED_BODY()

public:
	URageInMageMountainVortexAbility();

	/** Starts the reflect vortex for VortexDuration seconds. Call from the GA graph after CommitAbility. */
	UFUNCTION(BlueprintCallable, Category = "MountainVortex")
	void BeginMountainVortex();

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	/** How long the vortex reflects incoming projectiles, in seconds. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MountainVortex")
	FScalableFloat VortexDuration;

	/** Radius around the caster within which enemy projectiles are caught and reflected. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MountainVortex")
	FScalableFloat VortexRadius;

	/** How often (seconds) the vortex re-scans for new incoming projectiles. Smaller = catches faster projectiles. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MountainVortex")
	float ScanInterval = 0.05f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MountainVortex|Debug")
	bool bShowDebug = false;

private:
	void ScanAndReflect();
	void FinishVortex();

	FTimerHandle ScanTimerHandle;
	FTimerHandle DurationTimerHandle;
};
