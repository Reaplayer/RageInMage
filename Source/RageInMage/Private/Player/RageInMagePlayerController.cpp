// Copyright Reaplays


#include "Player/RageInMagePlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Data/KeyIconData.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "RageInMageGameplayTag.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Game/RageInMageSettingsSaveGame.h"
#include "Input/RageInMageConfig.h"
#include "Input/RageInMageEInputComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputTriggers.h"
#include "Interaction/EnemyInterface.h"
#include "GameFramework/Character.h"
#include "Player/RageInMagePlayerState.h"
#include "RageInMage/RageInMageLogChannels.h"
#include "UI/HUD/RageInMageHUD.h"
#include "UI/Widget/DamageTextComponent.h"

ARageInMagePlayerController::ARageInMagePlayerController()
{
	bReplicates = true;
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void ARageInMagePlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CursorTrace();
	UpdateAimDirection();
	RotatePawnToFaceAim(DeltaSeconds);
}

ARageInMageHUD* ARageInMagePlayerController::GetRageHUD()
{
	if (!RageHUD)
	{
		RageHUD = Cast<ARageInMageHUD>(GetHUD());
	}
	return RageHUD;
}

void ARageInMagePlayerController::TryInitOverlay()
{
	if (bOverlayInitialized) return;
	if (!IsLocalController()) return;

	APawn* LocalPawn = GetPawn();
	if (!LocalPawn) return;

	ARageInMagePlayerState* RagePS = GetPlayerState<ARageInMagePlayerState>();
	if (!RagePS) return;

	UAbilitySystemComponent* ASC = RagePS->GetAbilitySystemComponent();
	if (!ASC) return;

	ARageInMageHUD* HUD = Cast<ARageInMageHUD>(GetHUD());
	if (!HUD) return;

	HUD->InitOverlay(this, RagePS, ASC, RagePS->GetAttributeSet());
	bOverlayInitialized = true;
}

void ARageInMagePlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bIsCriticalHit, bool bIsVulnerableHit, bool bIsResistantHit)
{
	if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, bIsCriticalHit, bIsVulnerableHit, bIsResistantHit);
	}
}

void ARageInMagePlayerController::AutoRun()
{
	// Click-to-move disabled — movement is WASD/thumbstick only
}

void ARageInMagePlayerController::CursorTrace()
{
	GetHitResultUnderCursor(ECC_Visibility, false, CursorTraceHit);
	if (!CursorTraceHit.bBlockingHit) return;
	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorTraceHit.GetActor());

	if (LastActor != ThisActor)
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (ThisActor) ThisActor->HighlightActor();
	}
}

void ARageInMagePlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	UE_LOG(LogRageInMage, Warning, TEXT("[PC:PRESSED] InputTag=%s"), *InputTag.ToString());
}

void ARageInMagePlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	UE_LOG(LogRageInMage, Warning, TEXT("[PC:RELEASED] InputTag=%s  ASC=%s"), *InputTag.ToString(),
		GetASC() ? TEXT("valid") : TEXT("NULL"));
	if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
}

void ARageInMagePlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	UE_LOG(LogRageInMage, Warning, TEXT("[PC:HELD] InputTag=%s  ASC=%s"), *InputTag.ToString(),
		GetASC() ? TEXT("valid") : TEXT("NULL"));
	if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
}

URageInMageAbilitySystemComponent* ARageInMagePlayerController::GetASC()
{
	if (MageAbilitySystemComponent == nullptr)
	{
		MageAbilitySystemComponent = Cast<URageInMageAbilitySystemComponent>
		(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return MageAbilitySystemComponent;
}


void ARageInMagePlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(MageContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if(Subsystem)
	{
		Subsystem->AddMappingContext(MageContext, 0);
	}

	// Apply any saved custom keybindings
	LoadAndApplyKeybindings();

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);

	// Attempt overlay init — may succeed immediately on listen server,
	// or may need to wait for OnRep_PlayerState on clients.
	TryInitOverlay();
}

void ARageInMagePlayerController::UpdateControllerIMC()
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!Subsystem) return;

	if (bUsingGamepad && !bControllerIMCActive)
	{
		// Activate controller IMC at higher priority (1) so it overrides M&K
		UInputMappingContext* ControllerCtx = bUsingRuntimeControllerContext && RuntimeControllerContext
			? RuntimeControllerContext.Get()
			: ControllerMageContext.Get();
		if (ControllerCtx)
		{
			Subsystem->AddMappingContext(ControllerCtx, 1);
			bControllerIMCActive = true;
		}
	}
	else if (!bUsingGamepad && bControllerIMCActive)
	{
		// Deactivate controller IMC
		if (bUsingRuntimeControllerContext && RuntimeControllerContext)
		{
			Subsystem->RemoveMappingContext(RuntimeControllerContext);
		}
		else if (ControllerMageContext)
		{
			Subsystem->RemoveMappingContext(ControllerMageContext);
		}
		bControllerIMCActive = false;
	}
}

