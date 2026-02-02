// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "RageInMageHUD.generated.h"

class URageInMageUserWidget;
class UAbilitySystemComponent;
class UAttributeSet;
class UOverlayWidgetController;
class UAttributeMenuWidgetController;
class USpellMenuWidgetController;
class UTabbedMenuWidgetController;
struct FRageInMageWidgetControllerParams;
/**
 *
 */
UCLASS()
class RAGEINMAGE_API ARageInMageHUD : public AHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	URageInMageWidgetController* GetRageWidgetController(const FRageInMageWidgetControllerParams& WCParams);

	UFUNCTION(BlueprintCallable)
	UTabbedMenuWidgetController* GetTabbedMenuWidgetController(const FRageInMageWidgetControllerParams& WCParams);

	UFUNCTION(BlueprintCallable)
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FRageInMageWidgetControllerParams& WCParams);

	UFUNCTION(BlueprintCallable)
	USpellMenuWidgetController* GetSpellMenuWidgetController(const FRageInMageWidgetControllerParams& WCParams);
	
	UFUNCTION(BlueprintCallable)
	UOverlayWidgetController* GetOverlayWidgetController(const FRageInMageWidgetControllerParams& WCParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

private:

	UPROPERTY()
	TObjectPtr<URageInMageUserWidget> RageWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> RageWidgetClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<URageInMageWidgetController> RageWidgetControllerClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UTabbedMenuWidgetController> TabbedMenuWidgetControllerClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<USpellMenuWidgetController> SpellMenuWidgetControllerClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	// Cached controllers per player
	UPROPERTY()
	TMap<TObjectPtr<APlayerController>, TObjectPtr<URageInMageWidgetController>> RageWidgetControllers;
	
	UPROPERTY()
	TMap<TObjectPtr<APlayerController>, TObjectPtr<UTabbedMenuWidgetController>> TabbedMenuControllers;

	UPROPERTY()
	TMap<TObjectPtr<APlayerController>, TObjectPtr<UAttributeMenuWidgetController>> AttributeMenuControllers;

	UPROPERTY()
	TMap<TObjectPtr<APlayerController>, TObjectPtr<USpellMenuWidgetController>> SpellMenuControllers;

	UPROPERTY()
	TMap<TObjectPtr<APlayerController>, TObjectPtr<UOverlayWidgetController>> OverlayControllers;
};
