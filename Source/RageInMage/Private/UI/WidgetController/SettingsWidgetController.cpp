// Copyright Reaplays

#include "UI/WidgetController/SettingsWidgetController.h"
#include "Game/RageInMageSettingsSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"

void USettingsWidgetController::BindCallbacksToDependencies()
{
	// Settings controller has no GAS dependencies to bind to.
	// All interaction is driven by explicit user actions (set/get/apply/revert).
}

void USettingsWidgetController::BroadcastInitialValues()
{
	LoadSettings();

	OnMasterVolumeChanged.Broadcast(CurrentSettings->MasterVolume);
	OnMusicVolumeChanged.Broadcast(CurrentSettings->MusicVolume);
	OnSFXVolumeChanged.Broadcast(CurrentSettings->SFXVolume);
	OnVoiceVolumeChanged.Broadcast(CurrentSettings->VoiceVolume);
	OnMouseSensitivityChanged.Broadcast(CurrentSettings->MouseSensitivity);
	OnUIScaleChanged.Broadcast(CurrentSettings->UIScale);

	// Apply saved volumes to the audio engine
	ApplyVolumeToSoundClass(MasterSoundClass, CurrentSettings->MasterVolume);
	ApplyVolumeToSoundClass(MusicSoundClass, CurrentSettings->MusicVolume);
	ApplyVolumeToSoundClass(SFXSoundClass, CurrentSettings->SFXVolume);
	ApplyVolumeToSoundClass(VoiceSoundClass, CurrentSettings->VoiceVolume);
}

void USettingsWidgetController::SetMasterVolume(float Value)
{
	LoadSettings();
	CurrentSettings->MasterVolume = FMath::Clamp(Value, 0.f, 1.f);
	ApplyVolumeToSoundClass(MasterSoundClass, CurrentSettings->MasterVolume);
	OnMasterVolumeChanged.Broadcast(CurrentSettings->MasterVolume);
}

void USettingsWidgetController::SetMusicVolume(float Value)
{
	LoadSettings();
	CurrentSettings->MusicVolume = FMath::Clamp(Value, 0.f, 1.f);
	ApplyVolumeToSoundClass(MusicSoundClass, CurrentSettings->MusicVolume);
	OnMusicVolumeChanged.Broadcast(CurrentSettings->MusicVolume);
}

void USettingsWidgetController::SetSFXVolume(float Value)
{
	LoadSettings();
	CurrentSettings->SFXVolume = FMath::Clamp(Value, 0.f, 1.f);
	ApplyVolumeToSoundClass(SFXSoundClass, CurrentSettings->SFXVolume);
	OnSFXVolumeChanged.Broadcast(CurrentSettings->SFXVolume);
}

void USettingsWidgetController::SetVoiceVolume(float Value)
{
	LoadSettings();
	CurrentSettings->VoiceVolume = FMath::Clamp(Value, 0.f, 1.f);
	ApplyVolumeToSoundClass(VoiceSoundClass, CurrentSettings->VoiceVolume);
	OnVoiceVolumeChanged.Broadcast(CurrentSettings->VoiceVolume);
}

float USettingsWidgetController::GetMasterVolume() const
{
	return CurrentSettings ? CurrentSettings->MasterVolume : 1.f;
}

float USettingsWidgetController::GetMusicVolume() const
{
	return CurrentSettings ? CurrentSettings->MusicVolume : 1.f;
}

float USettingsWidgetController::GetSFXVolume() const
{
	return CurrentSettings ? CurrentSettings->SFXVolume : 1.f;
}

float USettingsWidgetController::GetVoiceVolume() const
{
	return CurrentSettings ? CurrentSettings->VoiceVolume : 1.f;
}

void USettingsWidgetController::SetMouseSensitivity(float Value)
{
	LoadSettings();
	CurrentSettings->MouseSensitivity = FMath::Clamp(Value, 0.1f, 10.f);
	OnMouseSensitivityChanged.Broadcast(CurrentSettings->MouseSensitivity);
}

float USettingsWidgetController::GetMouseSensitivity() const
{
	return CurrentSettings ? CurrentSettings->MouseSensitivity : 1.f;
}

void USettingsWidgetController::SetUIScale(float Value)
{
	LoadSettings();
	CurrentSettings->UIScale = FMath::Clamp(Value, 0.5f, 2.f);
	OnUIScaleChanged.Broadcast(CurrentSettings->UIScale);
}

float USettingsWidgetController::GetUIScale() const
{
	return CurrentSettings ? CurrentSettings->UIScale : 1.f;
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
	CurrentSettings = URageInMageSettingsSaveGame::LoadOrCreateSettings();

	// Re-apply reverted volumes
	ApplyVolumeToSoundClass(MasterSoundClass, CurrentSettings->MasterVolume);
	ApplyVolumeToSoundClass(MusicSoundClass, CurrentSettings->MusicVolume);
	ApplyVolumeToSoundClass(SFXSoundClass, CurrentSettings->SFXVolume);
	ApplyVolumeToSoundClass(VoiceSoundClass, CurrentSettings->VoiceVolume);

	// Broadcast all values to update UI
	OnMasterVolumeChanged.Broadcast(CurrentSettings->MasterVolume);
	OnMusicVolumeChanged.Broadcast(CurrentSettings->MusicVolume);
	OnSFXVolumeChanged.Broadcast(CurrentSettings->SFXVolume);
	OnVoiceVolumeChanged.Broadcast(CurrentSettings->VoiceVolume);
	OnMouseSensitivityChanged.Broadcast(CurrentSettings->MouseSensitivity);
	OnUIScaleChanged.Broadcast(CurrentSettings->UIScale);
}

void USettingsWidgetController::ApplyVolumeToSoundClass(USoundClass* SoundClass, float Volume)
{
	if (!SettingsSoundMix || !SoundClass || !PlayerController) return;

	UWorld* World = PlayerController->GetWorld();
	if (!World) return;

	UGameplayStatics::SetSoundMixClassOverride(World, SettingsSoundMix, SoundClass, Volume, 1.f, 0.f, true);
	UGameplayStatics::PushSoundMixModifier(World, SettingsSoundMix);
}

void USettingsWidgetController::LoadSettings()
{
	if (!CurrentSettings)
	{
		CurrentSettings = URageInMageSettingsSaveGame::LoadOrCreateSettings();
	}
}
