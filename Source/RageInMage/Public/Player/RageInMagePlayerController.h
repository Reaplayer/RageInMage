// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "RageInMagePlayerController.generated.h"

struct FRageInMageKeyBinding;
class UDamageTextComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class IEnemyInterface;
class URageInMageConfig;
class URageInMageAbilitySystemComponent;
class ARageInMageHUD;
class USplineComponent;

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

	/** Reset keybindings to defaults by restoring the original MageContext. */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void ResetKeybindingsToDefault();

	/** Load saved keybindings and apply them. Called during BeginPlay. */
	void LoadAndApplyKeybindings();

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

	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	float AimProjectionDistance = 1500.f;

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
};
