// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "MagePlayerController.generated.h"

class UDamageTextComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class IEnemyInterface;
class URageInMageConfig;
class URageInMageAbilitySystemComponent;
class USplineComponent;

/**
 * 
 */
UCLASS()
class RAGEINMAGE_API AMagePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMagePlayerController();
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> MageContext;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ShiftAction;

	void ShiftPressed() {bShiftKeyDown = true;}
	void ShiftReleased()  {bShiftKeyDown = false;}
	bool bShiftKeyDown = false;

	void Move(const FInputActionValue& InputActionValue);

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
};
