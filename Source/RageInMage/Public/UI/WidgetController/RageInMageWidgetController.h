// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Player/RageInMagePlayerState.h"
#include "RageInMageWidgetController.generated.h"

class UTabbedMenuWidgetController;
class URageInMageAttributeSet;
class URageInMageAbilitySystemComponent;
class URageInMageWidgetController;
class UOverlayWidgetController;
class UAttributeSet;
class UAbilitySystemComponent;

USTRUCT(BlueprintType, Blueprintable)
struct FRageInMageWidgetControllerParams
{
	GENERATED_BODY()

	FRageInMageWidgetControllerParams() {}
	FRageInMageWidgetControllerParams(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
	: PlayerController(PC), PlayerState(PS), AbilitySystemComponent(ASC), AttributeSet(AS) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSetBGXPStyleSignature, FSlateColor, ProgressBarColor, UMaterialInstance*, BackgroundMaterial);

/**
 *
 */
UCLASS()
class RAGEINMAGE_API URageInMageWidgetController : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void SetWidgetControllerParams(const FRageInMageWidgetControllerParams& WCParams);

	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues();
	virtual void BindCallbacksToDependencies();
	
	UFUNCTION(BlueprintCallable)
	void SetRageWidgetController(URageInMageWidgetController* Controller) { RageWidgetController = Controller; }
	
	UFUNCTION(BlueprintCallable)
	void SetOverlayWidgetController(UOverlayWidgetController* Controller) { OverlayWidgetController = Controller; }
	
	UFUNCTION(BlueprintCallable)
	void SetTabbedMenuWidgetController(UTabbedMenuWidgetController* Controller) { TabbedMenuWidgetController = Controller; }

	UFUNCTION(BlueprintPure)
	UOverlayWidgetController* GetOverlayWidgetController() const { return OverlayWidgetController; }
	
	UFUNCTION(BlueprintPure)
	URageInMageWidgetController* GetRageWidgetController() const { return RageWidgetController; }
	
	UFUNCTION(BlueprintPure)
	UTabbedMenuWidgetController* GetTabbedMenuWidgetController() const { return TabbedMenuWidgetController; }
	
	UFUNCTION(BlueprintPure)
	ARageInMagePlayerState* GetRagePlayerState() const { return RagePlayerState; }
	
	UFUNCTION(BlueprintPure)
	URageInMageAbilitySystemComponent* GetRageInMageASC() const { return RageASC; }
	
	UFUNCTION(BlueprintPure)
	URageInMageAttributeSet* GetRageInMageAS() const { return RageAS; }

	// Shared delegate for background/XP style - all child controllers can bind to this
	UPROPERTY(BlueprintAssignable, Category = "Style")
	FOnSetBGXPStyleSignature OnSetBGXPStyle;

	// Broadcast stored style to late-joining widgets
	UFUNCTION(BlueprintCallable, Category = "Style")
	void BroadcastStoredBGXPStyle();
	
	UFUNCTION()
	void HandleBGXPStyleChanged(const FSlateColor& ProgressBarColor, UMaterialInstance* BackgroundMaterial);

protected:
	// Store the current style so we can re-broadcast it to late-joining widgets
	FSlateColor StoredProgressBarColor;
	UPROPERTY()
	UMaterialInstance* StoredBackgroundMaterial = nullptr;
	bool bHasStoredStyle = false;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<APlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAttributeSet> AttributeSet;
	
	UPROPERTY()
	TObjectPtr<ARageInMagePlayerState> RagePlayerState;
    
	UPROPERTY()
	TObjectPtr<URageInMageAbilitySystemComponent> RageASC;
    
	UPROPERTY()
	TObjectPtr<URageInMageAttributeSet> RageAS;
	
	UPROPERTY()
	UOverlayWidgetController* OverlayWidgetController;
	
	UPROPERTY()
	UTabbedMenuWidgetController* TabbedMenuWidgetController;
	
	UPROPERTY()
	URageInMageWidgetController* RageWidgetController;
};
