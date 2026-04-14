// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "RageInMageProjectileSpell.generated.h"

class ARageInMageProjectile;

// Simple struct to store position history
struct FMotionSample
{
	FVector Location;
	double Time;
};
/**
 * 
 */
UCLASS()
class RAGEINMAGE_API URageInMageProjectileSpell : public URageInMageDamageGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData) override;
	
	/** Call this when you start casting (e.g. in ActivateAbility) */
	UFUNCTION(BlueprintCallable, Category = "Prediction")
	void StartTrackingTarget(AActor* TargetToTrack);

	void RecordTargetSample();

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectile(UPARAM(ref) FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, TSubclassOf<AActor> ProjectileClass, bool bCalculatePitch = false, AActor* LeadingTarget = nullptr);

private:
	UPROPERTY()
	AActor* TrackedActor;

	FTimerHandle TrackingTimerHandle;
	
	// Circular buffer or simple array of recent samples
	TArray<FMotionSample> TargetHistory;
};
