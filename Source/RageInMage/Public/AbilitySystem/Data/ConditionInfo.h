// Copyright Krymson Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "ConditionInfo.generated.h"

class UGameplayEffect;
class UNiagaraSystem;

UENUM(BlueprintType)
enum class EConditionCategory : uint8
{
	DamageOverTime,
	CrowdControl,
	MovementImpairment,
	Buff,
	Debuff,
	MechanicProc
};

UENUM(BlueprintType)
enum class EConditionStackBehavior : uint8
{
	None,
	StackCount,
	StackDuration,
	StackIntensity
};

USTRUCT(BlueprintType)
struct FRageInMageConditionInfo
{
	GENERATED_BODY()

	/* Identity */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ConditionTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ConditionName = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ConditionDescription = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EConditionCategory Category = EConditionCategory::Debuff;

	/* Trigger Source */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer TriggerDamageTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag MechanicsAttributeTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MechanicsThreshold = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bResetMechanicsOnTrigger = true;

	/* Effect */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> ConditionEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BaseDuration = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BaseIntensity = 10.f;

	/* Immunity — PER-CONDITION, so different CCs can be chained onto one target (stun -> petrify ->
	 * freeze) while the SAME condition can't simply be spammed. */

	/** If > 0, this condition grants itself an immunity window right after it lands, lasting
	 *  (BaseIntensity + this) seconds — BaseIntensity being the condition's own duration. Leave at 0
	 *  for any condition that shouldn't be immunity-protected. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float StunImmunityGraceSeconds = 0.f;

	/** GE granting THIS condition's own immunity tag. Its duration must read a SetByCaller keyed to
	 *  ImmunityTag. If left null, falls back to UConditionInfo's shared StunImmunityEffect
	 *  (Condition.StunImmune) — i.e. the old single-immunity behaviour. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> ImmunityEffect;

	/** The tag ImmunityEffect grants, and the SetByCaller key for its duration. Put this SAME tag in
	 *  this row's BlockedByConditions so the condition blocks only ITSELF — that is what allows a
	 *  different CC to still land on an already-CC'd target. Defaults to Condition.StunImmune when empty. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ImmunityTag = FGameplayTag();

	/* Per-character scaling — the values above are the shared BASE. Each field below optionally names an
	 * attribute (normally one of Attributes.Conditions.*) whose value is ADDED at apply time, and which
	 * character to read it from. Passives and items drive those attributes via GameplayEffects, so this
	 * DataAsset is never mutated. Leave an attribute unset for "no scaling".
	 *
	 * bFromTarget picks who is scaling the condition, and it is the difference between two opposite
	 * items: FALSE = the SOURCE ("my freezes last longer"), TRUE = the TARGET ("I stay frozen longer"). */

	/** Added to BaseIntensity (the condition's duration). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scaling")
	FGameplayAttribute DurationBonusAttribute;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scaling")
	bool bDurationBonusFromTarget = false;

	/** Added to StunImmunityGraceSeconds (the post-expiry immunity window). Usually read from the TARGET. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scaling")
	FGameplayAttribute ImmunityBonusAttribute;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scaling")
	bool bImmunityBonusFromTarget = true;

	/** Added to MaxStacks. Usually read from the SOURCE ("my fire stacks higher"). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scaling")
	FGameplayAttribute StackBonusAttribute;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scaling")
	bool bStackBonusFromTarget = false;

	/** Added to ExpirationDamageThreshold (damage absorbed before the condition breaks early). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scaling")
	FGameplayAttribute DamageThresholdBonusAttribute;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scaling")
	bool bDamageThresholdBonusFromTarget = true;

	/* Struggle — the "but can Struggle free" conditions (Petrified, Grappled) can be broken out of
	 * early by mashing an input, instead of just waiting the duration out. Handled by
	 * ARageInMageCharacterBase (BeginStruggle / AddStruggleProgress). */

	/** Enables mash-to-escape for this condition. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Struggle")
	bool bCanStruggleFree = false;

	/** Total progress needed to break out. Progress runs 0 -> this. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Struggle")
	float StruggleRequiredProgress = 100.f;

	/** Progress gained per input mash. Required/PerMash = how many presses an unhindered escape takes. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Struggle")
	float StruggleProgressPerMash = 10.f;

	/** Progress bled per second, so escaping requires mashing FAST rather than just eventually.
	 *  Keep below (PerMash x realistic mash rate) or escape becomes impossible. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Struggle")
	float StruggleProgressDecayPerSecond = 5.f;

	/* Stacking */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EConditionStackBehavior StackBehavior = EConditionStackBehavior::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxStacks = 1;

	/* Priority & Interactions */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Priority = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer OverridesConditions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer BlockedByConditions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer RemovesOnExpiry;

	/* Expiration Conditions */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ExpirationDamageThreshold = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer ExpirationOnCondition;

	/* Visuals */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> ConditionIcon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor ConditionColor = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> ConditionVFX = nullptr;
};

UCLASS()
class RAGEINMAGE_API UConditionInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Condition Info")
	TArray<FRageInMageConditionInfo> ConditionInfos;

	/** Maps heat stage number (-3 to -1, 1 to 3) to a Blueprint GE class.
	 *  Stages ±4 (Ignited/Frozen) remain in the ConditionInfo threshold system. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Heat Stages")
	TMap<int32, TSubclassOf<UGameplayEffect>> HeatStageEffects;

	/** Shared GE that grants Condition.Stunned. Used by any ability/condition that needs a generic,
	 *  unconditional stun application (e.g. Flash And Awe's ApplyGuaranteedStun). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stun")
	TSubclassOf<UGameplayEffect> StunnedEffect;

	/** Shared GE that grants Condition.StunImmune. Applied alongside every stun (see
	 *  FRageInMageConditionInfo::StunImmunityGraceSeconds and ApplyGuaranteedStun) so a target can't
	 *  be re-stunned until both the stun and its grace window have elapsed. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stun")
	TSubclassOf<UGameplayEffect> StunImmunityEffect;

	FRageInMageConditionInfo FindConditionInfoForTag(const FGameplayTag& ConditionTag, bool bLogNotFound = false) const;

	TArray<FRageInMageConditionInfo> GetConditionsByCategory(EConditionCategory Category) const;

	const FRageInMageConditionInfo* FindConditionForMechanicsThreshold(const FGameplayTag& MechanicsTag, float CurrentValue) const;
};
