// Copyright Krymson Reaplays

#include "UI/WidgetController/TabbedMenuWidgetController.h"

void UTabbedMenuWidgetController::BindCallbacksToDependencies()
{
	RageWidgetController->SetTabbedMenuWidgetController(this);
}

void UTabbedMenuWidgetController::BroadcastInitialValues()
{
	// TabbedMenu doesn't have its own initial values to broadcast
	// Child controllers (SpellMenu, AttributeMenu) handle their own
}

void UTabbedMenuWidgetController::SetAttributePoints(int32 Points)
{
	AttributePoints = Points;
	OnAttributePointsSpent.Broadcast(AttributePoints);
}

void UTabbedMenuWidgetController::SetSpellPoints(int32 Points)
{
	SpellPoints = Points;
	OnSpellPointsSpent.Broadcast(SpellPoints);
}

void UTabbedMenuWidgetController::NotifySpellEquipped(const FGameplayTag& SpellTag, const FGameplayTag& SlotTag)
{
	OnSpellEquipped.Broadcast(SpellTag, SlotTag);
}
