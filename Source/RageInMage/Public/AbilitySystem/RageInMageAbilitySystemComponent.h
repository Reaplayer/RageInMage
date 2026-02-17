// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "RageInMageAbilitySystemComponent.generated.h"

enum class ECharacterClass : uint8;
struct FRageInMageInventoryItem;
class URageInMageInventoryComponent;
class URageInMageAbilitySystemComponent;
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/)
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven)
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

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities, ECharacterClass CharacterClass);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);
	bool bStartupAbilitiesGiven = false;

	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void ForEachAbilitySpec(const FForEachAbilitySpec& Delegate);
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetAbilityTypeTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetAbilityProgressionTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	
	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);

	void SpendSpellPoint(const FGameplayTag& AbilityTag);

	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag);

	// ── Inventory Server RPCs ──

	void InventoryAddItem(const FRageInMageInventoryItem& Item);
	UFUNCTION(Server, Reliable)
	void ServerInventoryAddItem(const FRageInMageInventoryItem& Item);

	void InventoryRemoveItem(const FGuid& ItemID);
	UFUNCTION(Server, Reliable)
	void ServerInventoryRemoveItem(const FGuid& ItemID);

	void InventoryEquipItem(const FGuid& ItemID);
	UFUNCTION(Server, Reliable)
	void ServerInventoryEquipItem(const FGuid& ItemID);

	void InventoryUnequipItem(const FGuid& ItemID);
	UFUNCTION(Server, Reliable)
	void ServerInventoryUnequipItem(const FGuid& ItemID);

	void InventoryDropItem(const FGuid& ItemID);
	UFUNCTION(Server, Reliable)
	void ServerInventoryDropItem(const FGuid& ItemID);

	void InventoryUseItem(const FGuid& ItemID);
	UFUNCTION(Server, Reliable)
	void ServerInventoryUseItem(const FGuid& ItemID);

protected:
	
	virtual void OnRep_ActivateAbilities() override;
	
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec,
		FActiveGameplayEffectHandle ActiveGameplayEffectHandle);

	UFUNCTION()
	void OnAbilitiesGiven(URageInMageAbilitySystemComponent* RageInMageAbilitySystemComponent);
};
