// Copyright Reaplays


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "Player/RageInMagePlayerState.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "UI/WidgetController/OverlayWidgetController.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	check(AttributeInfo);

	for (auto& Pair : RageAS->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda
		([this, Pair](const FOnAttributeChangeData& Data)
		{
			BroadcastAttributeInfo(Pair.Key, Pair.Value());
		}
		);
	}

	RagePlayerState->OnAttributePointsChangedDelegate.AddLambda([this](int32 Points)
	{
		AttributePointsChangedDelegate.Broadcast(Points);
		// Update parent's shared state
		SetAttributePoints(Points);
	});
	RagePlayerState->OnSpellPointsChangedDelegate.AddLambda([this](int32 Points)
	{
		SpellPointsChangedDelegate.Broadcast(Points);
		// Update parent's shared state
		SetSpellPoints(Points);
	});

	// Get TabbedMenuWidgetController from RageWidgetController if not already set
	if (!TabbedMenuWidgetController && RageWidgetController)
	{
		TabbedMenuWidgetController = RageWidgetController->GetTabbedMenuWidgetController();
	}

	if (TabbedMenuWidgetController)
	{
		TabbedMenuWidgetController->SetAttributeMenuWidgetController(this);
		SpellMenuWidgetController = TabbedMenuWidgetController->GetSpellMenuWidgetController();
	}
	RageWidgetController->BroadcastStoredBGXPStyle();
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	check(AttributeInfo);
	
	for (auto& Pair : RageAS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
	
	const int32 CurrentAttributePoints = RagePlayerState->GetAttributePoints();
	const int32 CurrentSpellPoints = RagePlayerState->GetSpellPoints();

	AttributePointsChangedDelegate.Broadcast(CurrentAttributePoints);
	SpellPointsChangedDelegate.Broadcast(CurrentSpellPoints);

	// Initialize parent's shared state
	SetAttributePoints(CurrentAttributePoints);
	SetSpellPoints(CurrentSpellPoints);
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	RageASC->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::SpendSpellPoint(const FGameplayTag& AttributeTypeTag)
{
	
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
                                                            const FGameplayAttribute& Attribute)
{
	FRageInMageAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = 	Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
