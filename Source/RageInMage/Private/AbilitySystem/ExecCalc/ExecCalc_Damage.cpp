// Copyright Reaplays


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "RageInMageAbilitySystemTypes.h"
#include "RageInMageGameplayTags.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "AbilitySystem/RageInMageAttributeSet.h"

struct RageInMageDamageStatics
{
	/* Declare Source Attributes */
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalAttack);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalDefencePenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalDefencePenetrationPercentage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicalAttack);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicalDefencePenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicalDefencePenetrationPercentage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalDamage);


	/* Declare Target Attributes */
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalDefence);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_PhysicalDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_PhysicalDamage_Slashing);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_PhysicalDamage_Piercing);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_PhysicalDamage_Bludgeoning);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicalDefence);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_MagicalDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_MagicalDamage_Fire);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_MagicalDamage_Cold);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_MagicalDamage_Electric);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_MagicalDamage_Poison);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_MagicalDamage_Acid);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_MagicalDamage_Shadow);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_MagicalDamage_Radiant);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_MagicalDamage_Psychic);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_MagicalDamage_Force);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Heat);



	// Map to store Resistance tag-to-capture definition mappings
	mutable TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> ResistanceTagsToCaptureDefs;
	
	
	
	RageInMageDamageStatics()
	{
		/* Capture Source Attributes */
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, PhysicalAttack, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, PhysicalDefencePenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, PhysicalDefencePenetrationPercentage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, MagicalAttack, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, MagicalDefencePenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, MagicalDefencePenetrationPercentage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, CriticalChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, CriticalDamage, Source, false);


		/* Capture Target Attributes */
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, PhysicalDefence, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, Resistance_PhysicalDamage, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, Resistance_PhysicalDamage_Slashing, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, Resistance_PhysicalDamage_Piercing, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, Resistance_PhysicalDamage_Bludgeoning, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, MagicalDefence, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, Resistance_MagicalDamage, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, Resistance_MagicalDamage_Fire, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, Resistance_MagicalDamage_Cold, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, Resistance_MagicalDamage_Electric, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, Resistance_MagicalDamage_Poison, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, Resistance_MagicalDamage_Acid, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, Resistance_MagicalDamage_Shadow, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, Resistance_MagicalDamage_Radiant, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, Resistance_MagicalDamage_Psychic, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URageInMageAttributeSet, Resistance_MagicalDamage_Force, Target, false);


		// Populate the map initially
		PopulateResistanceTagsToCaptureDefs();
	}


	// Helper function to populate the TagsToCaptureDefs map
	void PopulateResistanceTagsToCaptureDefs() const
	{
		// Clear the existing map
		ResistanceTagsToCaptureDefs.Empty();
		// Get Gameplay Tags
		const FRageInMageGameplayTags& GameplayTags = FRageInMageGameplayTags::Get();
		// Populate ResistanceTagsToCaptureDefs
		ResistanceTagsToCaptureDefs.Add(GameplayTags.Resistance_PhysicalDamage, Resistance_PhysicalDamageDef);
		ResistanceTagsToCaptureDefs.Add(GameplayTags.Resistance_PhysicalDamage_Slashing, Resistance_PhysicalDamage_SlashingDef);
		ResistanceTagsToCaptureDefs.Add(GameplayTags.Resistance_PhysicalDamage_Piercing, Resistance_PhysicalDamage_PiercingDef);
		ResistanceTagsToCaptureDefs.Add(GameplayTags.Resistance_PhysicalDamage_Bludgeoning, Resistance_PhysicalDamage_BludgeoningDef);
		ResistanceTagsToCaptureDefs.Add(GameplayTags.Resistance_MagicalDamage, Resistance_MagicalDamageDef);
		ResistanceTagsToCaptureDefs.Add(GameplayTags.Resistance_MagicalDamage_Fire, Resistance_MagicalDamage_FireDef);
		ResistanceTagsToCaptureDefs.Add(GameplayTags.Resistance_MagicalDamage_Cold, Resistance_MagicalDamage_ColdDef);
		ResistanceTagsToCaptureDefs.Add(GameplayTags.Resistance_MagicalDamage_Electric, Resistance_MagicalDamage_ElectricDef);
		ResistanceTagsToCaptureDefs.Add(GameplayTags.Resistance_MagicalDamage_Poison, Resistance_MagicalDamage_PoisonDef);
		ResistanceTagsToCaptureDefs.Add(GameplayTags.Resistance_MagicalDamage_Acid, Resistance_MagicalDamage_AcidDef);
		ResistanceTagsToCaptureDefs.Add(GameplayTags.Resistance_MagicalDamage_Shadow, Resistance_MagicalDamage_ShadowDef);
		ResistanceTagsToCaptureDefs.Add(GameplayTags.Resistance_MagicalDamage_Radiant, Resistance_MagicalDamage_RadiantDef);
		ResistanceTagsToCaptureDefs.Add(GameplayTags.Resistance_MagicalDamage_Psychic, Resistance_MagicalDamage_PsychicDef);
		ResistanceTagsToCaptureDefs.Add(GameplayTags.Resistance_MagicalDamage_Force, Resistance_MagicalDamage_ForceDef);
	}

	
	// Helper function to get the map with validation
	const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& GetResistanceTagsToCaptureDefs() const
	{
		// Check if any key in the map is invalid
		bool bRepopulate = false;
		for (const TPair<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& Pair : ResistanceTagsToCaptureDefs)
		{
			if (Pair.Key.IsValid())
			{
				return ResistanceTagsToCaptureDefs;
			}
			bRepopulate = true;
			break;
		}

		// Repopulate if needed
		if (bRepopulate)
		{
			PopulateResistanceTagsToCaptureDefs();
		}

		return ResistanceTagsToCaptureDefs;
	}
};


