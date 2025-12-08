// Copyright Reaplays


#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/RageInMageGameplayAbility.h"
#include "RageInMage/RageInMageLogChannels.h"

void URageInMageAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &URageInMageAbilitySystemComponent::ClientEffectApplied);
}

void URageInMageAbilitySystemComponent::AddCharacterAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		// Check the Ability Class is Valid
		if (!AbilityClass) continue;
		// Create the Ability Spec
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const URageInMageGameplayAbility* MageAbility = Cast<URageInMageGameplayAbility>(AbilitySpec.Ability))
		{
			// Add the Tags
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(MageAbility->StartupInputTag);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(MageAbility->StartupAbilityTag);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(MageAbility->StartupAbilityTypeTag);
			
			// Give Ability
			GiveAbility(AbilitySpec);
		}
	}
	bStartupAbilitiesGiven = true;
	
	// Broadcast Ability Given Event
	AbilitiesGivenDelegate.Broadcast(this);
}

void URageInMageAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	// Return if Input Tag is not valid
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void URageInMageAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

void URageInMageAbilitySystemComponent::ForEachAbilitySpec(const FForEachAbilitySpec& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogRageInMage, Error, TEXT("Failed to execute delegate in %s"), *FString(__FUNCTION__));
		}
	}
}

void URageInMageAbilitySystemComponent::OnAbilitiesGiven(
	URageInMageAbilitySystemComponent* RageInMageAbilitySystemComponent)
{
}

FGameplayTag URageInMageAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags())
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Ability"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag URageInMageAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayTag URageInMageAbilitySystemComponent::GetAbilityTypeTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("AbilityType"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

void URageInMageAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	
	if (!bStartupAbilitiesGiven)
	{
		AbilitiesGivenDelegate.Broadcast(this);
		bStartupAbilitiesGiven = true;
	}
}

void URageInMageAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                           const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);
}
