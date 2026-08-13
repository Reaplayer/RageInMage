// Copyright Reaplays

#include "AbilitySystem/AbilityTasks/RageInMageAbilityTask_DualAimMode.h"
#include "AbilitySystemComponent.h"
#include "Player/RageInMagePlayerController.h"

URageInMageAbilityTask_DualAimMode* URageInMageAbilityTask_DualAimMode::CreateDualAimModeTask(
	UGameplayAbility* OwningAbility, float InMaxRange)
{
	URageInMageAbilityTask_DualAimMode* Task = NewAbilityTask<URageInMageAbilityTask_DualAimMode>(OwningAbility);
	Task->MaxRange = InMaxRange;
	return Task;
}

void URageInMageAbilityTask_DualAimMode::Activate()
{
	Super::Activate();
	bTickingTask = true;
	bInputReleased = false;

	// Switch the controller into dual-stick aim mode (left stick places, right stick rotates).
	if (const FGameplayAbilityActorInfo* ActorInfo = Ability ? Ability->GetCurrentActorInfo() : nullptr)
	{
		if (ARageInMagePlayerController* MagePC = Cast<ARageInMagePlayerController>(ActorInfo->PlayerController.Get()))
		{
			MagePC->BeginDualAim(MaxRange);
		}
	}

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

void URageInMageAbilityTask_DualAimMode::TickTask(float DeltaTime)
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

	FVector AimPos, AimDir;
	GetAimState(AimPos, AimDir);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnAimUpdate.Broadcast(AimPos, AimDir, DeltaTime);
	}
}

void URageInMageAbilityTask_DualAimMode::HandleInputRelease()
{
	if (bInputReleased) return;
	bInputReleased = true;

	FVector FinalPos, FinalDir;
	GetAimState(FinalPos, FinalDir);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnInputReleased.Broadcast(FinalPos, FinalDir);
	}

	EndTask();
}

void URageInMageAbilityTask_DualAimMode::OnDestroy(bool bInOwnerFinished)
{
	// Hand the left stick back to movement and the right stick back to normal facing.
	if (const FGameplayAbilityActorInfo* ActorInfo = Ability ? Ability->GetCurrentActorInfo() : nullptr)
	{
		if (ARageInMagePlayerController* MagePC = Cast<ARageInMagePlayerController>(ActorInfo->PlayerController.Get()))
		{
			MagePC->EndDualAim();
		}
	}

	// If destroyed without a release (ability cancelled), broadcast cancel
	if (!bInputReleased && ShouldBroadcastAbilityTaskDelegates())
	{
		OnCancelled.Broadcast();
	}

	Super::OnDestroy(bInOwnerFinished);
}

void URageInMageAbilityTask_DualAimMode::GetAimState(FVector& OutPosition, FVector& OutDirection) const
{
	OutPosition = FVector::ZeroVector;
	OutDirection = FVector::ForwardVector;

	if (!Ability) return;

	if (const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo())
	{
		if (const ARageInMagePlayerController* MagePC = Cast<ARageInMagePlayerController>(ActorInfo->PlayerController.Get()))
		{
			// The controller already clamps the point to MaxRange (passed in at BeginDualAim)
			// and keeps it projected onto the ground, so no extra work is needed here.
			OutPosition = MagePC->GetDualAimPoint();
			OutDirection = MagePC->GetDualAimDirection();
		}
	}
}
