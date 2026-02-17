// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Inventory/RageInMageInventoryTypes.h"
#include "TabbedMenuWidgetController.h"
#include "InventoryWidgetController.generated.h"

class URageInMageInventoryComponent;

/** Broadcasts the full inventory list (initial values and full refresh) */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnInventoryUpdatedSignature,
	const TArray<FRageInMageInventoryItem>&, Items);

/** Broadcasts when a single item's equip state changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnItemEquipStateChangedSignature,
	const FGuid&, ItemID,
	bool, bIsEquipped);

/** Broadcasts a status message (e.g., "Inventory full", "Cannot equip") */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnInventoryMessageSignature,
	const FText&, Message);

/**
 * Controls the inventory UI.
 * Reads from URageInMageInventoryComponent on PlayerState.
 * Routes user actions through ASC Server RPCs.
 * Siblings: UAttributeMenuWidgetController, USpellMenuWidgetController.
 */
UCLASS(BlueprintType, Blueprintable)
class RAGEINMAGE_API UInventoryWidgetController : public UTabbedMenuWidgetController
{
	GENERATED_BODY()

public:
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;
	virtual void BeginDestroy() override;

	// ── Delegates for UI binding ──

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdatedSignature OnInventoryUpdatedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemEquipStateChangedSignature OnItemEquipStateChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryMessageSignature OnInventoryMessageDelegate;

	// ── BlueprintCallable actions (all route to Server RPCs) ──

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RequestAddItem(const FRageInMageInventoryItem& Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RequestRemoveItem(const FGuid& ItemID);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RequestEquipItem(const FGuid& ItemID);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RequestUnequipItem(const FGuid& ItemID);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RequestDropItem(const FGuid& ItemID);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RequestUseItem(const FGuid& ItemID);

	// ── Read-only queries for UI ──

	UFUNCTION(BlueprintPure, Category = "Inventory")
	const TArray<FRageInMageInventoryItem>& GetInventoryItems() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsInventoryFull() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetMaxInventorySize() const;

private:
	void OnInventoryChanged();
	void OnItemEquipChanged(const FGuid& ItemID, bool bEquipped);
	void UnbindFromDependencies();

	UPROPERTY()
	TObjectPtr<URageInMageInventoryComponent> InventoryComponent;

	URageInMageInventoryComponent* GetInventoryComponent();

	FDelegateHandle InventoryChangedHandle;
	FDelegateHandle ItemEquipChangedHandle;
};
