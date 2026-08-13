// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/CharacterClassInfo.h"
#include "Data/KeyIconData.h"
#include "RageInMageAbilitySystemLibrary.generated.h"

class UConditionInfo;
class UNiagaraSystem;
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
class UDataTable;
class UInputMappingContext;
class URageInMageConfig;
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

	/** Applies UConditionInfo's shared StunImmunityEffect to TargetASC for TotalDuration (typically a
	 *  stun's own duration plus a grace period) via SetByCaller(Condition.StunImmune). Granting this
	 *  alongside a stun — rather than only on that stun's natural expiry — guarantees immunity covers
	 *  the whole stun+grace window even if the stun gets refreshed/extended, closing off any path to a
	 *  permanent stun-lock. Does not affect an already-active Condition.Stunned effect; it only blocks
	 *  future stun applications that check for it (e.g. via BlockedByConditions). */
	UFUNCTION(BlueprintCallable, Category = "RageInMageAbilitySystemLibrary|Conditions")
	static void ApplyStunImmunity(UAbilitySystemComponent* SourceASC, UAbilitySystemComponent* TargetASC, const UObject* WorldContextObject, float TotalDuration);

	/** Composes a ConditionInfo base value with a per-character bonus attribute.
	 *  The ConditionInfo DataAsset is shared by every character and must never be mutated, so all
	 *  per-character variation (passives, items) is layered on HERE, at apply time, by reading an
	 *  attribute that those passives/items drive via GameplayEffects.
	 *  BonusAttribute unset -> returns BaseValue untouched. bFromTarget selects whose attribute is read:
	 *  false = the source ("my CCs are stronger"), true = the target ("I resist CC").
	 *  MaxClamp <= 0 means "no ceiling". Clamping matters: enough stacked bonuses could otherwise
	 *  produce a zero-length condition or an effectively permanent immunity window. */
	UFUNCTION(BlueprintPure, Category = "RageInMageAbilitySystemLibrary|Conditions")
	static float ResolveConditionValue(float BaseValue, FGameplayAttribute BonusAttribute, bool bFromTarget,
		UAbilitySystemComponent* SourceASC, UAbilitySystemComponent* TargetASC,
		float MinClamp = 0.f, float MaxClamp = 0.f);

	/** Per-condition immunity. Applies ImmunityEffect for TotalDuration with its duration fed via
	 *  SetByCaller(ImmunityTag). Falls back to UConditionInfo's shared StunImmunityEffect /
	 *  Condition.StunImmune when either argument is unset, which reproduces the old single-immunity
	 *  behaviour. Giving each CC its own immunity tag is what lets a stun, a petrify and a freeze be
	 *  chained on one target while none of them can be re-applied on top of itself. */
	UFUNCTION(BlueprintCallable, Category = "RageInMageAbilitySystemLibrary|Conditions")
	static void ApplyConditionImmunity(UAbilitySystemComponent* SourceASC, UAbilitySystemComponent* TargetASC,
		const UObject* WorldContextObject, TSubclassOf<UGameplayEffect> ImmunityEffect,
		FGameplayTag ImmunityTag, float TotalDuration);

	/** For abilities with a guaranteed, unconditional stun (e.g. Flash And Awe) that should land even
	 *  through an existing Condition.StunImmune grace window. Strips any active StunImmunity from
	 *  TargetASC, applies UConditionInfo's shared StunnedEffect for StunDuration, then re-grants
	 *  StunImmunity for StunDuration + GraceSeconds so the target isn't immediately re-stunnable. */
	UFUNCTION(BlueprintCallable, Category = "RageInMageAbilitySystemLibrary|Conditions")
	static void ApplyGuaranteedStun(UAbilitySystemComponent* SourceASC, UAbilitySystemComponent* TargetASC, const UObject* WorldContextObject, float StunDuration, float GraceSeconds);

	// ── Lightning Chain ──

	/** Applies a lightning damage spec to InitialTargetActor, then chains to the nearest enemy
	 *  actor within JumpRadius, repeating from each new target — but only as long as the target
	 *  that was just hit owns Condition.Charged (granted at Charge >= 50 via ConditionInfo); if a
	 *  hit lands on a target that isn't and doesn't become Charged from that hit, the chain stops.
	 *  Each successive jump's damage (and other SetByCaller magnitudes on the spec, e.g. Charge
	 *  gain) is multiplied by (1 - DamageFalloffPerJump) cumulatively, EXCEPT a jump leaving a
	 *  target that owns Condition.OverCharged (Charge >= 100) always chains at 100% damage.
	 *  A target already hit by this instance is never hit again.
	 *  Charge accumulation and the Charge >= 100 -> OverCharged/Stunned threshold are handled
	 *  automatically by the existing ConditionInfo/HandleMechanicsThreshold pipeline as each hit lands. */
	UFUNCTION(BlueprintCallable, Category = "RageInMageAbilitySystemLibrary|Lightning")
	static void ApplyChainLightningDamage(
		AActor* InstigatorActor,
		AActor* InitialTargetActor,
		const FGameplayEffectSpecHandle& DamageEffectSpecHandle,
		float JumpRadius = 350.f,
		float DamageFalloffPerJump = 0.1f,
		int32 MaxJumps = 10,
		UNiagaraSystem* ImpactEffect = nullptr);

	// ── Immovable Mass (Earth) ──

	/** The Actor's current Immovable Mass stance stage (0-3). Returns 0 if it has no
	 *  UImmovableMassComponent or the stance is inactive. Any Earth ability/actor reads this to scale
	 *  itself off the caster's stance stage. */
	UFUNCTION(BlueprintPure, Category = "RageInMageAbilitySystemLibrary|ImmovableMass")
	static int32 GetImmovableMassStage(const AActor* Actor);

	/** Convenience: 1 + (per-stage percent / 100) for the Actor's current stance stage (1.0 at stage 0).
	 *  Pass the three stage percents (e.g. 10, 20, 30). Negative percents give reductions — e.g. the
	 *  Boulder's grow-time cut (-15, -30, -50) yields 0.85 / 0.70 / 0.50. */
	UFUNCTION(BlueprintPure, Category = "RageInMageAbilitySystemLibrary|ImmovableMass")
	static float GetImmovableMassStageScalar(const AActor* Actor, float Stage1Percent, float Stage2Percent, float Stage3Percent);

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

	// ── InputConfig Lookups ──

	/** Find the InputTag and ModifierTag for a given AbilityTypeTag in the InputConfig.
	 *  Returns true if found. Use this to query "what keys does the Primary/Defensive/etc ability use?" */
	UFUNCTION(BlueprintPure, Category = "RageInMageAbilitySystemLibrary|Input")
	static bool GetInputTagsForAbilityType(const FGameplayTag& AbilityTypeTag, const URageInMageConfig* InputConfig,
		FGameplayTag& OutInputTag, FGameplayTag& OutModifierTag);

	/** Find the InputAction for a given InputTag, optionally narrowing by ModifierTag.
	 *  If ModifierTag is valid: finds all entries matching the InputTag, then returns the one
	 *  whose InputModifierTag matches. If ModifierTag is empty: returns the entry matching
	 *  the InputTag that has NO modifier (InputModifierTag is empty).
	 *  Returns nullptr if not found. */
	UFUNCTION(BlueprintPure, Category = "RageInMageAbilitySystemLibrary|Input")
	static const UInputAction* FindInputActionByTags(const URageInMageConfig* InputConfig,
		const FGameplayTag& InputTag, const FGameplayTag& ModifierTag);

	// ── Key Icon Lookup ──

	/** Look up the FKey currently bound to a given InputTag in the active IMC.
	 *  Searches the InputConfig for the matching InputAction, then finds
	 *  which key is mapped to that action in the provided IMC. */
	UFUNCTION(BlueprintPure, Category = "RageInMageAbilitySystemLibrary|Input")
	static FKey GetKeyForInputTag(const FGameplayTag& InputTag, const URageInMageConfig* InputConfig, const UInputMappingContext* IMC);

	/** Look up the icon row for a given FKey from a KeyIcon DataTable.
	 *  Returns true if found, filling OutRow. */
	UFUNCTION(BlueprintPure, Category = "RageInMageAbilitySystemLibrary|Input")
	static bool GetIconForKey(const FKey& InKey, const UDataTable* KeyIconTable, FKeyIconRow& OutRow);

	/** Convenience: get the icon texture for an InputTag in one call.
	 *  Combines GetKeyForInputTag + GetIconForKey. Returns null if not found. */
	UFUNCTION(BlueprintPure, Category = "RageInMageAbilitySystemLibrary|Input")
	static UTexture2D* GetIconTextureForInputTag(const FGameplayTag& InputTag, const URageInMageConfig* InputConfig, const UInputMappingContext* IMC, const UDataTable* KeyIconTable);
};