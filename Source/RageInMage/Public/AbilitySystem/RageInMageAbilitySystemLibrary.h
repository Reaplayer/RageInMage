// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/CharacterClassInfo.h"
#include "RageInMageAbilitySystemLibrary.generated.h"

struct FGameplayEffectContextHandle;
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
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass);

	UFUNCTION(BlueprintCallable, Category = "RageInMageAbilitySystemLibrary|CharacterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "RageInMAgeAbilitySystemLibrary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "RageInMAgeAbilitySystemLibrary|GameplayEffects")
	static bool IsVulnerableHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "RageInMAgeAbilitySystemLibrary|GameplayEffects")
	static bool IsResistantHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "RageInMAgeAbilitySystemLibrary|GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCritHit);

	UFUNCTION(BlueprintCallable, Category = "RageInMAgeAbilitySystemLibrary|GameplayEffects")
	static void SetIsVulnerableHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsVulnerableHit);

	UFUNCTION(BlueprintCallable, Category = "RageInMAgeAbilitySystemLibrary|GameplayEffects")
	static void SetIsResistantHit(FGameplayEffectContextHandle& EffectContextHandle, bool bIsResistantHit);

	UFUNCTION(blueprintCallable, Category = "RageInMageAbilitySystemLibrary|GameplayMechanics")
	static void GetLivePlayersWithinRadius(const UObject* WorldContextObject, float Radius, TArray<AActor*>& OutOverlappingActors, TArray<AActor*>& ActorsToIgnore, const FVector& SphereOrigin);
};