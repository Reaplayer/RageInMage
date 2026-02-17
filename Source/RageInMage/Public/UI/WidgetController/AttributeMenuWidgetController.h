// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "UI/WidgetController/TabbedMenuWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

struct FGameplayAttribute;
struct FRageInMageAttributeInfo;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FRageInMageAttributeInfo&, Info);

/**
 * Controls the attribute menu UI (primary/secondary stats, point spending)
 */
UCLASS(BlueprintType, Blueprintable)
class RAGEINMAGE_API UAttributeMenuWidgetController : public UTabbedMenuWidgetController
{
	GENERATED_BODY()

public:
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;
	virtual void BeginDestroy() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnPlayerStatChangedSignature AttributePointsChangedDelegate;

	UFUNCTION(BlueprintCallable)
	void UpgradeAttribute(const FGameplayTag& AttributeTag);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TObjectPtr<UAttributeInfo> AttributeInfo;

private:
	void BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute);
	void UnbindFromDependencies();

	// Delegate handles for cleanup
	TMap<FGameplayAttribute, FDelegateHandle> AttributeChangeDelegateHandles;
	FDelegateHandle AttributePointsDelegateHandle;
	FDelegateHandle SpellPointsDelegateHandle;
};
