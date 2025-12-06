// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/MageWidgetController.h"
#include "OverlayWidgetController.generated.h"

class URageInMageAbilitySystemComponent;
class UAbilityInfo;
class UMageUserWidget;

struct FRageInMageAbilityInfo;
USTRUCT(BlueprintType, Blueprintable)
struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message = FText();
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UMageUserWidget> MessageWidget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* MessageImage = nullptr;
};

// Delegate for Attributes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);

// Delegate for Widgets
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, FUIWidgetRow, Row);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAbilityInfoSignature, const FRageInMageAbilityInfo&, AbilityInfo, UMaterialInstance*, BackgroundMaterial);

// Delegate for Leveling up
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnXPPercentChangedSignature, float, NewPercent, bool, bLevelUp); // Example usage you might already have or need
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLevelChangedSignature, int32, NewLevel, bool, bLevelUp);

// Delegate for Visuals
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSetBGXPStyleSignature, FSlateColor, ProgressBarColor, UMaterialInstance*, BackgroundMaterial);

/**
 * UOverlayWidgetController is responsible for managing updates and events related to overlay widgets in the game UI.
 * It inherits from UMageWidgetController and extends its functionality by providing specific callbacks
 * and data bindings for the overlay UI elements.
 */
UCLASS(BlueprintType, Blueprintable)
class RAGEINMAGE_API UOverlayWidgetController : public UMageWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitalValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FMessageWidgetRowSignature MessageWidgetRowDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FAbilityInfoSignature AbilityInfoDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FOnSetBGXPStyleSignature OnSetBGXPStyle;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WidgetData")
	TObjectPtr<UDataTable> MessageWidgetDataTable;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WidgetData")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);
	
	void OnInitialiseStartUpAbilities(URageInMageAbilitySystemComponent* RageInMageAbilitySystemComponent);
	
private:
	UMaterialInstance* CachedBGXPMaterialInstance;
};

template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}
