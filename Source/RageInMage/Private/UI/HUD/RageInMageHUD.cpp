#include "UI/HUD/RageInMageHUD.h"

#include "RageInMage/RageInMageLogChannels.h"
#include "UI/Widget/RageInMageUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "UI/WidgetController/SettingsWidgetController.h"
#include "UI/WidgetController/InventoryWidgetController.h"

void ARageInMageHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("OverlayWidgetClass not set in BP_RageInMageHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("OverlayWidgetControllerClass not set in BP_RageInMageHUD"));

	// Cache GAS references
	FPlayerGASReferences GASRefs;
	GASRefs.PlayerController = PC;
	GASRefs.PlayerState = PS;
	GASRefs.ASC = ASC;
	GASRefs.AttributeSet = AS;
	PlayerGASCache.Add(PC, GASRefs);

	// Create overlay widget
	OverlayWidget = CreateWidget<URageInMageUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget->AddToViewport();
	OverlayWidget->SetWidgetController(OverlayWidgetControllerClass);
}

UOverlayWidgetController* ARageInMageHUD::GetOverlayWidgetController(APlayerController* PC)
{
	if (!PC) return nullptr;

	// Return cached controller if exists
	if (TObjectPtr<UOverlayWidgetController>* Found = OverlayControllers.Find(PC))
	{
		return *Found;
	}

	// Create a new controller if one doesn't exist
	CreateOverlayController(PC);
	return OverlayControllers.FindRef(PC);
}

UAttributeMenuWidgetController* ARageInMageHUD::GetAttributeMenuWidgetController(APlayerController* PC)
{
	if (!PC) return nullptr;

	// Return cached controller if exists
	if (TObjectPtr<UAttributeMenuWidgetController>* Found = AttributeMenuControllers.Find(PC))
	{
		return *Found;
	}

	// Create a new controller if one doesn't exist
	CreateAttributeMenuController(PC);
	return AttributeMenuControllers.FindRef(PC);
}

USpellMenuWidgetController* ARageInMageHUD::GetSpellMenuWidgetController(APlayerController* PC)
{
	if (!PC) return nullptr;

	// Return cached controller if exists
	if (TObjectPtr<USpellMenuWidgetController>* Found = SpellMenuControllers.Find(PC))
	{
		return *Found;
	}

	// Create a new controller if one doesn't exist
	CreateSpellMenuController(PC);
	return SpellMenuControllers.FindRef(PC);
}

void ARageInMageHUD::CreateOverlayController(APlayerController* PC)
{
	if (!OverlayWidgetControllerClass)
	{
		UE_LOG(LogRageInMage, Error, TEXT("OverlayWidgetControllerClass not set"));
		return;
	}

	FPlayerGASReferences* GASRefs = PlayerGASCache.Find(PC);
	if (!GASRefs)
	{
		UE_LOG(LogRageInMage, Error, TEXT("No GAS references cached for player"));
		return;
	}

	UOverlayWidgetController* Controller = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
	Controller->SetGASReferences(GASRefs->PlayerController, GASRefs->PlayerState, GASRefs->ASC, GASRefs->AttributeSet);
	Controller->BindCallbacksToDependencies();
	// DO NOT call BroadcastInitialValues() here.
	// The widget will call it from WidgetControllerSet after binding.

	OverlayControllers.Add(PC, Controller);
}

void ARageInMageHUD::CreateAttributeMenuController(APlayerController* PC)
{
	if (!AttributeMenuWidgetControllerClass)
	{
		UE_LOG(LogTemp, Error, TEXT("AttributeMenuWidgetControllerClass not set"));
		return;
	}

	FPlayerGASReferences* GASRefs = PlayerGASCache.Find(PC);
	if (!GASRefs)
	{
		UE_LOG(LogTemp, Error, TEXT("No GAS references cached for player"));
		return;
	}

	UAttributeMenuWidgetController* Controller = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
	Controller->SetGASReferences(GASRefs->PlayerController, GASRefs->PlayerState, GASRefs->ASC, GASRefs->AttributeSet);
	Controller->BindCallbacksToDependencies();
	// NOTE: Do NOT call BroadcastInitialValues here — widgets haven't bound to delegates yet.
	// Widgets call BroadcastInitialValues themselves after binding in WidgetControllerSet.

	AttributeMenuControllers.Add(PC, Controller);
}

