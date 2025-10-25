// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/CharacterClassInfo.h"
#include "RageInMageAbilitySystemLibrary.generated.h"

class UAbilitySystemComponent;
class UOverlayWidgetController;
/**
 * 
 */
UCLASS()
class RAGEINMAGE_API URageInMageAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "RageInMageAbilitySystemLibrary|WidgetController")
	static UOverlayWidgetController* GetOverlayWidgetController(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "RageInMageAbilitySystemLibrary|WidgetController")
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "RageInMageAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(
		const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "RageInMageAbilitySystemLibrary|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC);
};
