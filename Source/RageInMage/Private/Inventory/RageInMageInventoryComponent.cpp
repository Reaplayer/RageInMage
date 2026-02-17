// Copyright Reaplays

#include "Inventory/RageInMageInventoryComponent.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "RageInMage/RageInMageLogChannels.h"

URageInMageInventoryComponent::URageInMageInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void URageInMageInventoryComponent::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(URageInMageInventoryComponent, Items);
}

bool URageInMageInventoryComponent::HasItem(const FGuid& ItemID) const
{
	return Items.ContainsByPredicate([&ItemID](const FRageInMageInventoryItem& Item)
	{
		return Item.ItemID == ItemID;
	});
}

const FRageInMageInventoryItem* URageInMageInventoryComponent::FindItemByID(const FGuid& ItemID) const
{
	return Items.FindByPredicate([&ItemID](const FRageInMageInventoryItem& Item)
	{
		return Item.ItemID == ItemID;
	});
}

const FRageInMageInventoryItem* URageInMageInventoryComponent::FindEquippedItemInSlot(
	ERageInMageEquipSlot Slot) const
{
	return Items.FindByPredicate([Slot](const FRageInMageInventoryItem& Item)
	{
		return Item.bIsEquipped && Item.EquipSlot == Slot;
	});
}

FRageInMageInventoryItem* URageInMageInventoryComponent::FindItemByIDMutable(const FGuid& ItemID)
{
	return Items.FindByPredicate([&ItemID](FRageInMageInventoryItem& Item)
	{
		return Item.ItemID == ItemID;
	});
}

bool URageInMageInventoryComponent::ServerAddItem(const FRageInMageInventoryItem& NewItem)
{
	// Try stacking with existing items of the same type
	for (FRageInMageInventoryItem& ExistingItem : Items)
	{
		if (ExistingItem.ItemTag == NewItem.ItemTag
			&& ExistingItem.StackCount < ExistingItem.MaxStackCount
			&& !ExistingItem.bIsEquipped)
		{
			const int32 SpaceAvailable = ExistingItem.MaxStackCount - ExistingItem.StackCount;
			const int32 ToAdd = FMath::Min(NewItem.StackCount, SpaceAvailable);
			ExistingItem.StackCount += ToAdd;

			if (ToAdd >= NewItem.StackCount)
			{
				OnInventoryChangedDelegate.Broadcast();
				return true;
			}
			// Partial stack consumed; remainder falls through to add as new entry
		}
	}

	// Check capacity for new entry
	if (Items.Num() >= MaxInventorySize)
	{
		UE_LOG(LogRageInMage, Warning, TEXT("Inventory full. Cannot add item."));
		return false;
	}

	FRageInMageInventoryItem ItemToAdd = NewItem;
	if (!ItemToAdd.ItemID.IsValid())
	{
		ItemToAdd.ItemID = FGuid::NewGuid();
	}
	Items.Add(ItemToAdd);
	OnInventoryChangedDelegate.Broadcast();
	return true;
}

bool URageInMageInventoryComponent::ServerRemoveItem(const FGuid& ItemID)
{
	const int32 IndexToRemove = Items.IndexOfByPredicate([&ItemID](const FRageInMageInventoryItem& Item)
	{
		return Item.ItemID == ItemID;
	});

	if (IndexToRemove == INDEX_NONE)
	{
		UE_LOG(LogRageInMage, Warning, TEXT("Item not found for removal: %s"), *ItemID.ToString());
		return false;
	}

	Items.RemoveAt(IndexToRemove);
	OnInventoryChangedDelegate.Broadcast();
	return true;
}

bool URageInMageInventoryComponent::ServerEquipItem(const FGuid& ItemID, UAbilitySystemComponent* ASC)
{
	FRageInMageInventoryItem* Item = FindItemByIDMutable(ItemID);
	if (!Item || !Item->IsEquippable() || Item->bIsEquipped)
	{
		return false;
	}

	// Unequip existing item in this slot
	if (const FRageInMageInventoryItem* CurrentlyEquipped = FindEquippedItemInSlot(Item->EquipSlot))
	{
		ServerUnequipItem(CurrentlyEquipped->ItemID, ASC);
	}

	Item->bIsEquipped = true;
	ApplyEquipEffects(*Item, ASC);

	OnItemEquipChangedDelegate.Broadcast(ItemID, true);
	OnInventoryChangedDelegate.Broadcast();
	return true;
}

