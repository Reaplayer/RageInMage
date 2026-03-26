// Copyright Reaplays

#include "UI/WidgetController/SettingsWidgetController.h"
#include "UI/WidgetController/AudioSettingsWidgetController.h"
#include "UI/WidgetController/GeneralSettingsWidgetController.h"
#include "UI/WidgetController/ControlsWidgetController.h"
#include "Game/RageInMageSettingsSaveGame.h"
#include "Player/RageInMagePlayerController.h"
#include "Input/RageInMageConfig.h"
#include "InputMappingContext.h"
#include "RageInMage/RageInMageLogChannels.h"

void USettingsWidgetController::BindCallbacksToDependencies()
{
	// Settings controller has no GAS dependencies to bind to.
	// All interaction is driven by explicit user actions (set/get/apply/revert).
}

void USettingsWidgetController::BroadcastInitialValues()
{
	LoadSettings();
	CreateChildControllers();

	if (AudioSettingsController)
	{
		AudioSettingsController->BroadcastInitialValues();
	}
	if (GeneralSettingsController)
	{
		GeneralSettingsController->BroadcastInitialValues();
	}
	if (ControlsController)
	{
		ControlsController->BroadcastInitialValues();
	}
}

UAudioSettingsWidgetController* USettingsWidgetController::GetAudioSettingsController()
{
	if (!AudioSettingsController)
	{
		CreateChildControllers();
	}
	return AudioSettingsController;
}

UGeneralSettingsWidgetController* USettingsWidgetController::GetGeneralSettingsController()
{
	if (!GeneralSettingsController)
	{
		CreateChildControllers();
	}
	return GeneralSettingsController;
}

UControlsWidgetController* USettingsWidgetController::GetControlsController()
{
	if (!ControlsController)
	{
		CreateChildControllers();
	}
	return ControlsController;
}

URageInMageSettingsSaveGame* USettingsWidgetController::GetCurrentSettings()
{
	LoadSettings();
	return CurrentSettings;
}

void USettingsWidgetController::ApplySettings()
{
	if (CurrentSettings)
	{
		CurrentSettings->SaveSettings();
	}
}

void USettingsWidgetController::RevertSettings()
{
	// Reload from disk (discards in-memory changes)
	CurrentSettings = URageInMageSettingsSaveGame::LoadOrCreateSettings();

	// Tell children to re-read from the (now-reverted) SaveGame and re-broadcast
	if (AudioSettingsController)
	{
		AudioSettingsController->RevertValues();
	}
	if (GeneralSettingsController)
	{
		GeneralSettingsController->RevertValues();
	}
	if (ControlsController)
	{
		ControlsController->RevertValues();
	}
}

void USettingsWidgetController::LoadSettings()
{
	if (!CurrentSettings)
	{
		CurrentSettings = URageInMageSettingsSaveGame::LoadOrCreateSettings();
	}
}

void USettingsWidgetController::CreateChildControllers()
{
	LoadSettings();

	// Audio child
	if (!AudioSettingsController && AudioSettingsControllerClass)
	{
		AudioSettingsController = NewObject<UAudioSettingsWidgetController>(this, AudioSettingsControllerClass);
		AudioSettingsController->Initialize(this, PlayerController);
	}

	// General child
	if (!GeneralSettingsController && GeneralSettingsControllerClass)
	{
		GeneralSettingsController = NewObject<UGeneralSettingsWidgetController>(this, GeneralSettingsControllerClass);
		GeneralSettingsController->Initialize(this);
	}

	// Controls child
	if (!ControlsController && ControlsControllerClass)
	{
		ControlsController = NewObject<UControlsWidgetController>(this, ControlsControllerClass);
		ControlsController->Initialize(this, GetRagePC(), InputConfig, DefaultMageContext);
	}
}
