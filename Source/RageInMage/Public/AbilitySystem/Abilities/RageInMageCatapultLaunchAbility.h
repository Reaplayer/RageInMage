// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "RageInMageCatapultLaunchAbility.generated.h"

/**
 * Catapult Launch - Air Secondary ability. A short forward dash that "grabs" the first enemy it reaches; the
 * air mage vaults over that enemy and hurls it backward (relative to the victim's own facing) as a living
 * projectile. The thrown enemy takes the initial hit, and any other enemy it flies near mid-flight is struck
 * and knocked back too.
 *
 * Reuses the CleanWindDash dash mechanic (BaseWalkSpeed boost, ECC_Pawn pass-through) to close on the target,
 * then launches the victim with ApplyKnockbackImpulse and tracks it for ThrowTrackDuration, dealing CauseDamage
 * + ApplyKnockback to fresh enemies within ThrowCollisionRadius of the flying body. Damage/knockback are
 * authority-gated. If the dash reaches its full range without catching anyone it simply whiffs and ends.
 */
UCLASS()
class RAGEINMAGE_API URageInMageCatapultLaunchAbility : public URageInMageDamageGameplayAbility
{
	GENERATED_BODY()

public:
	URageInMageCatapultLaunchAbility();

	/** Begins the dash-grab-throw. Call from the GA graph after CommitAbility. */
	UFUNCTION(BlueprintCallable, Category = "CatapultLaunch")
	void BeginCatapultLaunch();

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	/** BaseWalkSpeed set for the duration of the dash (reverted on end). Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CatapultLaunch|Dash")
	FScalableFloat CharacterSpeed;

	/** Max forward reach to catch an enemy. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CatapultLaunch|Dash")
	FScalableFloat DashDistance;

	/** Width of the sweep used to catch the first enemy along the dash path. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CatapultLaunch|Dash")
	float DashWidth = 150.f;

	/** Hard safety cap so a blocked dash can't hang the ability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CatapultLaunch|Dash")
	float MaxDashSafetyDuration = 1.5f;

	/** Impulse magnitude the grabbed enemy is thrown with. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CatapultLaunch|Throw")
	FScalableFloat ThrowStrength;

	/** Upward component of the throw (0-1). Higher = more of an arc. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CatapultLaunch|Throw")
	float ThrowUpwardForce = 0.5f;

	/** How long (seconds) the thrown enemy is tracked for mid-air collisions with other enemies. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CatapultLaunch|Throw")
	float ThrowTrackDuration = 1.f;

	/** Radius around the flying enemy within which other enemies are struck mid-air. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CatapultLaunch|Throw")
	float ThrowCollisionRadius = 150.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CatapultLaunch|Debug")
	bool bShowDebug = false;

private:
	void DashTick();
	void EndDash();
	void DoThrow();
	void ThrowTrackTick();

	// Dash state
	bool bIsDashing = false;
	FTimerHandle DashTickTimerHandle;
	FVector DashDirection = FVector::ZeroVector;
	FVector DashStartLocation = FVector::ZeroVector;
	float ActualDashDistance = 0.f;
	float OriginalBaseWalkSpeed = 600.f;
	float DashElapsedSafety = 0.f;
	TEnumAsByte<ECollisionResponse> SavedPawnCollisionResponse = ECR_Block;

	// Grab / throw state
	TWeakObjectPtr<AActor> GrabbedEnemy;
	FTimerHandle ThrowTrackTimerHandle;
	float ThrowElapsed = 0.f;
	TSet<TWeakObjectPtr<AActor>> ThrowAlreadyHit;
};
