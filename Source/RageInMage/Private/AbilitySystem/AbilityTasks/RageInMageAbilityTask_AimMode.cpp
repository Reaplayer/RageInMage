// Copyright Reaplays

#include "AbilitySystem/AbilityTasks/RageInMageAbilityTask_AimMode.h"
#include "AbilitySystemComponent.h"
#include "Player/RageInMagePlayerController.h"

URageInMageAbilityTask_AimMode* URageInMageAbilityTask_AimMode::CreateAimModeTask(
	UGameplayAbility* OwningAbility, float InMaxRange)
{
	URageInMageAbilityTask_AimMode* Task = NewAbilityTask<URageInMageAbilityTask_AimMode>(OwningAbility);
	Task->MaxRange = InMaxRange;
	return Task;
}

void URageInMageAbilityTask_AimMode::Activate()
{
	Super::Activate();
	bTickingTask = true;
	bInputReleased = false;

	// Check if input is already released before we start ticking
	if (Ability)
	{
		if (const FGameplayAbilitySpec* Spec = Ability->GetCurrentAbilitySpec())
		{
			if (!Spec->InputPressed)
			{
				// Input was already released before the task started
				HandleInputRelease();
				return;
			}
		}
	}
}

void URageInMageAbilityTask_AimMode::TickTask(float DeltaTime)
{
	if (bInputReleased) return;

	// Check if input was released since last tick
	if (Ability)
	{
		if (const FGameplayAbilitySpec* Spec = Ability->GetCurrentAbilitySpec())
		{
			if (!Spec->InputPressed)
			{
				HandleInputRelease();
				return;
			}
		}
	}

	const FVector AimPos = GetClampedAimPosition();

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnAimUpdate.Broadcast(AimPos, DeltaTime);
	}
}

void URageInMageAbilityTask_AimMode::HandleInputRelease()
{
	if (bInputReleased) return;
	bInputReleased = true;

	const FVector FinalAimPos = GetClampedAimPosition();

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnInputReleased.Broadcast(FinalAimPos);
	}

	EndTask();
}

void URageInMageAbilityTask_AimMode::OnDestroy(bool bInOwnerFinished)
{
	// If destroyed without a release (ability cancelled), broadcast cancel
	if (!bInputReleased && ShouldBroadcastAbilityTaskDelegates())
	{
		OnCancelled.Broadcast();
	}

	Super::OnDestroy(bInOwnerFinished);
}

FVector URageInMageAbilityTask_AimMode::GetClampedAimPosition() const
{
	FVector AimPos = FVector::ZeroVector;

	if (const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo())
	{
		if (const ARageInMagePlayerController* MagePC = Cast<ARageInMagePlayerController>(ActorInfo->PlayerController.Get()))
		{
			AimPos = MagePC->GetCurrentAimWorldPosition();
		}
	}

	// Clamp to max range if set
	if (MaxRange > 0.f)
	{
		if (const AActor* Avatar = Ability->GetAvatarActorFromActorInfo())
		{
			const FVector Origin = Avatar->GetActorLocation();
			const FVector ToAim = AimPos - Origin;
			if (ToAim.Size2D() > MaxRange)
			{
				const FVector ClampedDir = ToAim.GetSafeNormal2D();
				AimPos = Origin + ClampedDir * MaxRange;
				AimPos.Z = AimPos.Z; // Keep original Z
			}
		}
	}

	return AimPos;
}
