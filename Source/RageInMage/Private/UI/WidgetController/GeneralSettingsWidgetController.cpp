// Copyright Reaplays

#include "UI/WidgetController/GeneralSettingsWidgetController.h"
#include "UI/WidgetController/SettingsWidgetController.h"
#include "Game/RageInMageSettingsSaveGame.h"

void UGeneralSettingsWidgetController::Initialize(USettingsWidgetController* InParent)
{
	ParentController = InParent;
}

void UGeneralSettingsWidgetController::BroadcastInitialValues()
{
	URageInMageSettingsSaveGame* Settings = ParentController->GetCurrentSettings();
	if (!Settings) return;

	OnMouseSensitivityChanged.Broadcast(Settings->MouseSensitivity);
	OnUIScaleChanged.Broadcast(Settings->UIScale);
}

void UGeneralSettingsWidgetController::RevertValues()
{
	BroadcastInitialValues();
}

void UGeneralSettingsWidgetController::SetMouseSensitivity(float Value)
{
	URageInMageSettingsSaveGame* Settings = ParentController->GetCurrentSettings();
	if (!Settings) return;

	Settings->MouseSensitivity = FMath::Clamp(Value, 0.1f, 10.f);
	OnMouseSensitivityChanged.Broadcast(Settings->MouseSensitivity);
}

float UGeneralSettingsWidgetController::GetMouseSensitivity() const
{
	if (const URageInMageSettingsSaveGame* Settings = ParentController->GetCurrentSettings())
	{
		return Settings->MouseSensitivity;
	}
	return 1.f;
}

void UGeneralSettingsWidgetController::SetUIScale(float Value)
{
	URageInMageSettingsSaveGame* Settings = ParentController->GetCurrentSettings();
	if (!Settings) return;

	Settings->UIScale = FMath::Clamp(Value, 0.5f, 2.f);
	OnUIScaleChanged.Broadcast(Settings->UIScale);
}

float UGeneralSettingsWidgetController::GetUIScale() const
{
	if (const URageInMageSettingsSaveGame* Settings = ParentController->GetCurrentSettings())
	{
		return Settings->UIScale;
	}
	return 1.f;
}
