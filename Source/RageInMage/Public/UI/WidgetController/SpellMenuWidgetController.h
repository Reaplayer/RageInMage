// Copyright Krymson Reaplays

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/TabbedMenuWidgetController.h"
#include "GameplayTagContainer.h"
#include "SpellMenuWidgetController.generated.h"

struct FRageInMageAbilityInfo;
class UAbilityInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellGlobeSelectedSignature, bool, bSpendPointsButtonEnabled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipSelectionSignature, const FGameplayTag&, AbilityType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellGlobeReassignedSignature, const FGameplayTag&, AbilityTag);

/**
 * Controller for the Spell Menu tab
 * Handles spell selection, equipping, and point spending
 */
UCLASS(BlueprintType, Blueprintable)
class RAGEINMAGE_API USpellMenuWidgetController : public UTabbedMenuWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Spells")
	FOnPlayerStatChangedSignature SpellPointsChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Spells")
	FSpellGlobeSelectedSignature SpellGlobeSelectedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Spells")
	FWaitForEquipSelectionSignature WaitForEquipDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Spells")
	FWaitForEquipSelectionSignature StopWaitingForEquipDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Spells")
	FSpellGlobeReassignedSignature SpellGlobeReassignedDelegate;

	// Called when a spell globe is clicked in the UI
	UFUNCTION(BlueprintCallable, Category = "SpellMenu")
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);

	// Called when spending a spell point on an ability
	UFUNCTION(BlueprintCallable, Category = "SpellMenu")
	void SpendPointButtonPressed();

	// Called when equipping a spell to a globe slot
	UFUNCTION(BlueprintCallable, Category = "SpellMenu")
	void EquipButtonPressed();

	// Called when a spell globe is deselected
	UFUNCTION(BlueprintCallable, Category = "SpellMenu")
	void SpellGlobeDeselect();

	// Called when selecting which slot to equip the spell to
	UFUNCTION(BlueprintCallable, Category = "SpellMenu")
	void SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType);

	// Access spell points from parent or local state
	UFUNCTION(BlueprintCallable, Category = "SpellMenu")
	int32 GetCurrentSpellPoints() const { return GetSpellPoints(); }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "SpellMenu")
	TObjectPtr<UAbilityInfo> AbilityInfo;

private:
	FGameplayTag SelectedAbility;
	int32 CurrentSpellPoints = 0;
	bool bWaitingForEquipSelection = false;

	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot);
};
