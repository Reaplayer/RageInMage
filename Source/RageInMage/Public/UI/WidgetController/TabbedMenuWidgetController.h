// Copyright Krymson Reaplays

#pragma once

#include "CoreMinimal.h"
#include "RageInMageWidgetControllerBase.h"
#include "TabbedMenuWidgetController.generated.h"

// Delegates for cross-tab communication
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributePointsSpentSignature, int32, RemainingPoints);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpellPointsSpentSignature, int32, RemainingPoints);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpellEquippedSignature, FGameplayTag, SpellTag, FGameplayTag, SlotTag);

/**
 * Parent controller for tabbed menus (Attributes, Spells, Inventory)
 * Handles shared state and communication between child controllers
 */
UCLASS(BlueprintType, Blueprintable)
class RAGEINMAGE_API UTabbedMenuWidgetController : public URageInMageWidgetControllerBase
{
	GENERATED_BODY()

public:


protected:
	
};
