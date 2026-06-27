// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageGameplayAbility.h"
#include "Player/RageInMagePlayerController.h"
#include "RageInMageGameplayTag.h"

URageInMageGameplayAbility::URageInMageGameplayAbility()
{
	// All abilities are blocked while the caster is channeling another spell.
	// Channeling abilities grant Status.Channeling via ActivationOwnedTags.
	ActivationBlockedTags.AddTag(FRageInMageGameplayTag::Get().Status_Channeling);

	// Stunned characters cannot act.
	ActivationBlockedTags.AddTag(FRageInMageGameplayTag::Get().Condition_Stunned);
}

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
