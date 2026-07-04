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
	// On gamepad, highlighting is driven by the virtual cursor in UpdateAimDirection.
	// We still run the trace above so CursorTraceHit stays fresh for mouse-move detection
	// (the mouse -> gamepad switch-back in UpdateAimDirection compares against it).
	if (bUsingGamepad) return;
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

		// Keep the controller context active alongside M&K for the whole session.
		// The two contexts map disjoint physical keys (keyboard/mouse vs Gamepad_*),
		// so there is no conflict. This must be added up-front: the only input that
		// flips bUsingGamepad (the right-stick Look) is mapped *only* in this context,
		// so gating the context behind gamepad detection can never bootstrap itself.
		if (ControllerMageContext)
		{
			Subsystem->AddMappingContext(ControllerMageContext, 1);
			bControllerIMCActive = true;
		}
	}

	// Apply any saved custom keybindings (may swap ControllerMageContext for a runtime copy)
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
	// Both the M&K and controller mapping contexts are added once in BeginPlay and
	// stay active for the whole session (they map disjoint physical keys). Switching
	// input device only affects the cursor and aim mode (bUsingGamepad), not which
	// contexts are mapped — tearing the controller context down here would make it
	// impossible to switch back to the gamepad, since the right-stick Look that
	// re-detects the gamepad lives inside that very context.
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
		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector Origin = ControlledPawn->GetActorLocation();

			if (bAimingSpell)
			{
				// AIMING A SPELL: right stick drives a persistent "virtual cursor" — stick
				// deflection is a VELOCITY that moves the cursor, not an absolute direction.
				// The cursor is a local XY offset from the caster (AimCursorOffset), so
				// releasing the stick holds it in place (no snapback) and it keeps its
				// position relative to the character as they move.
				const float DeltaTime = GetWorld() ? GetWorld()->GetDeltaSeconds() : 0.f;

				if (GamepadAimInput.SizeSquared() > 0.04f) // past dead zone
				{
					const FRotator CamYaw(0.f, GetControlRotation().Yaw, 0.f);
					const FVector Forward = FRotationMatrix(CamYaw).GetUnitAxis(EAxis::X);
					const FVector Right = FRotationMatrix(CamYaw).GetUnitAxis(EAxis::Y);

					const FVector MoveDir = Forward * GamepadAimInput.Y + Right * GamepadAimInput.X;
					AimCursorOffset += FVector2D(MoveDir.X, MoveDir.Y) * AimCursorSpeed * DeltaTime;

					// Clamp reach so the cursor can't drift off to infinity.
					if (AimCursorOffset.Size() > AimProjectionDistance)
					{
						AimCursorOffset = AimCursorOffset.GetSafeNormal() * AimProjectionDistance;
					}
				}

				FVector AimPos = Origin + FVector(AimCursorOffset.X, AimCursorOffset.Y, 0.f);

				// Resolve Z by tracing straight down the cursor column: snaps the aim onto the
				// ground or whatever object sits under the cursor, and highlights an enemy there.
				FHitResult AimHit;
				const FVector TraceStart = AimPos + FVector(0.f, 0.f, 10000.f);
				const FVector TraceEnd   = AimPos - FVector(0.f, 0.f, 10000.f);
				FCollisionQueryParams AimTraceParams;
				AimTraceParams.AddIgnoredActor(ControlledPawn);
				if (GetWorld() && GetWorld()->LineTraceSingleByChannel(AimHit, TraceStart, TraceEnd, ECC_Visibility, AimTraceParams))
				{
					AimPos.Z = AimHit.ImpactPoint.Z;

					LastActor = ThisActor;
					ThisActor = Cast<IEnemyInterface>(AimHit.GetActor());
					if (LastActor != ThisActor)
					{
						if (LastActor) LastActor->UnHighlightActor();
						if (ThisActor) ThisActor->HighlightActor();
					}
				}
				else
				{
					AimPos.Z = Origin.Z;
				}

				CurrentAimWorldPosition = AimPos;
			}
			else if (GamepadAimInput.SizeSquared() > 0.04f)
			{
				// NORMAL MOVEMENT: right stick is a direction for character facing/dodging.
				// Distance scales with stick push so releasing snaps the aim back toward the
				// caster — keeps turning tight and dodges pointed the right way.
				const FRotator CamYaw(0.f, GetControlRotation().Yaw, 0.f);
				const FVector Forward = FRotationMatrix(CamYaw).GetUnitAxis(EAxis::X);
				const FVector Right = FRotationMatrix(CamYaw).GetUnitAxis(EAxis::Y);

				const FVector AimDirection = (Forward * GamepadAimInput.Y + Right * GamepadAimInput.X).GetSafeNormal();
				const float StickMagnitude = FMath::Min(GamepadAimInput.Size(), 1.f);
				CurrentAimWorldPosition = Origin + AimDirection * AimProjectionDistance * StickMagnitude;
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

void ARageInMagePlayerController::BeginGamepadAim(float InitialReach)
{
	bAimingSpell = true;

	// Seed the virtual cursor out in front of the caster so the spell starts aimed where
	// the character faces, rather than on top of itself. The stick then moves it from here.
	const float Reach = InitialReach > 0.f ? FMath::Min(InitialReach, AimProjectionDistance) : AimProjectionDistance;
	if (const APawn* ControlledPawn = GetPawn())
	{
		const FVector Fwd = ControlledPawn->GetActorForwardVector();
		AimCursorOffset = FVector2D(Fwd.X, Fwd.Y).GetSafeNormal() * Reach;
	}
	else
	{
		AimCursorOffset = FVector2D(Reach, 0.f);
	}
}

void ARageInMagePlayerController::EndGamepadAim()
{
	bAimingSpell = false;
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