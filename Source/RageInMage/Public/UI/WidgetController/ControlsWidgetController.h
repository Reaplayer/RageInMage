// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "Game/RageInMageSettingsSaveGame.h"
#include "ControlsWidgetController.generated.h"

class USettingsWidgetController;
class URageInMageConfig;
class UInputDisplayInfo;
class UInputMappingContext;
class ARageInMagePlayerController;
class UDataTable;

/**
 * Displayable keybinding info for the remapping UI.
 */
USTRUCT(BlueprintType)
struct FRageInMageDisplayBinding
{
	GENERATED_BODY()

	/** The input tag this binding controls (e.g., InputTag.LMB) */
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag InputTag;

	/** Human-readable name (e.g., "Primary Spell") */
	UPROPERTY(BlueprintReadOnly)
	FText DisplayName;

	/** Category for UI grouping (e.g., "Spells", "Movement") */
	UPROPERTY(BlueprintReadOnly)
	FText Category;

	/** Display text for the primary key (e.g., "Left Mouse Button", "RT") */
	UPROPERTY(BlueprintReadOnly)
	FText PrimaryKeyDisplayText;

	/** Display text for the modifier key (e.g., "Shift", "LB") — empty if no modifier */
	UPROPERTY(BlueprintReadOnly)
	FText ModifierKeyDisplayText;

	/** The primary key value */
	UPROPERTY(BlueprintReadOnly)
	FKey PrimaryKey;

	/** The modifier key value */
	UPROPERTY(BlueprintReadOnly)
	FKey ModifierKey;

	/** Whether this is a chord binding (modifier + key) */
	UPROPERTY(BlueprintReadOnly)
	bool bHasModifier = false;

	/** Whether this binding differs from the default */
	UPROPERTY(BlueprintReadOnly)
	bool bIsCustomized = false;

	/** Whether this binding can be remapped */
	UPROPERTY(BlueprintReadOnly)
	bool bIsRemappable = true;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBindingsChangedSignature, const TArray<FRageInMageDisplayBinding>&, Bindings);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeyCapturedSignature, FKey, CapturedKey);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBindingConflictSignature, FGameplayTag, ConflictingInputTag, FText, ConflictMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCaptureCancelledSignature);

/**
 * Controls keybinding remapping with single-key and chord (modifier+key) support.
 * Child of USettingsWidgetController — accesses SaveGame through parent.
 *
 * Key capture workflow:
 *   1. UI calls BeginCaptureKey(InputTag, bChordMode)
 *   2. Controller enters capture mode, broadcasts OnCaptureStarted
 *   3. UI calls OnKeyPressed(Key) when user presses a key
 *      - In chord mode: first press sets modifier, second press sets primary key
 *   4. Controller validates, checks conflicts, applies binding
 *   5. Broadcasts OnBindingsChanged with updated list
 */
