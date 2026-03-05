// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageGameplayAbility.h"
#include "Player/RageInMagePlayerController.h"

FVector URageInMageGameplayAbility::GetAimWorldPosition() const
{
	if (const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo())
	{
		if (const ARageInMagePlayerController* MagePC = Cast<ARageInMagePlayerController>(ActorInfo->PlayerController.Get()))
		{
			return MagePC->GetCurrentAimWorldPosition();
		}
	}

	// Fallback: project forward from avatar
	if (const AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		return Avatar->GetActorLocation() + Avatar->GetActorForwardVector() * 500.f;
	}
	return FVector::ZeroVector;
}
