// Copyright Reaplays

#include "UI/WidgetController/InventoryWidgetController.h"
#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "Inventory/RageInMageInventoryComponent.h"
#include "Player/RageInMagePlayerState.h"
#include "RageInMage/RageInMageLogChannels.h"

void UInventoryWidgetController::BroadcastInitialValues()
{
	URageInMageInventoryComponent* InvComp = GetInventoryComponent();
	check(InvComp);

	OnInventoryUpdatedDelegate.Broadcast(InvComp->GetItems());
}

void UInventoryWidgetController::BindCallbacksToDependencies()
{
	URageInMageInventoryComponent* InvComp = GetInventoryComponent();
	check(InvComp);

	InventoryChangedHandle = InvComp->OnInventoryChangedDelegate.AddUObject(
		this, &UInventoryWidgetController::OnInventoryChanged);

	ItemEquipChangedHandle = InvComp->OnItemEquipChangedDelegate.AddUObject(
		this, &UInventoryWidgetController::OnItemEquipChanged);
}

void UInventoryWidgetController::UnbindFromDependencies()
{
	if (URageInMageInventoryComponent* InvComp = GetInventoryComponent())
	{
		InvComp->OnInventoryChangedDelegate.Remove(InventoryChangedHandle);
		InvComp->OnItemEquipChangedDelegate.Remove(ItemEquipChangedHandle);
	}
}

void UInventoryWidgetController::BeginDestroy()
{
	UnbindFromDependencies();
	Super::BeginDestroy();
}

void UInventoryWidgetController::OnInventoryChanged()
{
	if (URageInMageInventoryComponent* InvComp = GetInventoryComponent())
	{
		OnInventoryUpdatedDelegate.Broadcast(InvComp->GetItems());
	}
}

void UInventoryWidgetController::OnItemEquipChanged(const FGuid& ItemID, bool bEquipped)
{
	OnItemEquipStateChangedDelegate.Broadcast(ItemID, bEquipped);
}

// ── Action requests (client-side validation then Server RPC) ──

void UInventoryWidgetController::RequestAddItem(const FRageInMageInventoryItem& Item)
{
	URageInMageAbilitySystemComponent* ASC = GetRageASC();
	check(ASC);

	URageInMageInventoryComponent* InvComp = GetInventoryComponent();
	if (!InvComp)
	{
		OnInventoryMessageDelegate.Broadcast(FText::FromString(TEXT("Inventory not available.")));
		return;
	}

	if (InvComp->IsInventoryFull())
	{
		OnInventoryMessageDelegate.Broadcast(FText::FromString(TEXT("Inventory is full.")));
		return;
	}

	ASC->ServerInventoryAddItem(Item);
}

void UInventoryWidgetController::RequestRemoveItem(const FGuid& ItemID)
{
	URageInMageAbilitySystemComponent* ASC = GetRageASC();
	check(ASC);

	URageInMageInventoryComponent* InvComp = GetInventoryComponent();
	if (!InvComp || !InvComp->HasItem(ItemID))
	{
		OnInventoryMessageDelegate.Broadcast(FText::FromString(TEXT("Item not found.")));
		return;
	}

	ASC->ServerInventoryRemoveItem(ItemID);
}

void UInventoryWidgetController::RequestEquipItem(const FGuid& ItemID)
{
	URageInMageAbilitySystemComponent* ASC = GetRageASC();
	check(ASC);

	URageInMageInventoryComponent* InvComp = GetInventoryComponent();
	if (!InvComp)
	{
		return;
	}

	const FRageInMageInventoryItem* Item = InvComp->FindItemByID(ItemID);
	if (!Item)
	{
		OnInventoryMessageDelegate.Broadcast(FText::FromString(TEXT("Item not found.")));
		return;
	}

	if (!Item->IsEquippable())
	{
		OnInventoryMessageDelegate.Broadcast(FText::FromString(TEXT("This item cannot be equipped.")));
		return;
	}

	if (Item->bIsEquipped)
	{
		OnInventoryMessageDelegate.Broadcast(FText::FromString(TEXT("Item is already equipped.")));
		return;
	}

	ASC->ServerInventoryEquipItem(ItemID);
}

void UInventoryWidgetController::RequestUnequipItem(const FGuid& ItemID)
{
	URageInMageAbilitySystemComponent* ASC = GetRageASC();
	check(ASC);

	URageInMageInventoryComponent* InvComp = GetInventoryComponent();
	if (!InvComp)
	{
		return;
	}

	const FRageInMageInventoryItem* Item = InvComp->FindItemByID(ItemID);
	if (!Item || !Item->bIsEquipped)
	{
		OnInventoryMessageDelegate.Broadcast(FText::FromString(TEXT("Item is not equipped.")));
		return;
	}

	ASC->ServerInventoryUnequipItem(ItemID);
}

void UInventoryWidgetController::RequestDropItem(const FGuid& ItemID)
{
	URageInMageAbilitySystemComponent* ASC = GetRageASC();
	check(ASC);

	URageInMageInventoryComponent* InvComp = GetInventoryComponent();
	if (!InvComp || !InvComp->HasItem(ItemID))
	{
		OnInventoryMessageDelegate.Broadcast(FText::FromString(TEXT("Item not found.")));
		return;
	}

	ASC->ServerInventoryDropItem(ItemID);
}

void UInventoryWidgetController::RequestUseItem(const FGuid& ItemID)
{
	URageInMageAbilitySystemComponent* ASC = GetRageASC();
	check(ASC);

	URageInMageInventoryComponent* InvComp = GetInventoryComponent();
	if (!InvComp)
	{
		return;
	}

	const FRageInMageInventoryItem* Item = InvComp->FindItemByID(ItemID);
	if (!Item)
	{
		OnInventoryMessageDelegate.Broadcast(FText::FromString(TEXT("Item not found.")));
		return;
	}

	if (!Item->IsConsumable())
	{
		OnInventoryMessageDelegate.Broadcast(FText::FromString(TEXT("This item cannot be used.")));
		return;
	}

	ASC->ServerInventoryUseItem(ItemID);
}

// ── Read-only queries ──

const TArray<FRageInMageInventoryItem>& UInventoryWidgetController::GetInventoryItems() const
{
	static TArray<FRageInMageInventoryItem> Empty;
	if (InventoryComponent)
	{
		return InventoryComponent->GetItems();
	}
	return Empty;
}

bool UInventoryWidgetController::IsInventoryFull() const
{
	return InventoryComponent ? InventoryComponent->IsInventoryFull() : true;
}

int32 UInventoryWidgetController::GetMaxInventorySize() const
{
	return InventoryComponent ? InventoryComponent->GetMaxInventorySize() : 0;
}

URageInMageInventoryComponent* UInventoryWidgetController::GetInventoryComponent()
{
	if (!InventoryComponent)
	{
		ARageInMagePlayerState* PS = GetRagePS();
		if (PS)
		{
			InventoryComponent = PS->GetInventoryComponent();
		}
	}
	return InventoryComponent;
}
