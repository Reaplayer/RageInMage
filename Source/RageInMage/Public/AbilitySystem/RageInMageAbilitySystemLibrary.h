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
class UDecalComponent;
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

	UFUNCTION(BlueprintPure, Category = "RageInMAgeAbilitySystemLibrary|GameplayEffects")
	static bool IsIceDamage(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "RageInMAgeAbilitySystemLibrary|GameplayEffects")
	static void SetIsIceDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsIceDamage);

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

	// ── Aim Prediction Utilities ──

	/** Calculate projectile arc trajectory points for visualization.
	 *  ArcParam: 0.0 = straight up, 0.5 = standard arc, 1.0 = near-flat. */
	UFUNCTION(BlueprintCallable, Category = "RageInMageAbilitySystemLibrary|Prediction")
	static TArray<FVector> CalculateProjectileArcPoints(
		const UObject* WorldContextObject,
		const FVector& LaunchLocation,
		const FVector& TargetLocation,
		float ArcParam = 0.5f,
		int32 NumPoints = 30);

	/** Snap a world position to ground level via downward line trace. */
	UFUNCTION(BlueprintPure, Category = "RageInMageAbilitySystemLibrary|Prediction")
	static FVector SnapToGround(const UObject* WorldContextObject,
		const FVector& Location, float TraceDistance = 500.f);

	/** Calculate arc parameter from aim distance (near = flat, far = lobbed).
	 *  MinArc = arc for close range (e.g. 0.8 = flat), MaxArc = arc for max range (e.g. 0.4 = lobbed). */
	UFUNCTION(BlueprintPure, Category = "RageInMageAbilitySystemLibrary|Prediction")
	static float GetArcFromDistance(float Distance, float MaxRange,
		float MinArc = 0.8f, float MaxArc = 0.4f);

	UFUNCTION(BlueprintPure, Category = "RageInMageAbilitySystemLibrary|CharacterClassDefaults", meta = (WorldContext = "WorldContextObject"))
	static FLinearColor GetClassProgressBarColor(const UObject* WorldContextObject, ECharacterClass CharacterClass);

	// ── Decal Utilities ──

	/** Set the DecalSize on a DecalComponent directly (not exposed to BP by default).
	 *  Size is the half-extents in each axis (Width, Width, Depth). */
	UFUNCTION(BlueprintCallable, Category = "RageInMageAbilitySystemLibrary|Decal")
	static void SetDecalSize(UDecalComponent* DecalComponent, const FVector& NewSize);
};