// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "RageInMageWidgetControllerBase.h"
#include "SettingsWidgetController.generated.h"

class URageInMageSettingsSaveGame;
class UAudioSettingsWidgetController;
class UGeneralSettingsWidgetController;
class UControlsWidgetController;
class URageInMageConfig;
class UInputMappingContext;

/**
 * Parent tab-manager for the Settings menu.
 * Owns the SaveGame object and provides access to child controllers:
 *   - Audio (volumes)
 *   - General (sensitivity, UI scale)
 *   - Controls (keybinding remapping with chord support)
 *
 * Children access SaveGame via GetCurrentSettings() and never reference each other.
 * Extends URageInMageWidgetControllerBase directly (not TabbedMenuWidgetController)
 * because settings are local-only with no GAS dependency.
 */
UCLASS(Blueprintable)
class RAGEINMAGE_API USettingsWidgetController : public URageInMageWidgetControllerBase
{
	GENERATED_BODY()

public:
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;

	// ── Child controller accessors ──

	UFUNCTION(BlueprintCallable, Category = "Settings")
	UAudioSettingsWidgetController* GetAudioSettingsController();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	UGeneralSettingsWidgetController* GetGeneralSettingsController();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	UControlsWidgetController* GetControlsController();

	// ── SaveGame management ──

	/** Get the current in-memory settings (lazy-loads from disk if needed). */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	URageInMageSettingsSaveGame* GetCurrentSettings();

	/** Save current settings to disk. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ApplySettings();

	/** Reload settings from disk and re-broadcast to all children. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void RevertSettings();

protected:
	// ── Child controller classes (set in Blueprint) ──

	UPROPERTY(EditDefaultsOnly, Category = "Widget Controllers")
	TSubclassOf<UAudioSettingsWidgetController> AudioSettingsControllerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widget Controllers")
	TSubclassOf<UGeneralSettingsWidgetController> GeneralSettingsControllerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widget Controllers")
	TSubclassOf<UControlsWidgetController> ControlsControllerClass;

	// ── Input references for controls tab ──

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<URageInMageConfig> InputConfig;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMageContext;

	/** Controller-specific input config for controls tab */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<URageInMageConfig> ControllerInputConfig;

	/** Controller-specific IMC for controls tab */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultControllerMageContext;

private:
	void LoadSettings();
	void CreateChildControllers();

	UPROPERTY()
	TObjectPtr<URageInMageSettingsSaveGame> CurrentSettings;

	UPROPERTY()
	TObjectPtr<UAudioSettingsWidgetController> AudioSettingsController;

	UPROPERTY()
	TObjectPtr<UGeneralSettingsWidgetController> GeneralSettingsController;

	UPROPERTY()
	TObjectPtr<UControlsWidgetController> ControlsController;
};
