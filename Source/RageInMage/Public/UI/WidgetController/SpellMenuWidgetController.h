// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TabbedMenuWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

struct FRageInMageAbilityInfo;

/**
 * Controls the spell menu UI (spell selection, leveling)
 * Responsible ONLY for spell menu logic
 */
UCLASS(Blueprintable)
class RAGEINMAGE_API USpellMenuWidgetController : public UTabbedMenuWidgetController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnPlayerStatChangedSignature SpellPointsChangedDelegate;
	
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;

	UFUNCTION(BlueprintCallable, Category = "Spells")
	void SpendSpellPoint(const FGameplayTag& AbilityTag);

protected:
	

private:
	void BroadcastAllAbilities();
};