// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RageInMageHUD.generated.h"

class URageInMageUserWidget;
class UAbilitySystemComponent;
class UAttributeSet;
class UOverlayWidgetController;
class UAttributeMenuWidgetController;
class USpellMenuWidgetController;
class USettingsWidgetController;
class UInventoryWidgetController;

// Cached GAS references per player
USTRUCT(BlueprintType, Blueprintable)
struct FPlayerGASReferences
{
	GENERATED_BODY()
	FPlayerGASReferences() {}
	FPlayerGASReferences(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
		: PlayerController(PC), PlayerState(PS), ASC(ASC), AttributeSet(AS)
	{
		check(PlayerController);
		check(PlayerState);
		check(ASC);
		check(AS);
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet;
};

/**
 * Central HUD that owns and provides access to all widget controllers.
 * This is the ONLY place that creates controllers.
 * Controllers never reference each other directly.
 */
UCLASS()
class RAGEINMAGE_API ARageInMageHUD : public AHUD
{
	GENERATED_BODY()

public:
	/**
	 * Initialize the main overlay UI (health, mana, XP bar, etc.)
	 * Called by PlayerController on BeginPlay
	 */
	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

	/**
	 * Controller accessors - Widgets call these to get their specific controller
	 * These use lazy initialization - controllers are created on first access
	 */
	UFUNCTION(BlueprintCallable, Category = "Widget Controllers")
	UOverlayWidgetController* GetOverlayWidgetController(APlayerController* PC);

	UFUNCTION(BlueprintCallable, Category = "Widget Controllers")
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(APlayerController* PC);

	UFUNCTION(BlueprintCallable, Category = "Widget Controllers")
	USpellMenuWidgetController* GetSpellMenuWidgetController(APlayerController* PC);

	UFUNCTION(BlueprintCallable, Category = "Widget Controllers")
	USettingsWidgetController* GetSettingsWidgetController(APlayerController* PC);

	UFUNCTION(BlueprintCallable, Category = "Widget Controllers")
	UInventoryWidgetController* GetInventoryWidgetController(APlayerController* PC);

private:
	// Helper to create controllers with proper GAS setup
	void CreateOverlayController(APlayerController* PC);
	void CreateAttributeMenuController(APlayerController* PC);
	void CreateSpellMenuController(APlayerController* PC);
	void CreateSettingsController(APlayerController* PC);
	void CreateInventoryController(APlayerController* PC);

	// Widget classes - set in Blueprint
	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<URageInMageUserWidget> OverlayWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<URageInMageUserWidget> TabbedMenuWidgetClass;

	// Controller classes - set in Blueprint
	UPROPERTY(EditAnywhere, Category = "Widget Controllers")
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	UPROPERTY(EditAnywhere, Category = "Widget Controllers")
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;

	UPROPERTY(EditAnywhere, Category = "Widget Controllers")
	TSubclassOf<USpellMenuWidgetController> SpellMenuWidgetControllerClass;

	UPROPERTY(EditAnywhere, Category = "Widget Controllers")
	TSubclassOf<USettingsWidgetController> SettingsWidgetControllerClass;

	UPROPERTY(EditAnywhere, Category = "Widget Controllers")
	TSubclassOf<UInventoryWidgetController> InventoryWidgetControllerClass;

	// Controller instances - per player (for split-screen support)
	UPROPERTY()
	TMap<TObjectPtr<APlayerController>, TObjectPtr<UOverlayWidgetController>> OverlayControllers;

	UPROPERTY()
	TMap<TObjectPtr<APlayerController>, TObjectPtr<UAttributeMenuWidgetController>> AttributeMenuControllers;

	UPROPERTY()
	TMap<TObjectPtr<APlayerController>, TObjectPtr<USpellMenuWidgetController>> SpellMenuControllers;

	UPROPERTY()
	TMap<TObjectPtr<APlayerController>, TObjectPtr<USettingsWidgetController>> SettingsControllers;

	UPROPERTY()
	TMap<TObjectPtr<APlayerController>, TObjectPtr<UInventoryWidgetController>> InventoryControllers;

	// Widget instances
	UPROPERTY()
	TObjectPtr<URageInMageUserWidget> OverlayWidget;

	UPROPERTY()
	TObjectPtr<URageInMageUserWidget> AttributeMenuWidget;

	UPROPERTY()
	TObjectPtr<URageInMageUserWidget> SpellMenuWidget;

	UPROPERTY()
	TMap<TObjectPtr<APlayerController>, FPlayerGASReferences> PlayerGASCache;
};