void ARageInMagePlayerController::UpdateCursorForDevice()
{
	if (bUsingGamepad)
	{
		bShowMouseCursor = false;
	}
	else
	{
		bShowMouseCursor = true;
		DefaultMouseCursor = EMouseCursor::Default;
	}
}

void ARageInMagePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	URageInMageEInputComponent* MageEInputComponent = CastChecked<URageInMageEInputComponent>(InputComponent);

	MageEInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARageInMagePlayerController::Move);
	MageEInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &ARageInMagePlayerController::ShiftPressed);
	MageEInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &ARageInMagePlayerController::ShiftReleased);
	if (LookAction)
	{
		MageEInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARageInMagePlayerController::Look);
	}
	MageEInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);

	// NOTE: Controller ability actions are NOT bound separately here.
	// Both IMCs (M&K and Controller) share the same InputAction assets
	// (e.g. IA_Ability_Primary), so the M&K binding above already handles
	// gamepad inputs too. The controller InputTags injected into ability
	// specs by AddCharacterAbilities are used only for UI display (key icons).
}

void ARageInMagePlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void ARageInMagePlayerController::Look(const FInputActionValue& InputActionValue)
{
	GamepadAimInput = InputActionValue.Get<FVector2D>();
	if (GamepadAimInput.SizeSquared() > 0.04f) // Past dead zone
	{
		if (!bUsingGamepad)
		{
			bUsingGamepad = true;
			UpdateControllerIMC();
			UpdateCursorForDevice();
			OnInputDeviceChanged.Broadcast(true);
		}
	}
}

void ARageInMagePlayerController::UpdateAimDirection()
{
	// Detect mouse movement to switch back from gamepad
	if (bUsingGamepad && CursorTraceHit.bBlockingHit)
	{
		const FVector NewCursorPos = CursorTraceHit.ImpactPoint;
		if (!NewCursorPos.Equals(LastCursorPosition, 1.f))
		{
			bUsingGamepad = false;
			UpdateControllerIMC();
			UpdateCursorForDevice();
			OnInputDeviceChanged.Broadcast(false);
		}
	}

	if (bUsingGamepad)
	{
		// Project right stick direction from character onto ground plane
		if (APawn* ControlledPawn = GetPawn())
		{
			if (GamepadAimInput.SizeSquared() > 0.04f)
			{
				const FRotator CamYaw(0.f, GetControlRotation().Yaw, 0.f);
				const FVector Forward = FRotationMatrix(CamYaw).GetUnitAxis(EAxis::X);
				const FVector Right = FRotationMatrix(CamYaw).GetUnitAxis(EAxis::Y);

				const FVector AimDirection = (Forward * GamepadAimInput.Y + Right * GamepadAimInput.X).GetSafeNormal();
				const float StickMagnitude = FMath::Min(GamepadAimInput.Size(), 1.f);
				CurrentAimWorldPosition = ControlledPawn->GetActorLocation() + AimDirection * AimProjectionDistance * StickMagnitude;
			}
		}
	}
	else
	{
		// Mouse path: use cursor hit for aim position
		if (CursorTraceHit.bBlockingHit)
		{
			LastCursorPosition = CursorTraceHit.ImpactPoint;
			CurrentAimWorldPosition = CursorTraceHit.ImpactPoint;
		}
	}
}

void ARageInMagePlayerController::RotatePawnToFaceAim(float DeltaSeconds)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;
	if (CurrentAimWorldPosition.IsZero()) return;

	const FVector PawnLocation = ControlledPawn->GetActorLocation();
	const FVector ToAim = CurrentAimWorldPosition - PawnLocation;

	// Only rotate if there's meaningful distance between pawn and aim point
	if (ToAim.SizeSquared2D() < 100.f) return;

	const FRotator TargetRotation(0.f, ToAim.Rotation().Yaw, 0.f);
	const FRotator CurrentRotation = ControlledPawn->GetActorRotation();
	const FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, AimRotationInterpSpeed);
	ControlledPawn->SetActorRotation(NewRotation);
}

