// Copyright Reaplays


#include "UI/HUD/RageInMageHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/Widget/RageInMageUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/WidgetController/TabbedMenuWidgetController.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"

URageInMageWidgetController* ARageInMageHUD::GetRageWidgetController(const FRageInMageWidgetControllerParams& WCParams)
{
	// Get or create per-player widget controller for local multiplayer support
	if (TObjectPtr<URageInMageWidgetController>* FoundController = RageWidgetControllers.Find(WCParams.PlayerController))
	{
		return FoundController->Get();
	}

	// Create the main shared controller that all others will reference
	URageInMageWidgetController* SharedController = NewObject<URageInMageWidgetController>(this, RageWidgetControllerClass);
	SharedController->SetWidgetControllerParams(WCParams);

	// Create all child controllers and make them share the same RageWidgetController
	if (TabbedMenuWidgetControllerClass)
	{
		UTabbedMenuWidgetController* TabbedController = NewObject<UTabbedMenuWidgetController>(this, TabbedMenuWidgetControllerClass);
		TabbedController->SetWidgetControllerParams(WCParams);
		TabbedController->SetRageWidgetController(SharedController); // Share the same instance!
		TabbedMenuControllers.Add(WCParams.PlayerController, TabbedController);

		// Create AttributeMenu controller
		if (AttributeMenuWidgetControllerClass)
		{
			UAttributeMenuWidgetController* AttributeController = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
			AttributeController->SetWidgetControllerParams(WCParams);
			AttributeController->SetRageWidgetController(SharedController); // Share the same instance!
			AttributeMenuControllers.Add(WCParams.PlayerController, AttributeController);

			// Link to parent
			TabbedController->SetAttributeMenuWidgetController(AttributeController);
		}

		// Create SpellMenu controller
		if (SpellMenuWidgetControllerClass)
		{
			USpellMenuWidgetController* SpellController = NewObject<USpellMenuWidgetController>(this, SpellMenuWidgetControllerClass);
			SpellController->SetWidgetControllerParams(WCParams);
			SpellController->SetRageWidgetController(SharedController); // Share the same instance!
			SpellMenuControllers.Add(WCParams.PlayerController, SpellController);

			// Link to parent
			TabbedController->SetSpellMenuWidgetController(SpellController);
		}

		// Initialize TabbedMenu first
		TabbedController->BroadcastInitialValues();
		TabbedController->BindCallbacksToDependencies();

		// Then initialize children
		if (TObjectPtr<UAttributeMenuWidgetController>* AttrController = AttributeMenuControllers.Find(WCParams.PlayerController))
		{
			(*AttrController)->BroadcastInitialValues();
			(*AttrController)->BindCallbacksToDependencies();
		}

		if (TObjectPtr<USpellMenuWidgetController>* SpellController = SpellMenuControllers.Find(WCParams.PlayerController))
		{
			(*SpellController)->BroadcastInitialValues();
			(*SpellController)->BindCallbacksToDependencies();
		}
	}

	SharedController->BindCallbacksToDependencies();

	RageWidgetControllers.Add(WCParams.PlayerController, SharedController);
	return SharedController;
}

UTabbedMenuWidgetController* ARageInMageHUD::GetTabbedMenuWidgetController(const FRageInMageWidgetControllerParams& WCParams)
{
	// Return cached controller if it exists
	if (TObjectPtr<UTabbedMenuWidgetController>* FoundController = TabbedMenuControllers.Find(WCParams.PlayerController))
	{
		return FoundController->Get();
	}

	// Create new controller
	if (!TabbedMenuWidgetControllerClass)
	{
		return nullptr;
	}

	UTabbedMenuWidgetController* NewController = NewObject<UTabbedMenuWidgetController>(this, TabbedMenuWidgetControllerClass);
	NewController->SetWidgetControllerParams(WCParams);

	// Cache it
	TabbedMenuControllers.Add(WCParams.PlayerController, NewController);

	// Initialize after creating all controllers
	NewController->BroadcastInitialValues();
	NewController->BindCallbacksToDependencies();

	return NewController;
}