UCLASS(Blueprintable)
class RAGEINMAGE_API UControlsWidgetController : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(USettingsWidgetController* InParent, ARageInMagePlayerController* InPC,
		URageInMageConfig* InInputConfig, UInputMappingContext* InDefaultIMC,
		URageInMageConfig* InControllerInputConfig = nullptr, UInputMappingContext* InControllerIMC = nullptr);
	void BroadcastInitialValues();
	void RevertValues();

	// ── Delegates ──

	/** Fired when bindings change (provides full updated list for UI) */
	UPROPERTY(BlueprintAssignable, Category = "Controls")
	FOnBindingsChangedSignature OnBindingsChanged;

	/** Fired when a key is captured during capture mode */
	UPROPERTY(BlueprintAssignable, Category = "Controls")
	FOnKeyCapturedSignature OnKeyCaptured;

	/** Fired when a binding would conflict with an existing one */
	UPROPERTY(BlueprintAssignable, Category = "Controls")
	FOnBindingConflictSignature OnBindingConflict;

	/** Fired when capture mode is cancelled */
	UPROPERTY(BlueprintAssignable, Category = "Controls")
	FOnCaptureCancelledSignature OnCaptureCancelled;

	// ── Query ──

	/** Get all M&K bindings for UI display */
	UFUNCTION(BlueprintCallable, Category = "Controls")
	TArray<FRageInMageDisplayBinding> GetAllBindings();

	/** Get all controller bindings for UI display */
	UFUNCTION(BlueprintCallable, Category = "Controls")
	TArray<FRageInMageDisplayBinding> GetAllControllerBindings();

	/** Check if currently in key capture mode */
	UFUNCTION(BlueprintPure, Category = "Controls")
	bool IsCapturingKey() const { return bIsCapturingKey; }

	/** Get the key icon DataTable from the player controller. */
	UFUNCTION(BlueprintPure, Category = "Controls")
	UDataTable* GetKeyIconTable() const;

	// ── Key capture workflow ──

	/**
	 * Start capturing input for the specified InputTag.
	 * @param InputTag The binding to remap
	 * @param bChordMode If true, captures modifier key first then primary key
	 */
	UFUNCTION(BlueprintCallable, Category = "Controls")
	void BeginCaptureKey(FGameplayTag InputTag, bool bChordMode);

	/** Cancel the current key capture */
	UFUNCTION(BlueprintCallable, Category = "Controls")
	void CancelCaptureKey();

	/**
	 * Called by widget when user presses a key during capture mode.
	 * In chord mode: first call sets modifier, second call sets primary key and applies.
	 * In single-key mode: immediately applies.
	 */
	UFUNCTION(BlueprintCallable, Category = "Controls")
	void OnKeyPressed(FKey Key);

	// ── Binding modification ──

	/** Set a binding directly (bypasses capture workflow). Returns true on success. */
	UFUNCTION(BlueprintCallable, Category = "Controls")
	bool SetBinding(FGameplayTag InputTag, FKey PrimaryKey, FKey ModifierKey, bool bHasModifier);

	/** Reset a single binding to its default */
	UFUNCTION(BlueprintCallable, Category = "Controls")
	void ResetBinding(FGameplayTag InputTag);

	/** Reset all M&K bindings to defaults */
	UFUNCTION(BlueprintCallable, Category = "Controls")
	void ResetAllBindings();

	/** Set a controller binding directly. Returns true on success. */
	UFUNCTION(BlueprintCallable, Category = "Controls")
	bool SetControllerBinding(FGameplayTag InputTag, FKey PrimaryKey);

	/** Reset a single controller binding to its default */
	UFUNCTION(BlueprintCallable, Category = "Controls")
	void ResetControllerBinding(FGameplayTag InputTag);

	/** Reset all controller bindings to defaults */
	UFUNCTION(BlueprintCallable, Category = "Controls")
	void ResetAllControllerBindings();

	// ── Conflict detection ──

	/**
	 * Check if a key combination conflicts with any existing binding.
	 * @param PrimaryKey The key to check
	 * @param ModifierKey The modifier key (ignored if bHasModifier is false)
	 * @param bHasModifier Whether this is a chord binding
	 * @param ExcludeInputTag Exclude this tag from conflict check (the one being rebound)
	 * @param OutConflictingTag Set to the conflicting InputTag if found
	 * @return True if a conflict was found
	 */
	UFUNCTION(BlueprintPure, Category = "Controls")
	bool CheckForConflicts(FKey PrimaryKey, FKey ModifierKey, bool bHasModifier,
		FGameplayTag ExcludeInputTag, FGameplayTag& OutConflictingTag) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	TObjectPtr<UInputDisplayInfo> InputDisplayInfo;

private:
	void LoadDefaultBindings();
	void LoadControllerDefaultBindings();
	void ApplyAllBindingsToIMC();
	void ApplyAllControllerBindingsToIMC();
	void BroadcastBindings();
	FRageInMageDisplayBinding BuildDisplayBinding(const FGameplayTag& InputTag, const FRageInMageKeyBinding& Binding, bool bCustomized) const;

	UPROPERTY()
	TObjectPtr<USettingsWidgetController> ParentController;

	UPROPERTY()
	TObjectPtr<ARageInMagePlayerController> RagePC;

	UPROPERTY()
	TObjectPtr<URageInMageConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UInputMappingContext> DefaultIMC;

	/** Controller-specific input config and IMC */
	UPROPERTY()
	TObjectPtr<URageInMageConfig> ControllerInputConfig;

	UPROPERTY()
	TObjectPtr<UInputMappingContext> ControllerDefaultIMC;

	/** Default bindings loaded from the original IMC at startup (never modified) */
	TMap<FGameplayTag, FRageInMageKeyBinding> DefaultBindings;

	/** Default controller bindings loaded from the controller IMC at startup */
	TMap<FGameplayTag, FRageInMageKeyBinding> ControllerDefaultBindings;

	// ── Key capture state ──
	bool bIsCapturingKey = false;
	bool bChordCaptureMode = false;
	bool bWaitingForPrimaryKey = false;
	FGameplayTag CaptureTargetInputTag;
	FKey CapturedModifierKey;
};
