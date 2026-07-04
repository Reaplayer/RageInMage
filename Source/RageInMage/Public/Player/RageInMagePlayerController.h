// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "RageInMagePlayerController.generated.h"

struct FRageInMageKeyBinding;
class UDamageTextComponent;
class UDataTable;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class IEnemyInterface;
class URageInMageConfig;
class URageInMageAbilitySystemComponent;
class ARageInMageHUD;
class USplineComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputDeviceChanged, bool, bIsGamepad);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnKeybindDisplayChanged, const FGameplayTag&, InputTag, const FText&, NewDisplayLabel);

/**
 *
 */
UCLASS()
class RAGEINMAGE_API ARageInMagePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ARageInMagePlayerController();
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bIsCriticalHit, bool bIsVulnerableHit, bool bIsResistantHit);
	
	UFUNCTION(BlueprintPure)
	ARageInMageHUD* GetRageHUD();

	/**
	 * Attempt to initialize the overlay UI. Safe to call multiple times;
	 * will only init once and silently returns if dependencies aren't ready yet.
	 * Called from both PlayerCharacter::InitPlayerAbilityActorInfo() and
	 * PlayerController::BeginPlay() to handle all timing scenarios.
	 */
	void TryInitOverlay();

	/** Current world-space aim position (cursor or right-stick projected onto ground). */
	UFUNCTION(BlueprintPure, Category = "Aim")
	FVector GetCurrentAimWorldPosition() const { return CurrentAimWorldPosition; }

	/**
	 * Apply custom keybindings by cloning the default IMC and modifying key mappings.
	 * Creates a runtime IMC clone on first call, modifies it with custom bindings.
	 * Supports chord bindings via UInputTriggerChordAction.
	 */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void ApplyCustomKeybindings(const TMap<FGameplayTag, FRageInMageKeyBinding>& CustomBindings);

	/** Apply custom controller keybindings by cloning the default controller IMC. */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void ApplyControllerCustomKeybindings(const TMap<FGameplayTag, FRageInMageKeyBinding>& CustomBindings);

	/** Reset keybindings to defaults by restoring the original MageContext. */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void ResetKeybindingsToDefault();

	/** Reset controller keybindings to defaults by restoring the original controller IMC. */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void ResetControllerKeybindingsToDefault();

	/** Load saved keybindings and apply them. Called during BeginPlay. */
	void LoadAndApplyKeybindings();

	/** Whether the player is currently using a gamepad (vs M&K). */
	UFUNCTION(BlueprintPure, Category = "Input")
	bool IsUsingGamepad() const { return bUsingGamepad; }

	/** Enter spell-aim mode: the right stick becomes a velocity-driven virtual cursor
	 *  (no snapback). Called by the AimMode ability task on activate. InitialReach seeds
	 *  the cursor out in front of the caster (uses AimProjectionDistance if <= 0). */
	void BeginGamepadAim(float InitialReach);

	/** Leave spell-aim mode: the right stick returns to normal direction-based facing
	 *  (with snapback) for turning/dodging. Called by the AimMode task on destroy. */
	void EndGamepadAim();

	/** Broadcast when the active input device changes between M&K and gamepad. */
	UPROPERTY(BlueprintAssignable, Category = "Input")
	FOnInputDeviceChanged OnInputDeviceChanged;

	/** Broadcast when a keybinding changes. Each globe can filter by its own InputTag. */
	UPROPERTY(BlueprintAssignable, Category = "Input")
	FOnKeybindDisplayChanged OnKeybindDisplayChanged;

	/** Fire OnKeybindDisplayChanged for the given InputTag with its current display label. */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void BroadcastKeybindDisplayChanged(const FGameplayTag& InputTag);

	/** Get the FKey currently bound to an InputTag for the active device (M&K or controller).
	 *  Accounts for runtime rebinds. Returns invalid FKey if not found. */
	UFUNCTION(BlueprintPure, Category = "Input")
	FKey GetBoundKeyForInputTag(const FGameplayTag& InputTag) const;

	/** Get the active InputConfig for the current device. */
	UFUNCTION(BlueprintPure, Category = "Input")
	URageInMageConfig* GetActiveInputConfig() const;

	/** Get the active IMC for the current device (accounts for runtime clones). */
	UFUNCTION(BlueprintPure, Category = "Input")
	UInputMappingContext* GetActiveIMC() const;

	/** Get the display label for a given InputTag based on current device and key icon DataTable. */
	UFUNCTION(BlueprintPure, Category = "Input")
	FText GetKeybindDisplayLabel(const FGameplayTag& InputTag) const;

	/** Key icon DataTable for resolving FKey -> display icon/label. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UDataTable> KeyIconTable;

protected:
	UPROPERTY()
	ARageInMageHUD* RageHUD;

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> MageContext;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ShiftAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	/** Controller-specific IMC (stacked on top of MageContext at higher priority when gamepad detected). */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> ControllerMageContext;

	/** Controller-specific InputConfig DataAsset mapping controller InputActions to controller InputTags. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<URageInMageConfig> ControllerInputConfig;

	void ShiftPressed() {bShiftKeyDown = true;}
	void ShiftReleased()  {bShiftKeyDown = false;}
	bool bShiftKeyDown = false;

	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);

	// ── Aim direction ──
	void UpdateAimDirection();
	void RotatePawnToFaceAim(float DeltaSeconds);
	FVector CurrentAimWorldPosition = FVector::ZeroVector;
	FVector LastCursorPosition = FVector::ZeroVector;
	FVector2D GamepadAimInput = FVector2D::ZeroVector;
	bool bUsingGamepad = false;

	/** True only while a spell is being aimed (driven by the AimMode task). Gates the
	 *  velocity virtual-cursor model; when false the right stick does normal facing. */
	bool bAimingSpell = false;

	/** Persistent gamepad "virtual cursor" — a local XY offset from the caster that the
	 *  right stick moves (velocity-integrated). Held between frames so there is no snapback,
	 *  and because it is relative to the caster it keeps its screen position as they move. */
	FVector2D AimCursorOffset = FVector2D::ZeroVector;

	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	float AimProjectionDistance = 1500.f;

	/** How fast (world units/sec at full stick deflection) the gamepad virtual cursor travels. */
	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	float AimCursorSpeed = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	float AimRotationInterpSpeed = 15.f;

	void CursorTrace();
	IEnemyInterface* LastActor;
	IEnemyInterface* ThisActor;
	FHitResult CursorTraceHit;

	void AbilityInputTagPressed (FGameplayTag InputTag);
	void AbilityInputTagReleased (FGameplayTag InputTag);
	void AbilityInputTagHeld (FGameplayTag InputTag);

	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<URageInMageConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<URageInMageAbilitySystemComponent> MageAbilitySystemComponent;

	URageInMageAbilitySystemComponent* GetASC();

	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;
	bool bAutoRunning = false;
	bool bTargeting = false;

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	void AutoRun();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;

	bool bOverlayInitialized = false;

	// ── IMC Remapping ──

	/** Runtime clone of MageContext used for custom keybindings (null if using defaults). */
	UPROPERTY()
	TObjectPtr<UInputMappingContext> RuntimeMageContext;

	bool bUsingRuntimeContext = false;

	/** Runtime clone of ControllerMageContext used for custom controller keybindings. */
	UPROPERTY()
	TObjectPtr<UInputMappingContext> RuntimeControllerContext;

	bool bUsingRuntimeControllerContext = false;

	// ── Device Detection ──

	/** Whether the controller IMC is currently added to the subsystem. */
	bool bControllerIMCActive = false;

	/** Add/remove the controller IMC based on current device state. */
	void UpdateControllerIMC();

	/** Handle cursor visibility when switching devices. */
	void UpdateCursorForDevice();
};
