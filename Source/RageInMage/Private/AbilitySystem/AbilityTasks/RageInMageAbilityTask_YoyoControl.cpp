// Copyright Reaplays

#include "AbilitySystem/AbilityTasks/RageInMageAbilityTask_YoyoControl.h"

#include "AbilitySystemComponent.h"
#include "Actor/RageInMageYoyoProjectile.h"
#include "Player/RageInMagePlayerController.h"


URageInMageAbilityTask_YoyoControl* URageInMageAbilityTask_YoyoControl::CreateYoyoControlTask(
	UGameplayAbility* OwningAbility,
	ARageInMageYoyoProjectile* Projectile,
	float InRecastRadius)
{
	URageInMageAbilityTask_YoyoControl* Task = NewAbilityTask<URageInMageAbilityTask_YoyoControl>(OwningAbility);
	Task->TrackedProjectile = Projectile;
	Task->RecastRadius = InRecastRadius;
	return Task;
}

void URageInMageAbilityTask_YoyoControl::Activate()
{
	Super::Activate();
	bTickingTask = true;
	bInRecastRange = false;

	// The ability's initial press may still be held. We need to wait for InputPressed to go FALSE
	// before we can detect a NEW press. Check current state.
	if (Ability)
	{
		if (const FGameplayAbilitySpec* Spec = Ability->GetCurrentAbilitySpec())
		{
			// If input is already released, we're ready to detect a new press immediately
			bWaitingForInputRelease = Spec->InputPressed;
		}
	}
}

void URageInMageAbilityTask_YoyoControl::TickTask(float DeltaTime)
{
	// Check if projectile is still alive
	if (!TrackedProjectile.IsValid())
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnProjectileLost.Broadcast();
		}
		EndTask();
		return;
	}

	// Get caster location
	const AActor* AvatarActor = Ability ? Ability->GetAvatarActorFromActorInfo() : nullptr;
	if (!AvatarActor) return;

	const float DistToCaster = FVector::Dist(TrackedProjectile->GetActorLocation(), AvatarActor->GetActorLocation());
	const bool bNowInRange = DistToCaster <= RecastRadius;

	// Broadcast range changes
	if (bNowInRange && !bInRecastRange)
	{
		bInRecastRange = true;
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnRecastAvailable.Broadcast();
		}
	}
	else if (!bNowInRange && bInRecastRange)
	{
		bInRecastRange = false;
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnRecastUnavailable.Broadcast();
		}
	}

	// Input detection — wait for release first, then detect new press
	if (Ability)
	{
		if (const FGameplayAbilitySpec* Spec = Ability->GetCurrentAbilitySpec())
		{
			if (bWaitingForInputRelease)
			{
				// Still waiting for the initial press to be released
				if (!Spec->InputPressed)
				{
					bWaitingForInputRelease = false;
				}
			}
			else
			{
				// Ready to detect a new press
				if (Spec->InputPressed && bInRecastRange)
				{
					const FVector AimPos = GetAimPosition();
					if (ShouldBroadcastAbilityTaskDelegates())
					{
						OnRecastTriggered.Broadcast(AimPos);
					}
					// After triggering recast, wait for next release-press cycle
					bWaitingForInputRelease = true;
					EndTask();
					return;
				}
			}
		}
	}
}

void URageInMageAbilityTask_YoyoControl::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}

FVector URageInMageAbilityTask_YoyoControl::GetAimPosition() const
{
	if (const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo())
	{
		if (const ARageInMagePlayerController* MagePC = Cast<ARageInMagePlayerController>(ActorInfo->PlayerController.Get()))
		{
			return MagePC->GetCurrentAimWorldPosition();
		}
	}
	return FVector::ZeroVector;
}
