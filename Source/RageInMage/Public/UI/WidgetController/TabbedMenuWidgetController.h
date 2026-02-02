// Copyright Krymson Reaplays

#pragma once

#include "CoreMinimal.h"
#include "RageInMageWidgetController.h"
#include "TabbedMenuWidgetController.generated.h"

class USpellMenuWidgetController;
class UAttributeMenuWidgetController;
// Delegates for cross-tab communication
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributePointsSpentSignature, int32, RemainingPoints);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpellPointsSpentSignature, int32, RemainingPoints);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpellEquippedSignature, FGameplayTag, SpellTag, FGameplayTag, SlotTag);

/**
 * Parent controller for tabbed menus (Attributes, Spells, Inventory)
 * Handles shared state and communication between child controllers
 */
UCLASS(BlueprintType, Blueprintable)
class RAGEINMAGE_API UTabbedMenuWidgetController : public URageInMageWidgetController
{
	GENERATED_BODY()

public:
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;
	
	// Shared delegates that child controllers can broadcast to
	UPROPERTY(BlueprintAssignable, Category = "TabbedMenu|Points")
	FOnAttributePointsSpentSignature OnAttributePointsSpent;

	UPROPERTY(BlueprintAssignable, Category = "TabbedMenu|Points")
	FOnSpellPointsSpentSignature OnSpellPointsSpent;

	UPROPERTY(BlueprintAssignable, Category = "TabbedMenu|Spells")
	FOnSpellEquippedSignature OnSpellEquipped;

	// Shared state accessors for child controllers
	UFUNCTION(BlueprintCallable, Category = "TabbedMenu")
	int32 GetAttributePoints() const { return AttributePoints; }

	UFUNCTION(BlueprintCallable, Category = "TabbedMenu")
	int32 GetSpellPoints() const { return SpellPoints; }

	// Methods for child controllers to update shared state
	UFUNCTION(BlueprintCallable, Category = "TabbedMenu")
	void SetAttributePoints(int32 Points);

	UFUNCTION(BlueprintCallable, Category = "TabbedMenu")
	void SetSpellPoints(int32 Points);

	UFUNCTION(BlueprintPure)
	USpellMenuWidgetController* GetSpellMenuWidgetController() const { return SpellMenuWidgetController; }
	
	UFUNCTION(BlueprintPure)
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController() const { return AttributeMenuWidgetController; }
	
	UFUNCTION(BlueprintCallable)
	void SetSpellMenuWidgetController(USpellMenuWidgetController* Controller) { SpellMenuWidgetController = Controller; }
	
	UFUNCTION(BlueprintCallable)
	void SetAttributeMenuWidgetController(UAttributeMenuWidgetController* Controller) { AttributeMenuWidgetController = Controller; }

	// Notify methods for cross-tab communication
	UFUNCTION(BlueprintCallable, Category = "TabbedMenu")
	void NotifySpellEquipped(const FGameplayTag& SpellTag, const FGameplayTag& SlotTag);

protected:
	// Shared state between tabs
	UPROPERTY(BlueprintReadOnly, Category = "TabbedMenu")
	int32 AttributePoints = 0;

	UPROPERTY(BlueprintReadOnly, Category = "TabbedMenu")
	int32 SpellPoints = 0;
	
	UPROPERTY()
	USpellMenuWidgetController* SpellMenuWidgetController;

	UPROPERTY()
	UAttributeMenuWidgetController* AttributeMenuWidgetController;
};
