// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RageInMageSettingsSaveGame.generated.h"

/**
 * Persists player settings (audio volumes, sensitivity, UI scale).
 * Saved locally per device — not replicated.
 */
UCLASS()
class RAGEINMAGE_API URageInMageSettingsSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame)
	float MasterVolume = 1.0f;

	UPROPERTY(SaveGame)
	float MusicVolume = 1.0f;

	UPROPERTY(SaveGame)
	float SFXVolume = 1.0f;

	UPROPERTY(SaveGame)
	float VoiceVolume = 1.0f;

	UPROPERTY(SaveGame)
	float MouseSensitivity = 1.0f;

	UPROPERTY(SaveGame)
	float UIScale = 1.0f;

	/** Save current settings to disk. */
	void SaveSettings();

	/** Load existing settings or create defaults. */
	static URageInMageSettingsSaveGame* LoadOrCreateSettings();

private:
	static const FString SlotName;
};
