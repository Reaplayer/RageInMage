// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "RageInMageWidgetControllerBase.h"
#include "SettingsWidgetController.generated.h"

class URageInMageSettingsSaveGame;
class USoundMix;
class USoundClass;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVolumeChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSensitivityChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUIScaleChangedSignature, float, NewValue);

/**
 * Controls the settings UI (audio volumes, sensitivity, UI scale).
 * All operations are local-only — no server RPCs.
 * Extends URageInMageWidgetControllerBase directly (not TabbedMenuWidgetController).
 */
UCLASS(Blueprintable)
class RAGEINMAGE_API USettingsWidgetController : public URageInMageWidgetControllerBase
{
	GENERATED_BODY()

public:
	// Volume change delegates
	UPROPERTY(BlueprintAssignable, Category = "Settings|Audio")
	FOnVolumeChangedSignature OnMasterVolumeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Settings|Audio")
	FOnVolumeChangedSignature OnMusicVolumeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Settings|Audio")
	FOnVolumeChangedSignature OnSFXVolumeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Settings|Audio")
	FOnVolumeChangedSignature OnVoiceVolumeChanged;

	// Input/UI delegates
	UPROPERTY(BlueprintAssignable, Category = "Settings|Input")
	FOnSensitivityChangedSignature OnMouseSensitivityChanged;

	UPROPERTY(BlueprintAssignable, Category = "Settings|UI")
	FOnUIScaleChangedSignature OnUIScaleChanged;

	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;

	// Audio setters
	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetMasterVolume(float Value);

	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetMusicVolume(float Value);

	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetSFXVolume(float Value);

	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetVoiceVolume(float Value);

	// Audio getters
	UFUNCTION(BlueprintPure, Category = "Settings|Audio")
	float GetMasterVolume() const;

	UFUNCTION(BlueprintPure, Category = "Settings|Audio")
	float GetMusicVolume() const;

	UFUNCTION(BlueprintPure, Category = "Settings|Audio")
	float GetSFXVolume() const;

	UFUNCTION(BlueprintPure, Category = "Settings|Audio")
	float GetVoiceVolume() const;

	// Input settings
	UFUNCTION(BlueprintCallable, Category = "Settings|Input")
	void SetMouseSensitivity(float Value);

	UFUNCTION(BlueprintPure, Category = "Settings|Input")
	float GetMouseSensitivity() const;

	// UI settings
	UFUNCTION(BlueprintCallable, Category = "Settings|UI")
	void SetUIScale(float Value);

	UFUNCTION(BlueprintPure, Category = "Settings|UI")
	float GetUIScale() const;

	// Persistence
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ApplySettings();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void RevertSettings();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundMix> SettingsSoundMix;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundClass> MasterSoundClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundClass> MusicSoundClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundClass> SFXSoundClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundClass> VoiceSoundClass;

private:
	void ApplyVolumeToSoundClass(USoundClass* SoundClass, float Volume);
	void LoadSettings();

	UPROPERTY()
	TObjectPtr<URageInMageSettingsSaveGame> CurrentSettings;
};
