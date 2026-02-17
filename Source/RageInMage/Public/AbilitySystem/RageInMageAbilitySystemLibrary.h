// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/CharacterClassInfo.h"
#include "RageInMageAbilitySystemLibrary.generated.h"

class UConditionInfo;
class URageInMageWidgetController;
class UTabbedMenuWidgetController;
class USpellMenuWidgetController;
class UAttributeMenuWidgetController;
struct FGameplayEffectContextHandle;
class UAbilitySystemComponent;
class UOverlayWidgetController;
class USettingsWidgetController;
class UInventoryWidgetController;
struct FRageInMageWidgetControllerParams;
/**
 * 
 */
UCLASS()
class RAGEINMAGE_API URageInMageAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "RageInMageAbilitySystemLibrary|WidgetController")
	static bool MakeGASReferences(APlayerController* PlayerController, FPlayerGASReferences& OutGASRefs, ARageInMageHUD*& OutRageHUD);
	
	UFUNCTION(BlueprintCallable, Category = "RageInMageAbilitySystemLibrary|WidgetController")
	static USpellMenuWidgetController* GetSpellMenuWidgetController(APlayerController* PC);
	
	UFUNCTION(BlueprintCallable, Category = "RageInMageAbilitySystemLibrary|WidgetController")
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(APlayerController* PC);
	
	UFUNCTION(BlueprintCallable, Category = "RageInMageAbilitySystemLibrary|WidgetController")
	static UOverlayWidgetController* GetOverlayWidgetController(APlayerController* PC);

	UFUNCTION(BlueprintCallable, Category = "RageInMageAbilitySystemLibrary|WidgetController")
	static USettingsWidgetController* GetSettingsWidgetController(APlayerController* PC);

	UFUNCTION(BlueprintCallable, Category = "RageInMageAbilitySystemLibrary|WidgetController")
	static UInventoryWidgetController* GetInventoryWidgetController(APlayerController* PC);

	UFUNCTION(BlueprintCallable, Category = "RageInMageAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);

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
	static void GetLivePlayersWithinRadius(
		const UObject* WorldContextObject, float Radius, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, const FVector& SphereOrigin);

	UFUNCTION(BlueprintCallable, Category = "RageInMageAbilitySystemLibrary|GameplayMechanics")
	static bool IsFriendly(AActor* FirstActor, AActor* SecondActor);

	UFUNCTION(BlueprintCallable, Category = "RageInMageAbilitySystemLibrary|GameplayMechanics")
	static bool IsBothEnemy(AActor* FirstActor, AActor* SecondActor);

	UFUNCTION(BlueprintPure, Category = "RageInMageAbilitySystemLibrary|GameplayTags")
	static FGameplayTagContainer GetOwnedGameplayTags(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "RageInMageAbilitySystemLibrary|Conditions")
	static UConditionInfo* GetConditionInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "RageInMageAbilitySystemLibrary|Conditions")
	static bool ApplyConditionToTarget(UAbilitySystemComponent* SourceASC, UAbilitySystemComponent* TargetASC, const FGameplayTag& ConditionTag, const UObject* WorldContextObject);

	static int32 GetXPRewardForClassAndLevel(ECharacterClass CharacterClass, int32 Level, const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "RageInMageAbilitySystemLibrary|Player")
	static int32 GetLocalPlayerIndex(APlayerController* PlayerController);
};