void ARageInMageHUD::CreateSpellMenuController(APlayerController* PC)
{
	if (!SpellMenuWidgetControllerClass)
	{
		UE_LOG(LogTemp, Error, TEXT("SpellMenuWidgetControllerClass not set"));
		return;
	}

	FPlayerGASReferences* GASRefs = PlayerGASCache.Find(PC);
	if (!GASRefs)
	{
		UE_LOG(LogTemp, Error, TEXT("No GAS references cached for player"));
		return;
	}

	USpellMenuWidgetController* Controller = NewObject<USpellMenuWidgetController>(this, SpellMenuWidgetControllerClass);
	Controller->SetGASReferences(GASRefs->PlayerController, GASRefs->PlayerState, GASRefs->ASC, GASRefs->AttributeSet);
	Controller->BindCallbacksToDependencies();
	// NOTE: Do NOT call BroadcastInitialValues here — widgets haven't bound to delegates yet.
	// Widgets call BroadcastInitialValues themselves after binding in WidgetControllerSet.

	SpellMenuControllers.Add(PC, Controller);
}

USettingsWidgetController* ARageInMageHUD::GetSettingsWidgetController(APlayerController* PC)
{
	if (!PC) return nullptr;

	// Return cached controller if exists
	if (TObjectPtr<USettingsWidgetController>* Found = SettingsControllers.Find(PC))
	{
		return *Found;
	}

	// Create a new controller if one doesn't exist
	CreateSettingsController(PC);
	return SettingsControllers.FindRef(PC);
}

void ARageInMageHUD::CreateSettingsController(APlayerController* PC)
{
	if (!SettingsWidgetControllerClass)
	{
		UE_LOG(LogTemp, Error, TEXT("SettingsWidgetControllerClass not set"));
		return;
	}

	FPlayerGASReferences* GASRefs = PlayerGASCache.Find(PC);
	if (!GASRefs)
	{
		UE_LOG(LogTemp, Error, TEXT("No GAS references cached for player"));
		return;
	}

	USettingsWidgetController* Controller = NewObject<USettingsWidgetController>(this, SettingsWidgetControllerClass);
	Controller->SetGASReferences(GASRefs->PlayerController, GASRefs->PlayerState, GASRefs->ASC, GASRefs->AttributeSet);
	Controller->BindCallbacksToDependencies();

	SettingsControllers.Add(PC, Controller);
}

UInventoryWidgetController* ARageInMageHUD::GetInventoryWidgetController(APlayerController* PC)
{
	if (!PC) return nullptr;

	// Return cached controller if exists
	if (TObjectPtr<UInventoryWidgetController>* Found = InventoryControllers.Find(PC))
	{
		return *Found;
	}

	// Create a new controller if one doesn't exist
	CreateInventoryController(PC);
	return InventoryControllers.FindRef(PC);
}

void ARageInMageHUD::CreateInventoryController(APlayerController* PC)
{
	if (!InventoryWidgetControllerClass)
	{
		UE_LOG(LogRageInMage, Error, TEXT("InventoryWidgetControllerClass not set"));
		return;
	}

	FPlayerGASReferences* GASRefs = PlayerGASCache.Find(PC);
	if (!GASRefs)
	{
		UE_LOG(LogRageInMage, Error, TEXT("No GAS references cached for player"));
		return;
	}

	UInventoryWidgetController* Controller = NewObject<UInventoryWidgetController>(this, InventoryWidgetControllerClass);
	Controller->SetGASReferences(GASRefs->PlayerController, GASRefs->PlayerState, GASRefs->ASC, GASRefs->AttributeSet);
	Controller->BindCallbacksToDependencies();
	// DO NOT call BroadcastInitialValues here.
	// The widget will call it from WidgetControllerSet after binding.

	InventoryControllers.Add(PC, Controller);
}