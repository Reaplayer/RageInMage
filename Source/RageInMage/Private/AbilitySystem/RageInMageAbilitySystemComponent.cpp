// Copyright Reaplays


#include "AbilitySystem/RageInMageAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Abilities/RageInMageGameplayAbility.h"
#include "GameFramework/PlayerState.h"
#include "Interaction/PlayerInterface.h"
#include "Inventory/RageInMageInventoryComponent.h"
#include "Inventory/RageInMageInventoryTypes.h"
#include "RageInMage/RageInMageLogChannels.h"
#include "RageInMageGameplayTag.h"

namespace
{
	URageInMageInventoryComponent* GetInventoryComponentFromASC(UAbilitySystemComponent* ASC)
	{
		if (!ASC || !ASC->GetAvatarActor()) return nullptr;
		if (const APawn* Pawn = Cast<APawn>(ASC->GetAvatarActor()))
		{
			if (APlayerState* PS = Pawn->GetPlayerState())
			{
				return PS->FindComponentByClass<URageInMageInventoryComponent>();
			}
		}
		return nullptr;
	}
}

void URageInMageAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &URageInMageAbilitySystemComponent::ClientEffectApplied);
}

void URageInMageAbilitySystemComponent::AddCharacterAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities, ECharacterClass CharacterClass)
{
	const FRageInMageGameplayTag& GameplayTag = FRageInMageGameplayTag::Get();
	const TMap<FGameplayTag, FGameplayTag>* TypeMap = GameplayTag.GetAbilityTypeMapForClass(CharacterClass);

	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		// Check the Ability Class is Valid
		if (!AbilityClass) continue;
		// Create the Ability Spec
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const URageInMageGameplayAbility* MageAbility = Cast<URageInMageGameplayAbility>(AbilitySpec.Ability))
		{
			// Add Input and Ability Tags
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(MageAbility->StartupInputTag);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(MageAbility->StartupAbilityTag);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(FRageInMageGameplayTag::Get().Ability_Progression_Unlocked);

			// Override AbilityTypeTag from the class's school map (single source of truth)
			FGameplayTag AbilityTypeTag = MageAbility->StartupAbilityTypeTag; // fallback to BP default
			if (TypeMap)
			{
				for (const auto& [TypeTag, AbilityTag] : *TypeMap)
				{
					if (AbilityTag.MatchesTagExact(MageAbility->StartupAbilityTag))
					{
						AbilityTypeTag = TypeTag;
						break;
					}
				}
			}
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityTypeTag);

			// Give Ability
			GiveAbility(AbilitySpec);
		}
	}
	bStartupAbilitiesGiven = true;

	// Broadcast Ability Given Event
	AbilitiesGivenDelegate.Broadcast();
}

void URageInMageAbilitySystemComponent::AddCharacterPassiveAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
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
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Ability.Type"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayTag URageInMageAbilitySystemComponent::GetAbilityProgressionTagFromSpec(
	const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Ability.Progression"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

void URageInMageAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			ServerUpgradeAttribute(AttributeTag);
		}
	}
}

void URageInMageAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.0f;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}
}

void URageInMageAbilitySystemComponent::SpendSpellPoint(const FGameplayTag& AbilityTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetSpellPoints(GetAvatarActor()) > 0)
		{
			ServerSpendSpellPoint(AbilityTag);
		}
	}
}

void URageInMageAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetSpellPoints(GetAvatarActor()) > 0)
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}
	}
}

// ── Inventory RPCs ──

void URageInMageAbilitySystemComponent::InventoryAddItem(const FRageInMageInventoryItem& Item)
{
	ServerInventoryAddItem(Item);
}

void URageInMageAbilitySystemComponent::ServerInventoryAddItem_Implementation(const FRageInMageInventoryItem& Item)
{
	URageInMageInventoryComponent* InvComp = GetInventoryComponentFromASC(this);
	if (!InvComp)
	{
		UE_LOG(LogRageInMage, Error, TEXT("ServerInventoryAddItem: No inventory component"));
		return;
	}
	InvComp->ServerAddItem(Item);
}

