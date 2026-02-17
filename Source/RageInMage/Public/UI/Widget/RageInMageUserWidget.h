// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RageInMageUserWidget.generated.h"

class UOverlayWidgetController;
class UTabbedMenuWidgetController;
class UAttributeMenuWidgetController;
class USpellMenuWidgetController;
class UInventoryWidgetController;

/**
 *
 */
UCLASS()
class RAGEINMAGE_API URageInMageUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;

	// Helper functions to get specific controller types from HUD
	UFUNCTION(BlueprintCallable, Category = "Widget Controller")
	UAttributeMenuWidgetController* GetAttributeMenuController();

	UFUNCTION(BlueprintCallable, Category = "Widget Controller")
	USpellMenuWidgetController* GetSpellMenuController();
	
	UFUNCTION(BlueprintCallable, Category = "Widget Controller")
	UOverlayWidgetController* GetOverlayController();

	UFUNCTION(BlueprintCallable, Category = "Widget Controller")
	UInventoryWidgetController* GetInventoryController();

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
};
