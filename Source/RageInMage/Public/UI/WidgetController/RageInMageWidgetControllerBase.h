// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "RageInMageWidgetControllerBase.generated.h"

class ARageInMagePlayerController;
class ARageInMagePlayerState;
class URageInMageAbilitySystemComponent;
class URageInMageAttributeSet;
class UAbilitySystemComponent;
class UAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSetBGXPStyleSignature, FSlateColor, ProgressBarColor, UMaterialInstance*, BackgroundMaterial);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FRageInMageAbilityInfo&, AbilityInfo);

/**
 * Base class for all widget controllers.
 * Controllers are INDEPENDENT and do NOT reference other controllers.
 * They only interact with GAS (Gameplay Ability System).
 */
UCLASS(Abstract, Blueprintable)
class RAGEINMAGE_API URageInMageWidgetControllerBase : public UObject
{
	GENERATED_BODY()

public:
	// Set GAS references - called by HUD when controller is created
	void SetGASReferences(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

	// Override in subclasses to bind to GAS attribute/gameplay event delegates
	virtual void BindCallbacksToDependencies();

	// Override in subclasses to broadcast initial attribute values to UI.
	// Widgets should call this AFTER binding to delegates (e.g. in WidgetControllerSet).
	UFUNCTION(BlueprintCallable, Category = "Widget Controller")
	virtual void BroadcastInitialValues();
	
	// Shared delegate for background/XP style - all child controllers can bind to this
	UPROPERTY(BlueprintAssignable, Category = "Style")
	FOnSetBGXPStyleSignature BGXPStyleDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FAbilityInfoSignature AbilityInfoDelegate;

	// Broadcast stored style to late-joining widgets
	UFUNCTION(BlueprintCallable, Category = "Style")
	void BroadcastClassVisuals();
	
	UFUNCTION(BlueprintCallable, Category = "Style")
	void BroadcastAbilityInfo();

protected:
	// Base GAS references
	UPROPERTY(BlueprintReadOnly, Category = "Controller")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "Controller")
	TObjectPtr<APlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly, Category = "Controller")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Controller")
	TObjectPtr<UAttributeSet> AttributeSet;

	// Typed references (cached after casting)
	UPROPERTY()
	TObjectPtr<ARageInMagePlayerController> RagePC;

	UPROPERTY()
	TObjectPtr<ARageInMagePlayerState> RagePS;

	UPROPERTY()
	TObjectPtr<URageInMageAbilitySystemComponent> RageASC;

	UPROPERTY()
	TObjectPtr<URageInMageAttributeSet> RageAS;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	// Helper getters
	ARageInMagePlayerController* GetRagePC();
	ARageInMagePlayerState* GetRagePS();
	URageInMageAbilitySystemComponent* GetRageASC();
	URageInMageAttributeSet* GetRageAS();
};