// ── IMC Remapping ──

void ARageInMagePlayerController::ApplyCustomKeybindings(const TMap<FGameplayTag, FRageInMageKeyBinding>& CustomBindings)
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!Subsystem || !MageContext || !InputConfig) return;

	// If no custom bindings, revert to defaults
	if (CustomBindings.Num() == 0)
	{
		ResetKeybindingsToDefault();
		return;
	}

	// Clone the default IMC on first use (or re-clone from original each time for clean state)
	if (bUsingRuntimeContext && RuntimeMageContext)
	{
		Subsystem->RemoveMappingContext(RuntimeMageContext);
	}
	else if (!bUsingRuntimeContext)
	{
		Subsystem->RemoveMappingContext(MageContext);
	}

	RuntimeMageContext = DuplicateObject(MageContext, this);
	bUsingRuntimeContext = true;

	// Apply each custom binding
	for (const auto& [InputTag, Binding] : CustomBindings)
	{
		// Find the InputAction associated with this InputTag
		const UInputAction* Action = InputConfig->FindAbilityInputActionForTag(InputTag);
		if (!Action) continue;

		// Remove existing mappings for this action in the cloned IMC
		// Collect keys first, then unmap (avoid modifying array during iteration)
		TArray<FKey> KeysToRemove;
		for (const FEnhancedActionKeyMapping& Mapping : RuntimeMageContext->GetMappings())
		{
			if (Mapping.Action == Action)
			{
				KeysToRemove.Add(Mapping.Key);
			}
		}
		for (const FKey& Key : KeysToRemove)
		{
			RuntimeMageContext->UnmapKey(Action, Key);
		}

		// Add the new mapping
		FEnhancedActionKeyMapping& NewMapping = RuntimeMageContext->MapKey(Action, Binding.PrimaryKey);

		// For chord bindings, add a ChordAction trigger
		if (Binding.bHasModifier && Binding.ModifierKey.IsValid())
		{
			// Find the InputAction for the modifier key (look it up in InputConfig)
			// First, check if the modifier key is already bound as an InputAction
			const UInputAction* ModifierAction = nullptr;

			// Search InputConfig for an action whose default key matches the modifier
			for (const FMageInputAction& MageAction : InputConfig->AbilityInputActions)
			{
				if (!MageAction.InputAction) continue;

				// Search the original MageContext for this action's key
				for (const FEnhancedActionKeyMapping& OrigMapping : MageContext->GetMappings())
				{
					if (OrigMapping.Action == MageAction.InputAction && OrigMapping.Key == Binding.ModifierKey)
					{
						ModifierAction = MageAction.InputAction;
						break;
					}
				}
				if (ModifierAction) break;
			}

			// Also check system actions (Shift, Move, etc.)
			if (!ModifierAction)
			{
				// Check ShiftAction
				if (ShiftAction)
				{
					for (const FEnhancedActionKeyMapping& OrigMapping : MageContext->GetMappings())
					{
						if (OrigMapping.Action == ShiftAction && OrigMapping.Key == Binding.ModifierKey)
						{
							ModifierAction = ShiftAction;
							break;
						}
					}
				}
			}

			if (ModifierAction)
			{
				UInputTriggerChordAction* ChordTrigger = NewObject<UInputTriggerChordAction>(RuntimeMageContext);
				ChordTrigger->ChordAction = ModifierAction;
				NewMapping.Triggers.Add(ChordTrigger);
			}
			else
			{
				UE_LOG(LogRageInMage, Warning,
					TEXT("ApplyCustomKeybindings: No InputAction found for modifier key '%s' on binding '%s'"),
					*Binding.ModifierKey.ToString(), *InputTag.ToString());
			}
		}
	}

	Subsystem->AddMappingContext(RuntimeMageContext, 0);
	Subsystem->RequestRebuildControlMappings();
}

void ARageInMagePlayerController::ResetKeybindingsToDefault()
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!Subsystem || !MageContext) return;

	if (bUsingRuntimeContext && RuntimeMageContext)
	{
		Subsystem->RemoveMappingContext(RuntimeMageContext);
		RuntimeMageContext = nullptr;
		bUsingRuntimeContext = false;
		Subsystem->AddMappingContext(MageContext, 0);
		Subsystem->RequestRebuildControlMappings();
	}
}

