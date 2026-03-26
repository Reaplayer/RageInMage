// Copyright Reaplays

#include "UI/WidgetController/ControlsWidgetController.h"
#include "UI/WidgetController/SettingsWidgetController.h"
#include "AbilitySystem/Data/InputDisplayInfo.h"
#include "Game/RageInMageSettingsSaveGame.h"
#include "Input/RageInMageConfig.h"
#include "Player/RageInMagePlayerController.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "RageInMage/RageInMageLogChannels.h"

void UControlsWidgetController::Initialize(USettingsWidgetController* InParent, ARageInMagePlayerController* InPC,
	URageInMageConfig* InInputConfig, UInputMappingContext* InDefaultIMC)
{
	ParentController = InParent;
	RagePC = InPC;
	InputConfig = InInputConfig;
	DefaultIMC = InDefaultIMC;

	LoadDefaultBindings();
}

void UControlsWidgetController::BroadcastInitialValues()
{
	BroadcastBindings();
}

void UControlsWidgetController::RevertValues()
{
	BroadcastBindings();
}

void UControlsWidgetController::LoadDefaultBindings()
{
	if (!InputConfig || !DefaultIMC) return;

	DefaultBindings.Empty();

	// For each ability input action in the config, find its default key in the IMC
	for (const FMageInputAction& MageAction : InputConfig->AbilityInputActions)
	{
		if (!MageAction.InputAction || !MageAction.InputTag.IsValid()) continue;

		// Search the default IMC for mappings of this InputAction
		const TArray<FEnhancedActionKeyMapping>& Mappings = DefaultIMC->GetMappings();
		for (const FEnhancedActionKeyMapping& Mapping : Mappings)
		{
			if (Mapping.Action == MageAction.InputAction)
			{
				FRageInMageKeyBinding DefaultBinding;
				DefaultBinding.InputTag = MageAction.InputTag;
				DefaultBinding.PrimaryKey = Mapping.Key;
				DefaultBinding.bHasModifier = false;

				// Check if this mapping has keyboard modifier flags
				// (EnhancedInput stores these in the mapping's Modifiers/Triggers arrays,
				//  but basic shift/ctrl/alt are part of key chord logic — we store them plainly)

				DefaultBindings.Add(MageAction.InputTag, DefaultBinding);
				break; // Take the first mapping as default
			}
		}
	}
}

TArray<FRageInMageDisplayBinding> UControlsWidgetController::GetAllBindings()
{
	TArray<FRageInMageDisplayBinding> Result;

	URageInMageSettingsSaveGame* Settings = ParentController ? ParentController->GetCurrentSettings() : nullptr;

	for (const auto& [InputTag, DefaultBinding] : DefaultBindings)
	{
		// Check for custom override
		const FRageInMageKeyBinding* CustomBinding = Settings ? Settings->CustomKeybindings.Find(InputTag) : nullptr;

		if (CustomBinding)
		{
			Result.Add(BuildDisplayBinding(InputTag, *CustomBinding, true));
		}
		else
		{
			Result.Add(BuildDisplayBinding(InputTag, DefaultBinding, false));
		}
	}

	return Result;
}

void UControlsWidgetController::BeginCaptureKey(FGameplayTag InputTag, bool bChordMode)
{
	if (!InputTag.IsValid()) return;

	bIsCapturingKey = true;
	bChordCaptureMode = bChordMode;
	bWaitingForPrimaryKey = !bChordMode; // In chord mode, first key is modifier; in single, first key is primary
	CaptureTargetInputTag = InputTag;
	CapturedModifierKey = FKey();
}

void UControlsWidgetController::CancelCaptureKey()
{
	bIsCapturingKey = false;
	bChordCaptureMode = false;
	bWaitingForPrimaryKey = false;
	CaptureTargetInputTag = FGameplayTag();
	CapturedModifierKey = FKey();
	OnCaptureCancelled.Broadcast();
}

void UControlsWidgetController::OnKeyPressed(FKey Key)
{
	if (!bIsCapturingKey) return;

	// Ignore Escape — treat as cancel
	if (Key == EKeys::Escape)
	{
		CancelCaptureKey();
		return;
	}

	OnKeyCaptured.Broadcast(Key);

	if (bChordCaptureMode && !bWaitingForPrimaryKey)
	{
		// First key in chord mode = modifier
		CapturedModifierKey = Key;
		bWaitingForPrimaryKey = true;
		// Widget should display "Now press the main key..." and continue listening
		return;
	}

	// Single-key mode or second key in chord mode — apply binding
	const FKey PrimaryKey = Key;
	const FKey ModifierKey = bChordCaptureMode ? CapturedModifierKey : FKey();
	const bool bHasModifier = bChordCaptureMode && ModifierKey.IsValid();

	// Check for conflicts
	FGameplayTag ConflictingTag;
	if (CheckForConflicts(PrimaryKey, ModifierKey, bHasModifier, CaptureTargetInputTag, ConflictingTag))
	{
		FText ConflictName = InputDisplayInfo
			? InputDisplayInfo->GetDisplayNameForTag(ConflictingTag)
			: FText::FromString(ConflictingTag.ToString());
		OnBindingConflict.Broadcast(ConflictingTag,
			FText::Format(NSLOCTEXT("Controls", "ConflictMsg", "This key is already bound to {0}"), ConflictName));
		// Don't cancel capture — let widget decide (show warning, ask to swap, or retry)
		return;
	}

	// Apply the binding
	SetBinding(CaptureTargetInputTag, PrimaryKey, ModifierKey, bHasModifier);

	// End capture
	bIsCapturingKey = false;
	bChordCaptureMode = false;
	bWaitingForPrimaryKey = false;
	CaptureTargetInputTag = FGameplayTag();
	CapturedModifierKey = FKey();
}

