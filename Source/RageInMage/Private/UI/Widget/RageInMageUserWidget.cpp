// Copyright Reaplays


#include "UI/Widget/RageInMageUserWidget.h"

#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "UI/HUD/RageInMageHUD.h"
#include "UI/WidgetController/TabbedMenuWidgetController.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"

void URageInMageUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}

UTabbedMenuWidgetController* URageInMageUserWidget::GetTabbedMenuController()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return nullptr;

	ARageInMageHUD* HUD = Cast<ARageInMageHUD>(PC->GetHUD());
	if (!HUD) return nullptr;

	// Create params
	ARageInMagePlayerState* PS = Cast<ARageInMagePlayerState>(PC->GetPlayerState<APlayerState>());
	if (!PS) return nullptr;

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	UAttributeSet* AS = PS->GetAttributeSet();
	if (!ASC || !AS) return nullptr;

	FRageInMageWidgetControllerParams Params(PC, PS, ASC, AS);
	return HUD->GetTabbedMenuWidgetController(Params);
}

UAttributeMenuWidgetController* URageInMageUserWidget::GetAttributeMenuController()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return nullptr;

	ARageInMageHUD* HUD = Cast<ARageInMageHUD>(PC->GetHUD());
	if (!HUD) return nullptr;

	ARageInMagePlayerState* PS = Cast<ARageInMagePlayerState>(PC->GetPlayerState<APlayerState>());
	if (!PS) return nullptr;

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	UAttributeSet* AS = PS->GetAttributeSet();
	if (!ASC || !AS) return nullptr;

	FRageInMageWidgetControllerParams Params(PC, PS, ASC, AS);
	return HUD->GetAttributeMenuWidgetController(Params);
}

USpellMenuWidgetController* URageInMageUserWidget::GetSpellMenuController()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return nullptr;

	ARageInMageHUD* HUD = Cast<ARageInMageHUD>(PC->GetHUD());
	if (!HUD) return nullptr;

	ARageInMagePlayerState* PS = Cast<ARageInMagePlayerState>(PC->GetPlayerState<APlayerState>());
	if (!PS) return nullptr;

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	UAttributeSet* AS = PS->GetAttributeSet();
	if (!ASC || !AS) return nullptr;

	FRageInMageWidgetControllerParams Params(PC, PS, ASC, AS);
	return HUD->GetSpellMenuWidgetController(Params);
}

UOverlayWidgetController* URageInMageUserWidget::GetOverlayController()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return nullptr;

	ARageInMageHUD* HUD = Cast<ARageInMageHUD>(PC->GetHUD());
	if (!HUD) return nullptr;

	ARageInMagePlayerState* PS = Cast<ARageInMagePlayerState>(PC->GetPlayerState<APlayerState>());
	if (!PS) return nullptr;

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	UAttributeSet* AS = PS->GetAttributeSet();
	if (!ASC || !AS) return nullptr;

	FRageInMageWidgetControllerParams Params(PC, PS, ASC, AS);
	return HUD->GetOverlayWidgetController(Params);
}
