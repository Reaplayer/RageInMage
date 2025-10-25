// Copyright Reaplays


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "RageInMageGameplayTags.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"

struct RageInMageDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalAttack);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicalAttack);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalDefence);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicalDefence);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalDefencePenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicalDefencePenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalDamage);
	RageInMageDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, PhysicalAttack, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, MagicalAttack, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, PhysicalDefence, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, MagicalDefence, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, PhysicalDefencePenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, MagicalDefencePenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, CriticalChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, CriticalDamage, Source, false);
	}
};

static const RageInMageDamageStatics& DamageStatics()
{
	static RageInMageDamageStatics DamageStatics;
	return DamageStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalAttackDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicalAttackDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalDefenceDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicalDefenceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalDefencePenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicalDefencePenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalDamageDef);
}


void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParams;
	EvaluationParams.SourceTags = SourceTags;
	EvaluationParams.TargetTags = TargetTags;

	// Search through DamageType Gameplay Tags
	float Damage = 0.f;
	for(FGameplayTag DamageTypeTag : FRageInMageGameplayTags::Get().DamageTypes)
		
	{
		//Check if Damage is Physical Damage
		if (DamageTypeTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("DamageType_PhysicalDamage"))))
	{
		// Get Damage Set by Caller Magnitude
		if (Spec.GetSetByCallerMagnitude(DamageTypeTag) == 0) { return; }
		float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag);

		// Get Attacker's Physical Attack
		float SourcePhysicalAttack = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().PhysicalAttackDef, EvaluationParams, SourcePhysicalAttack);
		SourcePhysicalAttack = FMath::Max<float>(SourcePhysicalAttack, 0.f);
		// Increase Damage by Physical Attack
		DamageTypeValue *= 1 + SourcePhysicalAttack / 100.f;

		// Get Attacker's Critical Chance
		float SourceCriticalChance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalChanceDef, EvaluationParams, SourceCriticalChance);
		SourceCriticalChance = FMath::Max<float>(SourceCriticalChance, 0.f);
		if (bool bCritHit =  FMath::RandRange(1, 100) <= SourceCriticalChance)
		{
			// Get Attacker's Critical Damage
			float SourceCriticalDamage = 0.f;
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalDamageDef, EvaluationParams, SourceCriticalDamage);
			// Increase Damage based on Critical Damage
			DamageTypeValue *= 1 + SourceCriticalDamage / 100.f;
		}

		// Get Target's Defence
		float TargetDefence = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().PhysicalDefenceDef, EvaluationParams, TargetDefence);
		TargetDefence = FMath::Max<float>(TargetDefence, 0.f);

		// Get Attacker's Defence Penetration
		float SourceDefencePenetration = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().PhysicalDefencePenetrationDef, EvaluationParams, SourceDefencePenetration);
		SourceDefencePenetration = FMath::Max<float>(SourceDefencePenetration, 0.f);
		// Reduce Damage based on Target's Effective Defence
		float EffectiveDefence = TargetDefence - SourceDefencePenetration;
		DamageTypeValue *= (1 - (EffectiveDefence / (EffectiveDefence + 100.f)));

		Damage += DamageTypeValue;	
	}

		// Check if Damage is Magical Damage
		if (DamageTypeTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("DamageType_MagicalDamage"))))
			{
				// Get Damage Set by Caller Magnitude
				if (Spec.GetSetByCallerMagnitude(DamageTypeTag) == 0) { return; }
				float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag);

				// Get Attacker's Magical Attack
				float SourceMagicalAttack = 0.f;
				ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().MagicalAttackDef, EvaluationParams, SourceMagicalAttack);
				SourceMagicalAttack = FMath::Max<float>(SourceMagicalAttack, 0.f);
				// Increase Damage by Magical Attack
				DamageTypeValue *= 1 + SourceMagicalAttack / 100.f;

				// Get Attacker's Critical Chance
				float SourceCriticalChance = 0.f;
				ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalChanceDef, EvaluationParams, SourceCriticalChance);
				SourceCriticalChance = FMath::Max<float>(SourceCriticalChance, 0.f);
				if (bool bCritHit =  FMath::RandRange(1, 100) <= SourceCriticalChance)
				{
					// Get Attacker's Critical Damage
					float SourceCriticalDamage = 0.f;
					ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalDamageDef, EvaluationParams, SourceCriticalDamage);
					// Increase Damage based on Critical Damage
					DamageTypeValue *= 1 + SourceCriticalDamage / 100.f;
				}

				// Get Target's Defence
				float TargetDefence = 0.f;
				ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().MagicalDefenceDef, EvaluationParams, TargetDefence);
				TargetDefence = FMath::Max<float>(TargetDefence, 0.f);

				// Get Attacker's Defence Penetration
				float SourceDefencePenetration = 0.f;
				ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().MagicalDefencePenetrationDef, EvaluationParams, SourceDefencePenetration);
				SourceDefencePenetration = FMath::Max<float>(SourceDefencePenetration, 0.f);
				// Reduce Damage based on Target's Effective Defence
				float EffectiveDefence = TargetDefence - SourceDefencePenetration;
				DamageTypeValue *= (1- (EffectiveDefence / (EffectiveDefence + 100.f)));

				Damage += DamageTypeValue;
			}
	}
}

