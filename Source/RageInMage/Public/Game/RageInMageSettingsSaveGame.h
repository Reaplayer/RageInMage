// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "RageInMageSettingsSaveGame.generated.h"

/**
 * A single key binding: maps an InputTag to a physical key (with optional modifier for chords).
 * Used for persistence of custom keybindings.
 */
USTRUCT(BlueprintType)
struct FRageInMageKeyBinding
{
	GENERATED_BODY()

	/** Which ability/action this binding is for (e.g., InputTag.LMB) */
	UPROPERTY(SaveGame, BlueprintReadOnly)
	FGameplayTag InputTag;

	/** The main key (e.g., Gamepad_RightTrigger, Q, LeftMouseButton) */
	UPROPERTY(SaveGame, BlueprintReadOnly)
	FKey PrimaryKey;

	/** Optional modifier key for chord bindings (e.g., Gamepad_LeftShoulder, LeftShift) */
	UPROPERTY(SaveGame, BlueprintReadOnly)
	FKey ModifierKey;

	/** Whether this is a chord binding requiring modifier + primary */
	UPROPERTY(SaveGame, BlueprintReadOnly)
	bool bHasModifier = false;

	FRageInMageKeyBinding() = default;

	FRageInMageKeyBinding(const FGameplayTag& InInputTag, const FKey& InPrimaryKey)
		: InputTag(InInputTag), PrimaryKey(InPrimaryKey), bHasModifier(false) {}

	FRageInMageKeyBinding(const FGameplayTag& InInputTag, const FKey& InPrimaryKey, const FKey& InModifierKey)
		: InputTag(InInputTag), PrimaryKey(InPrimaryKey), ModifierKey(InModifierKey), bHasModifier(true) {}
};

/**
 * Persists player settings (audio volumes, sensitivity, UI scale, keybindings).
 * Saved locally per device — not replicated.
 */
UCLASS()
class RAGEINMAGE_API URageInMageSettingsSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	// ── Audio ──

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

	// ── Keybindings ──

	/** Custom keybindings overriding defaults. Key = InputTag, Value = binding info. */
	UPROPERTY(SaveGame)
	TMap<FGameplayTag, FRageInMageKeyBinding> CustomKeybindings;

	/** Save current settings to disk. */
	void SaveSettings();

	/** Load existing settings or create defaults. */
	static URageInMageSettingsSaveGame* LoadOrCreateSettings();

private:
	static const FString SlotName;
};
