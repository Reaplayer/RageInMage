// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/Texture2D.h"
#include "RageInMageInventoryTypes.generated.h"

class UGameplayAbility;
class UGameplayEffect;

/**
 * Item rarity levels
 */
UENUM(BlueprintType)
enum class ERageInMageItemRarity : uint8
{
	Common      UMETA(DisplayName = "Common"),
	Uncommon    UMETA(DisplayName = "Uncommon"),
	Rare        UMETA(DisplayName = "Rare"),
	Epic        UMETA(DisplayName = "Epic"),
	Legendary   UMETA(DisplayName = "Legendary")
};

/**
 * Equipment slot types
 */
UENUM(BlueprintType)
enum class ERageInMageEquipSlot : uint8
{
	None        UMETA(DisplayName = "None"),
	Head        UMETA(DisplayName = "Head"),
	Chest       UMETA(DisplayName = "Chest"),
	Legs        UMETA(DisplayName = "Legs"),
	Feet        UMETA(DisplayName = "Feet"),
	Hands       UMETA(DisplayName = "Hands"),
	MainHand    UMETA(DisplayName = "Main Hand"),
	OffHand     UMETA(DisplayName = "Off Hand"),
	Ring1       UMETA(DisplayName = "Ring 1"),
	Ring2       UMETA(DisplayName = "Ring 2"),
	Necklace    UMETA(DisplayName = "Necklace"),
	Consumable  UMETA(DisplayName = "Consumable")
};

/**
 * A single inventory item instance.
 * Replicated as part of TArray on the InventoryComponent.
 */
USTRUCT(BlueprintType)
struct RAGEINMAGE_API FRageInMageInventoryItem
{
	GENERATED_BODY()

	FRageInMageInventoryItem()
		: ItemID(FGuid())
		, ItemTag(FGameplayTag())
		, ItemName(FText::GetEmpty())
		, ItemDescription(FText::GetEmpty())
		, ItemIcon(nullptr)
		, Rarity(ERageInMageItemRarity::Common)
		, EquipSlot(ERageInMageEquipSlot::None)
		, StackCount(1)
		, MaxStackCount(1)
		, bIsEquipped(false)
		, GrantedAbilityClass(nullptr)
		, EquipEffect(nullptr)
		, ConsumeEffect(nullptr)
	{}

	/** Unique instance ID for this specific item stack */
	UPROPERTY(BlueprintReadOnly, Category = "Item")
	FGuid ItemID;

	/** Gameplay tag identifying this item type (e.g., Item.Weapon.Sword.Iron) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FGameplayTag ItemTag;

	/** Display name */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText ItemName;

	/** Display description */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText ItemDescription;

	/** Item icon texture */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UTexture2D> ItemIcon;

	/** Rarity */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	ERageInMageItemRarity Rarity;

	/** Which slot this item equips to (None = not equippable) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	ERageInMageEquipSlot EquipSlot;

	/** Current stack count */
	UPROPERTY(BlueprintReadOnly, Category = "Item")
	int32 StackCount;

	/** Max stack size */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	int32 MaxStackCount;

	/** Whether currently equipped */
	UPROPERTY(BlueprintReadOnly, Category = "Item")
	bool bIsEquipped;

	/** Ability granted when equipped (can be null) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|GAS")
	TSubclassOf<UGameplayAbility> GrantedAbilityClass;

	/** Gameplay effect applied while equipped (stat bonuses, infinite duration) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|GAS")
	TSubclassOf<UGameplayEffect> EquipEffect;

	/** Gameplay effect applied on consumption */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|GAS")
	TSubclassOf<UGameplayEffect> ConsumeEffect;

	bool IsConsumable() const { return EquipSlot == ERageInMageEquipSlot::Consumable; }
	bool IsEquippable() const { return EquipSlot != ERageInMageEquipSlot::None && EquipSlot != ERageInMageEquipSlot::Consumable; }
	bool IsValid() const { return ItemID.IsValid(); }

	bool operator==(const FRageInMageInventoryItem& Other) const
	{
		return ItemID == Other.ItemID;
	}
};