static const RageInMageDamageStatics& DamageStatics()
{
	static RageInMageDamageStatics DamageStatics;
	return DamageStatics;
}


UExecCalc_Damage::UExecCalc_Damage()
{
	/* Add Source Attribute Definitions */
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalAttackDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicalAttackDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalDefencePenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalDefencePenetrationPercentageDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicalDefencePenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicalDefencePenetrationPercentageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalDamageDef);

	
	/* Add Target Attribute Definitions */
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalDefenceDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicalDefenceDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_PhysicalDamageDef);
	RelevantAttributesToCapture.Add((DamageStatics().Resistance_PhysicalDamage_SlashingDef));
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_PhysicalDamage_PiercingDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_PhysicalDamage_BludgeoningDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_MagicalDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_MagicalDamage_FireDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_MagicalDamage_ColdDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_MagicalDamage_ElectricDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_MagicalDamage_PoisonDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_MagicalDamage_AcidDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_MagicalDamage_ShadowDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_MagicalDamage_RadiantDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_MagicalDamage_PsychicDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_MagicalDamage_ForceDef);
}


void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	/* Get Ability System Components */
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	/* Get Avatars */
	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	/* Get Spec */
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	/* Get Tags */
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParams;
	EvaluationParams.SourceTags = SourceTags;
	EvaluationParams.TargetTags = TargetTags;
	
	if (!URageInMageAbilitySystemLibrary::IsFriendly(SourceAvatar, TargetAvatar))
	{
		// Search Through Source Tags for DamageType Tags
        	for (FGameplayTag Tag : *SourceTags)
        	{
        		// Search through DamageType Gameplay Tags
        		for(const TPair<FGameplayTag, FGameplayTag>& Pair : FRageInMageGameplayTags::Get().DamageTypesToResistances)
        		{
        			// Match Source DamageType Tag With DamageToResistance Tag
        			if (Pair.Key.MatchesTagExact(Tag))
        			{
        				//Check if Damage Type is Physical Damage
        				if (Pair.Key.MatchesTag(FGameplayTag::RequestGameplayTag(FName("DamageType.PhysicalDamage"))))
        				{
        					// Get Damage Set by Caller Magnitude
        					float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key);
        
        		 	
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
        			
        					if (FMath::RandRange(1, 100) <= SourceCriticalChance)
        					{
        						// Get Attacker's Critical Damage
        						float SourceCriticalDamage = 0.f;
        						ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalDamageDef, EvaluationParams, SourceCriticalDamage);
        						// Increase Damage based on Critical Damage
        						DamageTypeValue *= 1 + SourceCriticalDamage / 100.f;
        						SetCriticalHit(Spec, true);
        					}
        
        					// Check to see if Target is Resistant or Vulnerable to Physical Damage
        					float PhysicalResistance = 0.f;
        					ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Resistance_PhysicalDamageDef, EvaluationParams, PhysicalResistance);
        					if (PhysicalResistance != 0.f)
        					{
        						DamageTypeValue *= 1 - PhysicalResistance / 100.f;
        						// Set the Vulnerable or Resistant Hit variables for Show Damage Numbers
        						if (PhysicalResistance < 0.f)
        						{
        							SetVulnerableHit(Spec, true);
        						}
        						else
        						{
        							SetResistantHit(Spec, true);
        						}
        					}
        
        
        					// Capture Target's Resistance to DamageType
        					const FGameplayEffectAttributeCaptureDefinition* CaptureDef = nullptr;
        					for (const auto& CapturePair : DamageStatics().GetResistanceTagsToCaptureDefs())
        					{
        						if (CapturePair.Key.MatchesTagExact(Pair.Value))
        						{
        							CaptureDef = &CapturePair.Value;
        							break;
        						}
        					}
        					float DamageTypeResistanceValue = 0.f;
        					ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(*CaptureDef, EvaluationParams, DamageTypeResistanceValue);
        					// Check to see if Target is Resistant to DamageType
        					if (DamageTypeResistanceValue != 0.f)
        					{
        						DamageTypeResistanceValue = FMath::Clamp(DamageTypeResistanceValue, -100.f, 100.f);
        						DamageTypeValue *= 1 - DamageTypeResistanceValue / 100.f;
        						// Set the Vulnerable or Resistant Hit variables for Show Damage Numbers
        						if (DamageTypeResistanceValue < 0.f)
        						{
        							SetVulnerableHit(Spec, true);
        						}
        						else if (DamageTypeResistanceValue > 0.f)
        						{
        							SetResistantHit(Spec, true);
        						}
        					}
        			
        
        					// Get Target's Defence
        					float TargetDefence = 0.f;
        					ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().PhysicalDefenceDef, EvaluationParams, TargetDefence);
        					TargetDefence = FMath::Max<float>(TargetDefence, 0.f);
        					// Get Attacker's Defence Penetration Percentage
        					float SourceDefencePenetrationPercentage = 0.f;
        					ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().PhysicalDefencePenetrationPercentageDef, EvaluationParams, SourceDefencePenetrationPercentage);
        					SourceDefencePenetrationPercentage = FMath::Max<float>(SourceDefencePenetrationPercentage, 0.f);
        					TargetDefence *= (1 - SourceDefencePenetrationPercentage);
        					// Get Attacker's Defence Penetration
        					float SourceDefencePenetration = 0.f;
        					ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().PhysicalDefencePenetrationDef, EvaluationParams, SourceDefencePenetration);
        					SourceDefencePenetration = FMath::Max<float>(SourceDefencePenetration, 0.f);
        					// Reduce Damage based on Target's Effective Defence
        					float EffectiveDefence = TargetDefence - SourceDefencePenetration;
        					DamageTypeValue *= (1 - (EffectiveDefence / (EffectiveDefence + 100.f)));
        
        			
        					//Do Damage
        					FGameplayModifierEvaluatedData DamageData (URageInMageAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Override, DamageTypeValue);
        					OutExecutionOutput.AddOutputModifier(DamageData);	
        				}
        
        				// Check if Damage Type is Magical Damage
        				if (Pair.Key.MatchesTag(FGameplayTag::RequestGameplayTag(FName("DamageType.MagicalDamage"))))
        				{
        					// Get Damage Set by Caller Magnitude
        					float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key);
        					// Make Local Mechanics Variables
        					float HeatDamage = 0.f;
        					float ChargeDamage = 0.f;
        					
        					if (Pair.Key.MatchesTagExact(FRageInMageGameplayTags::Get().DamageType_MagicalDamage_Fire) ||
        						Pair.Key.MatchesTagExact(FRageInMageGameplayTags::Get().DamageType_MagicalDamage_Cold))	
        					{
        						HeatDamage = Spec.GetSetByCallerMagnitude(FRageInMageGameplayTags::Get().Attributes_Mechanics_Heat);
        					}
        					if (Pair.Key.MatchesTagExact(FRageInMageGameplayTags::Get().DamageType_MagicalDamage_Electric))
        					{
        						ChargeDamage = Spec.GetSetByCallerMagnitude(FRageInMageGameplayTags::Get().Attributes_Mechanics_Charge);
        					}
        
        			
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
        					// Check whether the Hit is Critical
        					if (FMath::RandRange(1, 100) <= SourceCriticalChance)
        					{
        						// Get Attacker's Critical Damage
        						float SourceCriticalDamage = 0.f;
        						ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalDamageDef, EvaluationParams, SourceCriticalDamage);
        						// Increase Damage based on Critical Damage
        						DamageTypeValue *= 1 + SourceCriticalDamage / 100.f;
        						// Set the Critical Hit variable for Show Damage Numbers
        						SetCriticalHit(Spec, true);
        						HeatDamage *= 1.5f;
        						ChargeDamage *= 1.5f;
        					}
        
        
        					// Capture Target's Resistance to Magical Damage
        					float MagicalResistance = 0.f;
        					ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Resistance_MagicalDamageDef, EvaluationParams, MagicalResistance);
        					// Check to see if Target is Resistant or Vulnerable to Magical Damage
        					if (MagicalResistance != 0.f)
        					{
        						DamageTypeValue *= 1 - MagicalResistance / 100.f;
        						if (DamageTypeValue < 0.f)
        						{
        							SetVulnerableHit(Spec, true);
        						}
        						else if (DamageTypeValue > 0.f)
        						{
        							SetResistantHit(Spec, true);
        						}
        					}
        
        
        					// Find Target's Resistance to DamageType
        					const FGameplayEffectAttributeCaptureDefinition* CaptureDef = nullptr;
        					for (const auto CapturePair : DamageStatics().GetResistanceTagsToCaptureDefs())
        					{
        						if (Pair.Value.MatchesTagExact(CapturePair.Key))
        						{
        							CaptureDef = &CapturePair.Value;
        							break;
        						}
        					}
        					// Capture Resistance Value
        					float DamageTypeResistanceValue = 0.f;
        					ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(*CaptureDef, EvaluationParams, DamageTypeResistanceValue);
        					// Check to see if Target is Resistant or Vulnerable to DamageType
        					if (DamageTypeResistanceValue != 0.f)
        					{
        						DamageTypeResistanceValue = FMath::Clamp(DamageTypeResistanceValue, -100.f, 100.f);
        						DamageTypeResistanceValue = DamageTypeResistanceValue / 100.f;
        						DamageTypeValue *= 1 - DamageTypeResistanceValue;
        						HeatDamage *= 1 - DamageTypeResistanceValue;
        						ChargeDamage *= 1 - DamageTypeResistanceValue;
        
        						// Set the Vulnerable or Resistant Hit variables for Show Damage Numbers
        						if (DamageTypeResistanceValue < 0.f)
        						{
        							SetVulnerableHit(Spec, true);
        						}
        						else if (DamageTypeResistanceValue > 0.f)
        						{
        							SetResistantHit(Spec, true);
        						}
        					}
        
        			
        					// Get Target's Defence
        					float TargetDefence = 0.f;
        					ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().MagicalDefenceDef, EvaluationParams, TargetDefence);
        					TargetDefence = FMath::Max<float>(TargetDefence, 0.f);
        					// Get Attacker's Defence Penetration Percentage
        					float SourceDefencePenetrationPercentage = 0.f;
        					ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().MagicalDefencePenetrationPercentageDef, EvaluationParams, SourceDefencePenetrationPercentage);
        					SourceDefencePenetrationPercentage = FMath::Clamp<float>(SourceDefencePenetrationPercentage, 0.f, 100.f);
        					TargetDefence *= (1 - SourceDefencePenetrationPercentage / 100.f);
        					// Get Attacker's Defence Penetration
        					float SourceDefencePenetration = 0.f;
        					ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().MagicalDefencePenetrationDef, EvaluationParams, SourceDefencePenetration);
        					SourceDefencePenetration = FMath::Max<float>(SourceDefencePenetration, 0.f);
        					// Reduce Damage based on Target's Effective Defence
        					float EffectiveDefence = TargetDefence - SourceDefencePenetration;
        					DamageTypeValue *= (1- (EffectiveDefence / (EffectiveDefence + 100.f)));
        
        
        					// Do Damage && Apply Heat && Charge
        					FGameplayModifierEvaluatedData DamageData (URageInMageAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, DamageTypeValue);
        					OutExecutionOutput.AddOutputModifier(DamageData);
        					FGameplayModifierEvaluatedData HeatData (URageInMageAttributeSet::GetHeatAttribute(), EGameplayModOp::Additive, HeatDamage);
        					OutExecutionOutput.AddOutputModifier(HeatData);
        					FGameplayModifierEvaluatedData ChargeData (URageInMageAttributeSet::GetChargeAttribute(), EGameplayModOp::Additive, ChargeDamage);
        					OutExecutionOutput.AddOutputModifier(ChargeData);
        				}
        			}
        		}
        	}
	}
	
}


void UExecCalc_Damage::SetCriticalHit(const FGameplayEffectSpec& Spec, bool bIsCritHit)
{
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	URageInMageAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bIsCritHit);
}


void UExecCalc_Damage::SetVulnerableHit(const FGameplayEffectSpec& Spec, bool bIsVulnerableHit)
{
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
URageInMageAbilitySystemLibrary::SetIsVulnerableHit(EffectContextHandle, bIsVulnerableHit);
}

void UExecCalc_Damage::SetResistantHit(const FGameplayEffectSpec& Spec, bool bIsResistantHit)
{
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	URageInMageAbilitySystemLibrary::SetIsResistantHit(EffectContextHandle, bIsResistantHit);
}