// Copyright Reaplays


#include "UI/Widget/RageInMageUserWidget.h"
#include "UI/HUD/RageInMageHUD.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "UI/WidgetController/InventoryWidgetController.h"

void URageInMageUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}

UAttributeMenuWidgetController* URageInMageUserWidget::GetAttributeMenuController()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return nullptr;

	ARageInMageHUD* HUD = Cast<ARageInMageHUD>(PC->GetHUD());
	if (!HUD) return nullptr;
	return HUD->GetAttributeMenuWidgetController(PC);
}

USpellMenuWidgetController* URageInMageUserWidget::GetSpellMenuController()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return nullptr;

	ARageInMageHUD* HUD = Cast<ARageInMageHUD>(PC->GetHUD());
	if (!HUD) return nullptr;
	return HUD->GetSpellMenuWidgetController(PC);
}

UOverlayWidgetController* URageInMageUserWidget::GetOverlayController()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return nullptr;

	ARageInMageHUD* HUD = Cast<ARageInMageHUD>(PC->GetHUD());
	if (!HUD) return nullptr;
	return HUD->GetOverlayWidgetController(PC);
}

UInventoryWidgetController* URageInMageUserWidget::GetInventoryController()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return nullptr;

	ARageInMageHUD* HUD = Cast<ARageInMageHUD>(PC->GetHUD());
	if (!HUD) return nullptr;
	return HUD->GetInventoryWidgetController(PC);
}