void URageInMageAbilitySystemComponent::InventoryRemoveItem(const FGuid& ItemID)
{
	ServerInventoryRemoveItem(ItemID);
}

void URageInMageAbilitySystemComponent::ServerInventoryRemoveItem_Implementation(const FGuid& ItemID)
{
	URageInMageInventoryComponent* InvComp = GetInventoryComponentFromASC(this);
	if (!InvComp)
	{
		UE_LOG(LogRageInMage, Error, TEXT("ServerInventoryRemoveItem: No inventory component"));
		return;
	}
	InvComp->ServerRemoveItem(ItemID);
}

void URageInMageAbilitySystemComponent::InventoryEquipItem(const FGuid& ItemID)
{
	ServerInventoryEquipItem(ItemID);
}

void URageInMageAbilitySystemComponent::ServerInventoryEquipItem_Implementation(const FGuid& ItemID)
{
	URageInMageInventoryComponent* InvComp = GetInventoryComponentFromASC(this);
	if (!InvComp)
	{
		UE_LOG(LogRageInMage, Error, TEXT("ServerInventoryEquipItem: No inventory component"));
		return;
	}
	const FRageInMageInventoryItem* Item = InvComp->FindItemByID(ItemID);
	if (!Item || !Item->IsEquippable() || Item->bIsEquipped)
	{
		return;
	}
	InvComp->ServerEquipItem(ItemID, this);
}

void URageInMageAbilitySystemComponent::InventoryUnequipItem(const FGuid& ItemID)
{
	ServerInventoryUnequipItem(ItemID);
}

void URageInMageAbilitySystemComponent::ServerInventoryUnequipItem_Implementation(const FGuid& ItemID)
{
	URageInMageInventoryComponent* InvComp = GetInventoryComponentFromASC(this);
	if (!InvComp)
	{
		UE_LOG(LogRageInMage, Error, TEXT("ServerInventoryUnequipItem: No inventory component"));
		return;
	}
	const FRageInMageInventoryItem* Item = InvComp->FindItemByID(ItemID);
	if (!Item || !Item->bIsEquipped)
	{
		return;
	}
	InvComp->ServerUnequipItem(ItemID, this);
}

void URageInMageAbilitySystemComponent::InventoryDropItem(const FGuid& ItemID)
{
	ServerInventoryDropItem(ItemID);
}

void URageInMageAbilitySystemComponent::ServerInventoryDropItem_Implementation(const FGuid& ItemID)
{
	URageInMageInventoryComponent* InvComp = GetInventoryComponentFromASC(this);
	if (!InvComp)
	{
		UE_LOG(LogRageInMage, Error, TEXT("ServerInventoryDropItem: No inventory component"));
		return;
	}
	if (!InvComp->HasItem(ItemID))
	{
		return;
	}
	InvComp->ServerDropItem(ItemID, this);
}

void URageInMageAbilitySystemComponent::InventoryUseItem(const FGuid& ItemID)
{
	ServerInventoryUseItem(ItemID);
}

void URageInMageAbilitySystemComponent::ServerInventoryUseItem_Implementation(const FGuid& ItemID)
{
	URageInMageInventoryComponent* InvComp = GetInventoryComponentFromASC(this);
	if (!InvComp)
	{
		UE_LOG(LogRageInMage, Error, TEXT("ServerInventoryUseItem: No inventory component"));
		return;
	}
	const FRageInMageInventoryItem* Item = InvComp->FindItemByID(ItemID);
	if (!Item || !Item->IsConsumable())
	{
		return;
	}
	InvComp->ServerUseItem(ItemID, this);
}

void URageInMageAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	
	if (!bStartupAbilitiesGiven)
	{
		AbilitiesGivenDelegate.Broadcast();
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