bool URageInMageInventoryComponent::ServerUnequipItem(const FGuid& ItemID, UAbilitySystemComponent* ASC)
{
	FRageInMageInventoryItem* Item = FindItemByIDMutable(ItemID);
	if (!Item || !Item->bIsEquipped)
	{
		return false;
	}

	RemoveEquipEffects(*Item, ASC);
	Item->bIsEquipped = false;

	OnItemEquipChangedDelegate.Broadcast(ItemID, false);
	OnInventoryChangedDelegate.Broadcast();
	return true;
}

bool URageInMageInventoryComponent::ServerUseItem(const FGuid& ItemID, UAbilitySystemComponent* ASC)
{
	FRageInMageInventoryItem* Item = FindItemByIDMutable(ItemID);
	if (!Item || !Item->IsConsumable())
	{
		return false;
	}

	// Apply consume effect
	if (ASC && Item->ConsumeEffect)
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(ASC->GetAvatarActor());
		const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Item->ConsumeEffect, 1.f, ContextHandle);
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}

	// Decrement stack
	Item->StackCount -= 1;
	if (Item->StackCount <= 0)
	{
		ServerRemoveItem(ItemID);
	}
	else
	{
		OnInventoryChangedDelegate.Broadcast();
	}
	return true;
}

bool URageInMageInventoryComponent::ServerDropItem(const FGuid& ItemID, UAbilitySystemComponent* ASC)
{
	FRageInMageInventoryItem* Item = FindItemByIDMutable(ItemID);
	if (!Item)
	{
		return false;
	}

	// If equipped, unequip first to remove GAS effects
	if (Item->bIsEquipped)
	{
		ServerUnequipItem(ItemID, ASC);
	}

	// TODO: Spawn world pickup actor at player location
	return ServerRemoveItem(ItemID);
}

void URageInMageInventoryComponent::ApplyEquipEffects(
	const FRageInMageInventoryItem& Item, UAbilitySystemComponent* ASC)
{
	if (!ASC) return;

	// Grant ability (avoid duplicates by checking handle map)
	if (Item.GrantedAbilityClass && !GrantedAbilityHandles.Contains(Item.ItemID))
	{
		FGameplayAbilitySpec AbilitySpec(Item.GrantedAbilityClass, 1);
		const FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(AbilitySpec);
		GrantedAbilityHandles.Add(Item.ItemID, Handle);
	}

	// Apply equip gameplay effect (infinite duration for stat bonuses)
	if (Item.EquipEffect && !ActiveEquipEffectHandles.Contains(Item.ItemID))
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(ASC->GetAvatarActor());
		const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Item.EquipEffect, 1.f, ContextHandle);
		if (SpecHandle.IsValid())
		{
			const FActiveGameplayEffectHandle ActiveHandle =
				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			ActiveEquipEffectHandles.Add(Item.ItemID, ActiveHandle);
		}
	}
}

void URageInMageInventoryComponent::RemoveEquipEffects(
	const FRageInMageInventoryItem& Item, UAbilitySystemComponent* ASC)
{
	if (!ASC) return;

	// Remove granted ability
	if (const FGameplayAbilitySpecHandle* Handle = GrantedAbilityHandles.Find(Item.ItemID))
	{
		ASC->ClearAbility(*Handle);
		GrantedAbilityHandles.Remove(Item.ItemID);
	}

	// Remove equip gameplay effect
	if (const FActiveGameplayEffectHandle* Handle = ActiveEquipEffectHandles.Find(Item.ItemID))
	{
		ASC->RemoveActiveGameplayEffect(*Handle);
		ActiveEquipEffectHandles.Remove(Item.ItemID);
	}
}

void URageInMageInventoryComponent::OnRep_Items()
{
	OnInventoryChangedDelegate.Broadcast();
}
