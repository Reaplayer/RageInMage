// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GeneralSettingsWidgetController.generated.h"

class USettingsWidgetController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSensitivityChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUIScaleChangedSignature, float, NewValue);

/**
 * Controls general settings (mouse sensitivity, UI scale).
 * Child of USettingsWidgetController — accesses SaveGame through parent.
 */
UCLASS(Blueprintable)
class RAGEINMAGE_API UGeneralSettingsWidgetController : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(USettingsWidgetController* InParent);
	void BroadcastInitialValues();
	void RevertValues();

	// ── Delegates ──

	UPROPERTY(BlueprintAssignable, Category = "Settings|Input")
	FOnSensitivityChangedSignature OnMouseSensitivityChanged;

	UPROPERTY(BlueprintAssignable, Category = "Settings|UI")
	FOnUIScaleChangedSignature OnUIScaleChanged;

	// ── Input settings ──

	UFUNCTION(BlueprintCallable, Category = "Settings|Input")
	void SetMouseSensitivity(float Value);

	UFUNCTION(BlueprintPure, Category = "Settings|Input")
	float GetMouseSensitivity() const;

	// ── UI settings ──

	UFUNCTION(BlueprintCallable, Category = "Settings|UI")
	void SetUIScale(float Value);

	UFUNCTION(BlueprintPure, Category = "Settings|UI")
	float GetUIScale() const;

private:
	UPROPERTY()
	TObjectPtr<USettingsWidgetController> ParentController;
};
