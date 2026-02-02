// Copyright Krymson Reaplays

#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "Player/RageInMagePlayerState.h"
#include "RageInMageGameplayTag.h"
#include "UI/HUD/RageInMageHUD.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	if (RagePlayerState)
	{
		const int32 Points = RagePlayerState->GetSpellPoints();
		SpellPointsChanged.Broadcast(Points);
		CurrentSpellPoints = Points;

		// Update parent's shared state
		SetSpellPoints(Points);
	}
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	//if (RageASC)
	{
		//RageInMageASC->AbilityEquipped.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);
	}


	if (RagePlayerState)
	{
		RagePlayerState->OnSpellPointsChangedDelegate.AddLambda([this](int32 Points)
		{
			SpellPointsChanged.Broadcast(Points);
			CurrentSpellPoints = Points;

			// Update parent's shared state so AttributeMenu knows about it
			SetSpellPoints(Points);
		});
	}

	// Get TabbedMenuWidgetController from RageWidgetController if not already set
	if (!TabbedMenuWidgetController && RageWidgetController)
	{
		TabbedMenuWidgetController = RageWidgetController->GetTabbedMenuWidgetController();
	}

	if (TabbedMenuWidgetController)
	{
		TabbedMenuWidgetController->SetSpellMenuWidgetController(this);
		AttributeMenuWidgetController = TabbedMenuWidgetController->GetAttributeMenuWidgetController();
	}
	RageWidgetController->BroadcastStoredBGXPStyle();
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	if (bWaitingForEquipSelection)
	{
		if (RageASC)
		{
			//const FGameplayTag SelectedAbilityType = RageInMageASC->GetAbilityTypeTagFromSpec(RageInMageASC->GetSpecFromAbilityTag(AbilityTag));
			//StopWaitingForEquipDelegate.Broadcast(SelectedAbilityType);
			bWaitingForEquipSelection = false;
		}
	}

	SelectedAbility = AbilityTag;

	// Check if we can spend points on this ability
	const bool bCanSpendPoints = CurrentSpellPoints > 0;
	SpellGlobeSelectedDelegate.Broadcast(bCanSpendPoints);
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	if (RageASC)
	{
		//RageInMageASC->ServerSpendSpellPoint(SelectedAbility);
	}
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	if (RageASC)
	{
		//const FGameplayTag AbilityType = RageInMageASC->GetAbilityTypeTagFromSpec(RageInMageASC->GetSpecFromAbilityTag(SelectedAbility));

		//WaitForEquipDelegate.Broadcast(AbilityType);
		bWaitingForEquipSelection = true;
	}
}

void USpellMenuWidgetController::SpellGlobeDeselect()
{
	if (bWaitingForEquipSelection)
	{
		if (RageASC)
		{
			//const FGameplayTag SelectedAbilityType = RageInMageASC->GetAbilityTagFromSpec(RageInMageASC->GetSpecFromAbilityTag(SelectedAbility));
			//StopWaitingForEquipDelegate.Broadcast(SelectedAbilityType);
			bWaitingForEquipSelection = false;
		}
	}

	SelectedAbility = FGameplayTag();
	SpellGlobeSelectedDelegate.Broadcast(false);
}

void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
	if (!bWaitingForEquipSelection) return;

	if (RageASC)
	{
		//RageInMageASC->ServerEquipAbility(SelectedAbility, SlotTag);
	}

	StopWaitingForEquipDelegate.Broadcast(AbilityType);
	bWaitingForEquipSelection = false;
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,
	const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	// Notify parent controller so other tabs can react
	NotifySpellEquipped(AbilityTag, Slot);

	// Broadcast to this controller's widgets
	SpellGlobeReassignedDelegate.Broadcast(AbilityTag);
}