UAttributeMenuWidgetController* ARageInMageHUD::GetAttributeMenuWidgetController(const FRageInMageWidgetControllerParams& WCParams)
{
	// Return cached controller if it exists
	if (TObjectPtr<UAttributeMenuWidgetController>* FoundController = AttributeMenuControllers.Find(WCParams.PlayerController))
	{
		return FoundController->Get();
	}

	// Create new controller
	if (!AttributeMenuWidgetControllerClass)
	{
		return nullptr;
	}

	UAttributeMenuWidgetController* NewController = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
	NewController->SetWidgetControllerParams(WCParams);

	// Cache it
	AttributeMenuControllers.Add(WCParams.PlayerController, NewController);

	// Get or create the parent TabbedMenuWidgetController first
	UTabbedMenuWidgetController* TabbedController = GetTabbedMenuWidgetController(WCParams);
	if (TabbedController)
	{
		TabbedController->SetAttributeMenuWidgetController(NewController);
	}

	// Initialize
	NewController->BroadcastInitialValues();
	NewController->BindCallbacksToDependencies();

	return NewController;
}

USpellMenuWidgetController* ARageInMageHUD::GetSpellMenuWidgetController(const FRageInMageWidgetControllerParams& WCParams)
{
	// Return cached controller if it exists
	if (TObjectPtr<USpellMenuWidgetController>* FoundController = SpellMenuControllers.Find(WCParams.PlayerController))
	{
		return FoundController->Get();
	}

	// Create new controller
	if (!SpellMenuWidgetControllerClass)
	{
		return nullptr;
	}

	USpellMenuWidgetController* NewController = NewObject<USpellMenuWidgetController>(this, SpellMenuWidgetControllerClass);
	NewController->SetWidgetControllerParams(WCParams);

	// Cache it
	SpellMenuControllers.Add(WCParams.PlayerController, NewController);

	// Get or create the parent TabbedMenuWidgetController first
	UTabbedMenuWidgetController* TabbedController = GetTabbedMenuWidgetController(WCParams);
	if (TabbedController)
	{
		TabbedController->SetSpellMenuWidgetController(NewController);
	}

	// Initialize
	NewController->BroadcastInitialValues();
	NewController->BindCallbacksToDependencies();

	return NewController;
}

UOverlayWidgetController* ARageInMageHUD::GetOverlayWidgetController(const FRageInMageWidgetControllerParams& WCParams)
{
	// Return cached controller if it exists
	if (TObjectPtr<UOverlayWidgetController>* FoundController = OverlayControllers.Find(WCParams.PlayerController))
	{
		return FoundController->Get();
	}

	// Create new controller
	if (!OverlayWidgetControllerClass)
	{
		return nullptr;
	}

	UOverlayWidgetController* NewController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
	NewController->SetWidgetControllerParams(WCParams);

	// Get or create the shared RageWidgetController
	URageInMageWidgetController* SharedController = GetRageWidgetController(WCParams);
	if (SharedController)
	{
		NewController->SetRageWidgetController(SharedController); // Share the same instance!
	}

	// Cache it
	OverlayControllers.Add(WCParams.PlayerController, NewController);

	return NewController;
}

void ARageInMageHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(RageWidgetClass, TEXT("Rage Widget Class uninitialised, please fill out BP_MageHUD"));
	checkf(RageWidgetControllerClass, TEXT("Widget Controller Class unitialised, please fill out BP_MageHUD"));
	
	URageInMageUserWidget* WidgetClass = CreateWidget<URageInMageUserWidget>(GetWorld(), RageWidgetClass);
	RageWidget = WidgetClass;

	const FRageInMageWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	URageInMageWidgetController* WidgetController = GetRageWidgetController(WidgetControllerParams);
	
	RageWidget->SetWidgetController(WidgetController);
	WidgetController->BroadcastInitialValues();
	RageWidget->AddToViewport();
}
