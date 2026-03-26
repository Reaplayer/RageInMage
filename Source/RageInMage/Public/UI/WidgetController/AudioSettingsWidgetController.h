// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AudioSettingsWidgetController.generated.h"

class USoundMix;
class USoundClass;
class USettingsWidgetController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVolumeChangedSignature, float, NewValue);

/**
 * Controls audio settings (master/music/SFX/voice volumes).
 * Child of USettingsWidgetController — accesses SaveGame through parent.
 */
UCLASS(Blueprintable)
class RAGEINMAGE_API UAudioSettingsWidgetController : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(USettingsWidgetController* InParent, APlayerController* InPC);
	void BroadcastInitialValues();
	void RevertValues();

	// ── Volume change delegates ──

	UPROPERTY(BlueprintAssignable, Category = "Settings|Audio")
	FOnVolumeChangedSignature OnMasterVolumeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Settings|Audio")
	FOnVolumeChangedSignature OnMusicVolumeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Settings|Audio")
	FOnVolumeChangedSignature OnSFXVolumeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Settings|Audio")
	FOnVolumeChangedSignature OnVoiceVolumeChanged;

	// ── Audio setters ──

	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetMasterVolume(float Value);

	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetMusicVolume(float Value);

	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetSFXVolume(float Value);

	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetVoiceVolume(float Value);

	// ── Audio getters ──

	UFUNCTION(BlueprintPure, Category = "Settings|Audio")
	float GetMasterVolume() const;

	UFUNCTION(BlueprintPure, Category = "Settings|Audio")
	float GetMusicVolume() const;

	UFUNCTION(BlueprintPure, Category = "Settings|Audio")
	float GetSFXVolume() const;

	UFUNCTION(BlueprintPure, Category = "Settings|Audio")
	float GetVoiceVolume() const;

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

	UPROPERTY()
	TObjectPtr<USettingsWidgetController> ParentController;

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;
};
