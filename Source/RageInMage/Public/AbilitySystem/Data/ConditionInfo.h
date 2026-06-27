// Copyright Krymson Reaplays

#pragma once

#include "CoreMinimal.h"
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

	/* Stun Immunity */

	/** If > 0, this condition is treated as a stun: right after it's applied, UConditionInfo's shared
	 *  StunImmunityEffect is granted for (BaseIntensity + this) seconds — BaseIntensity being the
	 *  stun's own duration for threshold-driven conditions (e.g. OverCharged). Leave at 0 for any
	 *  condition that isn't a stun. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float StunImmunityGraceSeconds = 0.f;

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