void ARageInMagePlayerController::ApplyControllerCustomKeybindings(const TMap<FGameplayTag, FRageInMageKeyBinding>& CustomBindings)
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!Subsystem || !ControllerMageContext || !ControllerInputConfig) return;

	if (CustomBindings.Num() == 0)
	{
		ResetControllerKeybindingsToDefault();
		return;
	}

	// Remove old context
	if (bUsingRuntimeControllerContext && RuntimeControllerContext)
	{
		Subsystem->RemoveMappingContext(RuntimeControllerContext);
	}
	else if (bControllerIMCActive)
	{
		Subsystem->RemoveMappingContext(ControllerMageContext);
	}

	RuntimeControllerContext = DuplicateObject(ControllerMageContext, this);
	bUsingRuntimeControllerContext = true;

	for (const auto& [InputTag, Binding] : CustomBindings)
	{
		const UInputAction* Action = ControllerInputConfig->FindAbilityInputActionForTag(InputTag);
		if (!Action) continue;

		TArray<FKey> KeysToRemove;
		for (const FEnhancedActionKeyMapping& Mapping : RuntimeControllerContext->GetMappings())
		{
			if (Mapping.Action == Action)
			{
				KeysToRemove.Add(Mapping.Key);
			}
		}
		for (const FKey& Key : KeysToRemove)
		{
			RuntimeControllerContext->UnmapKey(Action, Key);
		}

		RuntimeControllerContext->MapKey(Action, Binding.PrimaryKey);
	}

	if (bControllerIMCActive)
	{
		Subsystem->AddMappingContext(RuntimeControllerContext, 1);
	}
	Subsystem->RequestRebuildControlMappings();
}

void ARageInMagePlayerController::ResetControllerKeybindingsToDefault()
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!Subsystem || !ControllerMageContext) return;

	if (bUsingRuntimeControllerContext && RuntimeControllerContext)
	{
		Subsystem->RemoveMappingContext(RuntimeControllerContext);
		RuntimeControllerContext = nullptr;
		bUsingRuntimeControllerContext = false;

		if (bControllerIMCActive)
		{
			Subsystem->AddMappingContext(ControllerMageContext, 1);
		}
		Subsystem->RequestRebuildControlMappings();
	}
}

void ARageInMagePlayerController::LoadAndApplyKeybindings()
{
	URageInMageSettingsSaveGame* Settings = URageInMageSettingsSaveGame::LoadOrCreateSettings();
	if (Settings)
	{
		if (Settings->CustomKeybindings.Num() > 0)
		{
			ApplyCustomKeybindings(Settings->CustomKeybindings);
		}
		if (Settings->ControllerCustomKeybindings.Num() > 0)
		{
			ApplyControllerCustomKeybindings(Settings->ControllerCustomKeybindings);
		}
	}
}

// ── Key Icon Getters ──

URageInMageConfig* ARageInMagePlayerController::GetActiveInputConfig() const
{
	return bUsingGamepad ? ControllerInputConfig : InputConfig;
}

UInputMappingContext* ARageInMagePlayerController::GetActiveIMC() const
{
	if (bUsingGamepad)
	{
		return bUsingRuntimeControllerContext ? RuntimeControllerContext : ControllerMageContext;
	}
	return bUsingRuntimeContext ? RuntimeMageContext : MageContext;
}

FKey ARageInMagePlayerController::GetBoundKeyForInputTag(const FGameplayTag& InputTag) const
{
	return URageInMageAbilitySystemLibrary::GetKeyForInputTag(InputTag, GetActiveInputConfig(), GetActiveIMC());
}

FText ARageInMagePlayerController::GetKeybindDisplayLabel(const FGameplayTag& InputTag) const
{
	const FKey BoundKey = GetBoundKeyForInputTag(InputTag);
	if (!BoundKey.IsValid()) return FText::GetEmpty();

	// Try DataTable lookup first for custom display labels
	if (KeyIconTable)
	{
		FKeyIconRow IconRow;
		if (URageInMageAbilitySystemLibrary::GetIconForKey(BoundKey, KeyIconTable, IconRow))
		{
			if (!IconRow.DisplayLabel.IsEmpty())
			{
				return IconRow.DisplayLabel;
			}
		}
	}

	// Fallback: use the engine's key display name
	return BoundKey.GetDisplayName();
}

void ARageInMagePlayerController::BroadcastKeybindDisplayChanged(const FGameplayTag& InputTag)
{
	const FText NewLabel = GetKeybindDisplayLabel(InputTag);
	OnKeybindDisplayChanged.Broadcast(InputTag, NewLabel);
}