// Copyright Reaplays

#include "UI/WidgetController/AudioSettingsWidgetController.h"
#include "UI/WidgetController/SettingsWidgetController.h"
#include "Game/RageInMageSettingsSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"

void UAudioSettingsWidgetController::Initialize(USettingsWidgetController* InParent, APlayerController* InPC)
{
	ParentController = InParent;
	PlayerController = InPC;
}

void UAudioSettingsWidgetController::BroadcastInitialValues()
{
	URageInMageSettingsSaveGame* Settings = ParentController->GetCurrentSettings();
	if (!Settings) return;

	OnMasterVolumeChanged.Broadcast(Settings->MasterVolume);
	OnMusicVolumeChanged.Broadcast(Settings->MusicVolume);
	OnSFXVolumeChanged.Broadcast(Settings->SFXVolume);
	OnVoiceVolumeChanged.Broadcast(Settings->VoiceVolume);

	ApplyVolumeToSoundClass(MasterSoundClass, Settings->MasterVolume);
	ApplyVolumeToSoundClass(MusicSoundClass, Settings->MusicVolume);
	ApplyVolumeToSoundClass(SFXSoundClass, Settings->SFXVolume);
	ApplyVolumeToSoundClass(VoiceSoundClass, Settings->VoiceVolume);
}

void UAudioSettingsWidgetController::RevertValues()
{
	BroadcastInitialValues();
}

void UAudioSettingsWidgetController::SetMasterVolume(float Value)
{
	URageInMageSettingsSaveGame* Settings = ParentController->GetCurrentSettings();
	if (!Settings) return;

	Settings->MasterVolume = FMath::Clamp(Value, 0.f, 1.f);
	ApplyVolumeToSoundClass(MasterSoundClass, Settings->MasterVolume);
	OnMasterVolumeChanged.Broadcast(Settings->MasterVolume);
}

void UAudioSettingsWidgetController::SetMusicVolume(float Value)
{
	URageInMageSettingsSaveGame* Settings = ParentController->GetCurrentSettings();
	if (!Settings) return;

	Settings->MusicVolume = FMath::Clamp(Value, 0.f, 1.f);
	ApplyVolumeToSoundClass(MusicSoundClass, Settings->MusicVolume);
	OnMusicVolumeChanged.Broadcast(Settings->MusicVolume);
}

void UAudioSettingsWidgetController::SetSFXVolume(float Value)
{
	URageInMageSettingsSaveGame* Settings = ParentController->GetCurrentSettings();
	if (!Settings) return;

	Settings->SFXVolume = FMath::Clamp(Value, 0.f, 1.f);
	ApplyVolumeToSoundClass(SFXSoundClass, Settings->SFXVolume);
	OnSFXVolumeChanged.Broadcast(Settings->SFXVolume);
}

void UAudioSettingsWidgetController::SetVoiceVolume(float Value)
{
	URageInMageSettingsSaveGame* Settings = ParentController->GetCurrentSettings();
	if (!Settings) return;

	Settings->VoiceVolume = FMath::Clamp(Value, 0.f, 1.f);
	ApplyVolumeToSoundClass(VoiceSoundClass, Settings->VoiceVolume);
	OnVoiceVolumeChanged.Broadcast(Settings->VoiceVolume);
}

float UAudioSettingsWidgetController::GetMasterVolume() const
{
	if (const URageInMageSettingsSaveGame* Settings = ParentController->GetCurrentSettings())
	{
		return Settings->MasterVolume;
	}
	return 1.f;
}

float UAudioSettingsWidgetController::GetMusicVolume() const
{
	if (const URageInMageSettingsSaveGame* Settings = ParentController->GetCurrentSettings())
	{
		return Settings->MusicVolume;
	}
	return 1.f;
}

float UAudioSettingsWidgetController::GetSFXVolume() const
{
	if (const URageInMageSettingsSaveGame* Settings = ParentController->GetCurrentSettings())
	{
		return Settings->SFXVolume;
	}
	return 1.f;
}

float UAudioSettingsWidgetController::GetVoiceVolume() const
{
	if (const URageInMageSettingsSaveGame* Settings = ParentController->GetCurrentSettings())
	{
		return Settings->VoiceVolume;
	}
	return 1.f;
}

void UAudioSettingsWidgetController::ApplyVolumeToSoundClass(USoundClass* SoundClass, float Volume)
{
	if (!SettingsSoundMix || !SoundClass || !PlayerController) return;

	UWorld* World = PlayerController->GetWorld();
	if (!World) return;

	UGameplayStatics::SetSoundMixClassOverride(World, SettingsSoundMix, SoundClass, Volume, 1.f, 0.f, true);
	UGameplayStatics::PushSoundMixModifier(World, SettingsSoundMix);
}
