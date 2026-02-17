// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayAbilitySpecHandle.h"
#include "ActiveGameplayEffectHandle.h"
#include "Inventory/RageInMageInventoryTypes.h"
#include "RageInMageInventoryComponent.generated.h"

class UAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE(FOnInventoryChanged);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemEquipChanged, const FGuid& /*ItemID*/, bool /*bEquipped*/);

/**
 * Server-authoritative inventory component.
 * Lives on PlayerState alongside ASC and AttributeSet.
 *
 * All mutations happen only on the server.
 * The Items array is replicated; OnRep_Items fires on clients,
 * which broadcasts FOnInventoryChanged so widget controllers can update UI.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RAGEINMAGE_API URageInMageInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URageInMageInventoryComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ── Delegates (non-dynamic, for C++ controllers) ──
	FOnInventoryChanged OnInventoryChangedDelegate;
	FOnItemEquipChanged OnItemEquipChangedDelegate;

	// ── Read-only accessors ──

	UFUNCTION(BlueprintPure, Category = "Inventory")
	const TArray<FRageInMageInventoryItem>& GetItems() const { return Items; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetItemCount() const { return Items.Num(); }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasItem(const FGuid& ItemID) const;

	const FRageInMageInventoryItem* FindItemByID(const FGuid& ItemID) const;

	const FRageInMageInventoryItem* FindEquippedItemInSlot(ERageInMageEquipSlot Slot) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetMaxInventorySize() const { return MaxInventorySize; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsInventoryFull() const { return Items.Num() >= MaxInventorySize; }

	// ── Server-only mutations ──
	// Called from ASC Server RPC _Implementations.

	bool ServerAddItem(const FRageInMageInventoryItem& NewItem);
	bool ServerRemoveItem(const FGuid& ItemID);
	bool ServerEquipItem(const FGuid& ItemID, UAbilitySystemComponent* ASC);
	bool ServerUnequipItem(const FGuid& ItemID, UAbilitySystemComponent* ASC);
	bool ServerUseItem(const FGuid& ItemID, UAbilitySystemComponent* ASC);
	bool ServerDropItem(const FGuid& ItemID, UAbilitySystemComponent* ASC);

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Items)
	TArray<FRageInMageInventoryItem> Items;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 MaxInventorySize = 40;

	UFUNCTION()
	void OnRep_Items();

private:
	FRageInMageInventoryItem* FindItemByIDMutable(const FGuid& ItemID);

	void ApplyEquipEffects(const FRageInMageInventoryItem& Item, UAbilitySystemComponent* ASC);
	void RemoveEquipEffects(const FRageInMageInventoryItem& Item, UAbilitySystemComponent* ASC);

	/** Tracks granted ability handles per item for clean removal */
	TMap<FGuid, FGameplayAbilitySpecHandle> GrantedAbilityHandles;

	/** Tracks active GE handles per item for clean removal */
	TMap<FGuid, FActiveGameplayEffectHandle> ActiveEquipEffectHandles;
};
