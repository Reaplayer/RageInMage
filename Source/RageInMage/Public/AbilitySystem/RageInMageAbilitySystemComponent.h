// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "RageInMageAbilitySystemComponent.generated.h"

class URageInMageAbilitySystemComponent;
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilitiesGiven, URageInMageAbilitySystemComponent* /*AbilitySystemComponent*/)
DECLARE_DELEGATE_OneParam(FForEachAbilitySpec, FGameplayAbilitySpec& /*AbilitySpec*/)

/**
 * @class URageInMageAbilitySystemComponent
 * @brief A specialized extension of UAbilitySystemComponent, customized for the Rage in Mage game,
 *        which manages abilities and gameplay effect processing for actors.
 *
 * This class adds functionality for handling character abilities, Ability Input tags,
 * and various Gameplay Effects and Ability-related callbacks.
 */
UCLASS()
class RAGEINMAGE_API URageInMageAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AbilityActorInfoSet();

	FEffectAssetTags EffectAssetTags;
	FAbilitiesGiven AbilitiesGivenDelegate;

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);
	bool bStartupAbilitiesGiven = false;

	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void ForEachAbilitySpec(const FForEachAbilitySpec& Delegate);
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetAbilityTypeTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	
	void UpgradeAttribute(const FGameplayTag& AttributeTag);
	
	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);

protected:
	
	virtual void OnRep_ActivateAbilities() override;
	
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec,
		FActiveGameplayEffectHandle ActiveGameplayEffectHandle);

	UFUNCTION()
	void OnAbilitiesGiven(URageInMageAbilitySystemComponent* RageInMageAbilitySystemComponent);
};