bool UControlsWidgetController::SetBinding(FGameplayTag InputTag, FKey PrimaryKey, FKey ModifierKey, bool bHasModifier)
{
	if (!InputTag.IsValid() || !PrimaryKey.IsValid()) return false;

	URageInMageSettingsSaveGame* Settings = ParentController ? ParentController->GetCurrentSettings() : nullptr;
	if (!Settings) return false;

	// Check if the new binding matches the default — if so, remove the custom override
	const FRageInMageKeyBinding* DefaultBinding = DefaultBindings.Find(InputTag);
	if (DefaultBinding && DefaultBinding->PrimaryKey == PrimaryKey
		&& !bHasModifier && !DefaultBinding->bHasModifier)
	{
		// Same as default — remove custom override
		Settings->CustomKeybindings.Remove(InputTag);
	}
	else
	{
		// Store custom binding
		FRageInMageKeyBinding NewBinding(InputTag, PrimaryKey);
		if (bHasModifier && ModifierKey.IsValid())
		{
			NewBinding.ModifierKey = ModifierKey;
			NewBinding.bHasModifier = true;
		}
		Settings->CustomKeybindings.Add(InputTag, NewBinding);
	}

	// Apply to the active IMC
	ApplyAllBindingsToIMC();

	// Broadcast updated bindings to UI
	BroadcastBindings();

	return true;
}

void UControlsWidgetController::ResetBinding(FGameplayTag InputTag)
{
	URageInMageSettingsSaveGame* Settings = ParentController ? ParentController->GetCurrentSettings() : nullptr;
	if (!Settings) return;

	Settings->CustomKeybindings.Remove(InputTag);

	ApplyAllBindingsToIMC();
	BroadcastBindings();
}

void UControlsWidgetController::ResetAllBindings()
{
	URageInMageSettingsSaveGame* Settings = ParentController ? ParentController->GetCurrentSettings() : nullptr;
	if (!Settings) return;

	Settings->CustomKeybindings.Empty();

	if (RagePC)
	{
		RagePC->ResetKeybindingsToDefault();
	}

	BroadcastBindings();
}

bool UControlsWidgetController::CheckForConflicts(FKey PrimaryKey, FKey ModifierKey, bool bHasModifier,
	FGameplayTag ExcludeInputTag, FGameplayTag& OutConflictingTag) const
{
	URageInMageSettingsSaveGame* Settings = ParentController ? ParentController->GetCurrentSettings() : nullptr;

	for (const auto& [InputTag, DefaultBinding] : DefaultBindings)
	{
		if (InputTag.MatchesTagExact(ExcludeInputTag)) continue;

		// Get the active binding (custom override or default)
		const FRageInMageKeyBinding* ActiveBinding = nullptr;
		FRageInMageKeyBinding CustomBinding;
		if (Settings)
		{
			if (const FRageInMageKeyBinding* Custom = Settings->CustomKeybindings.Find(InputTag))
			{
				CustomBinding = *Custom;
				ActiveBinding = &CustomBinding;
			}
		}
		if (!ActiveBinding)
		{
			ActiveBinding = &DefaultBinding;
		}

		// Check for conflict: same primary key AND same modifier status
		if (ActiveBinding->PrimaryKey == PrimaryKey)
		{
			if (bHasModifier == ActiveBinding->bHasModifier)
			{
				if (!bHasModifier || ModifierKey == ActiveBinding->ModifierKey)
				{
					OutConflictingTag = InputTag;
					return true;
				}
			}
			// A single-key binding conflicts with a chord that uses the same primary
			// (because pressing the key without modifier would also trigger it)
			// Exception: if the existing binding HAS a modifier and new one doesn't, that's OK
			// because the existing one requires an extra key.
			// But if new binding HAS NO modifier and existing HAS NO modifier, that's a conflict.
			// This is already handled above.
		}
	}

	return false;
}

void UControlsWidgetController::ApplyAllBindingsToIMC()
{
	URageInMageSettingsSaveGame* Settings = ParentController ? ParentController->GetCurrentSettings() : nullptr;
	if (!Settings || !RagePC) return;

	RagePC->ApplyCustomKeybindings(Settings->CustomKeybindings);
}

void UControlsWidgetController::BroadcastBindings()
{
	TArray<FRageInMageDisplayBinding> Bindings = GetAllBindings();
	OnBindingsChanged.Broadcast(Bindings);
}

FRageInMageDisplayBinding UControlsWidgetController::BuildDisplayBinding(const FGameplayTag& InputTag,
	const FRageInMageKeyBinding& Binding, bool bCustomized) const
{
	FRageInMageDisplayBinding Display;
	Display.InputTag = InputTag;
	Display.PrimaryKey = Binding.PrimaryKey;
	Display.ModifierKey = Binding.ModifierKey;
	Display.bHasModifier = Binding.bHasModifier;
	Display.bIsCustomized = bCustomized;
	Display.PrimaryKeyDisplayText = Binding.PrimaryKey.GetDisplayName();
	Display.ModifierKeyDisplayText = Binding.bHasModifier
		? Binding.ModifierKey.GetDisplayName()
		: FText::GetEmpty();

	if (InputDisplayInfo)
	{
		Display.DisplayName = InputDisplayInfo->GetDisplayNameForTag(InputTag);
		if (const FInputTagDisplayInfo* Info = InputDisplayInfo->FindInfoForTag(InputTag))
		{
			Display.Category = Info->Category;
			Display.bIsRemappable = Info->bIsRemappable;
		}
	}
	else
	{
		Display.DisplayName = FText::FromString(InputTag.ToString());
	}

	return Display;
}
