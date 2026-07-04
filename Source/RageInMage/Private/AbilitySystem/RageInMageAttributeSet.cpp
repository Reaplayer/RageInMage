// Copyright Reaplays


#include "AbilitySystem/RageInMageAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "RageInMageGameplayTag.h"
#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "AbilitySystem/Data/ConditionInfo.h"
#include "Character/RageInMageCharacterBase.h"
#include "GameplayCueInterface.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Player/RageInMagePlayerController.h"
#include "RageInMage/RageInMageLogChannels.h"

URageInMageAttributeSet::URageInMageAttributeSet()
{
	
}

void URageInMageAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Primary Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Dexterity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Agility, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Wit, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Vigor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Endurance, COND_None, REPNOTIFY_Always);

	// Secondary Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, PhysicalAttack, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, MagicalAttack, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, CriticalChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, CriticalDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, AttackSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, PhysicalDefence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, MagicalDefence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, PhysicalDefencePenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, MagicalDefencePenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Poise, COND_None, REPNOTIFY_Always);

	// Vital Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Mana, COND_None, REPNOTIFY_Always);

	// Resistance Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Resistance_Damage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Resistance_PhysicalDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Resistance_PhysicalDamage_Slashing, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Resistance_PhysicalDamage_Piercing, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Resistance_PhysicalDamage_Bludgeoning, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Resistance_MagicalDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Resistance_MagicalDamage_Fire, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Resistance_MagicalDamage_Cold, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Resistance_MagicalDamage_Electric, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Resistance_MagicalDamage_Poison, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Resistance_MagicalDamage_Acid, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Resistance_MagicalDamage_Shadow, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Resistance_MagicalDamage_Radiant, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Resistance_MagicalDamage_Force, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Resistance_MagicalDamage_Psychic, COND_None, REPNOTIFY_Always);

	// Item Specific Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, PhysicalDefencePenetrationPercentage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, MagicalDefencePenetrationPercentage, COND_None, REPNOTIFY_Always);

	// Mechanics Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Heat, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Momentum, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, ImmovableMass, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Charge, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, OverGrowth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Crescendo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Obscurity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, BlackOmen, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Retribution, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, ConstantCirculation, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, LethalToxins, COND_None, REPNOTIFY_Always);
}

void URageInMageAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Make Sure Health cannot go below 0 or above MaxHealth
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	// Make Sure Mana cannot go below 0 or above MaxMana
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void URageInMageAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data,
	FEffectProperties& Properties) const
{
	// Source = causer of the effect, Target = Target of the effect (Owner of this AS)
	// Get Context Handle
	Properties.EffectContextHandle = Data.EffectSpec.GetContext();
	// Get Source Ability System Component
	Properties.SourceASC = Properties.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	
	if (IsValid(Properties.SourceASC) && Properties.SourceASC->AbilityActorInfo.IsValid() && Properties.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		// Get Source Properties
		Properties.SourceAvatarActor = Properties.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Properties.SourceController = Properties.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Properties.SourceController == nullptr && Properties.SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(Properties.SourceAvatarActor))
			{
				Properties.SourceController = Pawn->GetController();
			}
		}
		if (Properties.SourceController)
		{
			Properties.SourceCharacter = Cast<ACharacter>(Properties.SourceController->GetPawn());
		}
	}
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		// Get Target Properties
		Properties.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Properties.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();

		//Check whether this is an AI controller
		if (Properties.TargetController == nullptr && Properties.TargetAvatarActor != nullptr)
		{
			Properties.TargetCharacter = Cast<ACharacter>(Properties.TargetAvatarActor);
			Properties.TargetController = Properties.TargetCharacter->GetController();
		}
		else if (Properties.TargetController)
		{
			Properties.TargetCharacter = Cast<ACharacter>(Properties.TargetController->GetPawn());
		}
		Properties.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Properties.TargetAvatarActor);
	}
}

void URageInMageAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Properties;
	SetEffectProperties(Data, Properties);
	// Health Change
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	};
	// Mana Change
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	// Incoming Damage
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalDamage = GetIncomingDamage();
		SetIncomingDamage(0.f);
		if (LocalDamage > 0.f)
		{
			// Shield absorption: if target has Status.Shielded, absorb all damage and broadcast
			if (Properties.TargetASC && Properties.TargetASC->HasMatchingGameplayTag(
				FRageInMageGameplayTag::Get().Status_Shielded))
			{
				OnShieldAbsorbedDamage.Broadcast(LocalDamage);
				ShowFloatingText(Properties, LocalDamage, false, false, false);
				return;
			}

			// Reflect: target still takes the (possibly reduced) damage normally below, but also
			// zaps the attacker back. The reflecting ability (e.g. Static Megasurge) owns the actual
			// zap-back amount/effect and binds to this delegate only while its buff is active.
			if (Properties.TargetASC && Properties.SourceAvatarActor && Properties.SourceAvatarActor != Properties.TargetAvatarActor
				&& Properties.TargetASC->HasMatchingGameplayTag(FRageInMageGameplayTag::Get().Status_Reflecting))
			{
				OnDamageReflected.Broadcast(Properties.SourceAvatarActor);
			}

			const float NewHealth = GetHealth() - LocalDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
			if (NewHealth <= 0.f)
			{
				if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Properties.TargetAvatarActor))
				{
					CombatInterface->Die();
				}
				SendXPEvent(Properties);
			}
			else
			{
				const ARageInMageCharacterBase* TargetCharacter = Cast<ARageInMageCharacterBase>(Properties.TargetAvatarActor);
				if (!TargetCharacter || TargetCharacter->CanTriggerHitReaction())
				{
					FGameplayTagContainer TagContainer;
					TagContainer.AddTag(FRageInMageGameplayTag::Get().Effects_HitReaction);
					Properties.TargetASC->TryActivateAbilitiesByTag(TagContainer);
				}
			}
			const bool bCritHit = URageInMageAbilitySystemLibrary::IsCriticalHit(Properties.EffectContextHandle);
			const bool bVulnerableHit = URageInMageAbilitySystemLibrary::IsVulnerableHit(Properties.EffectContextHandle);
			const bool bResistantHit = URageInMageAbilitySystemLibrary::IsResistantHit(Properties.EffectContextHandle);
			ShowFloatingText(Properties, LocalDamage, bCritHit, bVulnerableHit, bResistantHit);
		}
	}
	// Mechanics Attribute Threshold Checks
	const FRageInMageGameplayTag& GameplayTags = FRageInMageGameplayTag::Get();

	if (Data.EvaluatedData.Attribute == GetHeatAttribute())
	{
		const float OldHeat = GetHeat() - Data.EvaluatedData.Magnitude;
		const float NewHeat = FMath::Clamp(GetHeat(), -120.f, 120.f);
		SetHeat(NewHeat);

		// DEBUG: show Heat on screen (remove after testing)
		// Commented out for now while testing the Frozen mechanic — re-enable later if needed.
		// if (GEngine)
		// {
		// 	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange,
		// 		FString::Printf(TEXT("Heat: %.1f (was %.1f)"), NewHeat, OldHeat));
		// }

		// Fire hit on already-burning target -> add ignite stack
		// Check Magnitude > 0 (heat was added) because when heat is clamped at 120,
		// OldHeat == NewHeat so the stage-change path in HandleHeatChange won't trigger
		if (Data.EvaluatedData.Magnitude > 0.f && Properties.TargetASC)
		{
			FGameplayTagContainer TargetTags;
			Properties.TargetASC->GetOwnedGameplayTags(TargetTags);
			if (TargetTags.HasTagExact(GameplayTags.Condition_Burning))
			{
				if (URageInMageAbilitySystemComponent* RageASC = Cast<URageInMageAbilitySystemComponent>(Properties.TargetASC))
				{
					RageASC->AddIgniteStack(RageASC->DefaultIgniteDPS, Properties.SourceAvatarActor);
				}
			}
		}

		HandleHeatChange(OldHeat, NewHeat, Properties);
	}
	if (Data.EvaluatedData.Attribute == GetChargeAttribute())
	{
		HandleMechanicsThreshold(GetChargeAttribute(), GameplayTags.Attributes_Mechanics_Charge, 0.f, 120.f, Data.EvaluatedData.Magnitude, Properties);
	}
	if (Data.EvaluatedData.Attribute == GetMomentumAttribute())
	{
		HandleMechanicsThreshold(GetMomentumAttribute(), GameplayTags.Attributes_Mechanics_Momentum, 0.f, 120.f, Data.EvaluatedData.Magnitude, Properties);
	}
	if (Data.EvaluatedData.Attribute == GetImmovableMassAttribute())
	{
		HandleMechanicsThreshold(GetImmovableMassAttribute(), GameplayTags.Attributes_Mechanics_ImmovableMass, 0.f, 120.f, Data.EvaluatedData.Magnitude, Properties);
	}
	if (Data.EvaluatedData.Attribute == GetOverGrowthAttribute())
	{
		HandleMechanicsThreshold(GetOverGrowthAttribute(), GameplayTags.Attributes_Mechanics_Overgrowth, 0.f, 120.f, Data.EvaluatedData.Magnitude, Properties);
	}
	if (Data.EvaluatedData.Attribute == GetCrescendoAttribute())
	{
		HandleMechanicsThreshold(GetCrescendoAttribute(), GameplayTags.Attributes_Mechanics_Crescendo, 0.f, 120.f, Data.EvaluatedData.Magnitude, Properties);
	}
	if (Data.EvaluatedData.Attribute == GetObscurityAttribute())
	{
		HandleMechanicsThreshold(GetObscurityAttribute(), GameplayTags.Attributes_Mechanics_Obscurity, 0.f, 120.f, Data.EvaluatedData.Magnitude, Properties);
	}
	if (Data.EvaluatedData.Attribute == GetBlackOmenAttribute())
	{
		HandleMechanicsThreshold(GetBlackOmenAttribute(), GameplayTags.Attributes_Mechanics_BlackOmen, 0.f, 120.f, Data.EvaluatedData.Magnitude, Properties);
	}
	if (Data.EvaluatedData.Attribute == GetRetributionAttribute())
	{
		HandleMechanicsThreshold(GetRetributionAttribute(), GameplayTags.Attributes_Mechanics_Retribution, 0.f, 120.f, Data.EvaluatedData.Magnitude, Properties);
	}
	if (Data.EvaluatedData.Attribute == GetConstantCirculationAttribute())
	{
		HandleMechanicsThreshold(GetConstantCirculationAttribute(), GameplayTags.Attributes_Mechanics_ConstantCirculation, 0.f, 120.f, Data.EvaluatedData.Magnitude, Properties);
	}
	if (Data.EvaluatedData.Attribute == GetLethalToxinsAttribute())
	{
		HandleMechanicsThreshold(GetLethalToxinsAttribute(), GameplayTags.Attributes_Mechanics_LethalToxins, 0.f, 120.f, Data.EvaluatedData.Magnitude, Properties);
	}
	// Set XP Change
	if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		const float LocalXP = GetIncomingXP();
		SetIncomingXP(0.f);
		
		//TODO: See if we should level up
		if (Properties.SourceCharacter->Implements<UPlayerInterface>())
		{
			const int32 CurrentLevel = ICombatInterface::Execute_GetCharacterLevel(Properties.SourceCharacter);
			const int32 CurrentXP = IPlayerInterface::Execute_GetXP(Properties.SourceCharacter);
			
			const int32 NewLevel = IPlayerInterface::Execute_FindLevelForXP(Properties.SourceCharacter, CurrentXP + LocalXP);
			const int32 NumLevelUpsToApply = NewLevel - CurrentLevel;
			if (NumLevelUpsToApply > 0)
			{
				const int32 AttributePointsReward = IPlayerInterface::Execute_GetAttributePointsReward(Properties.SourceCharacter, CurrentLevel);
				const int32 SpellPointsReward = IPlayerInterface::Execute_GetSpellPointsReward(Properties.SourceCharacter, CurrentLevel);
				IPlayerInterface::Execute_AddToPlayerLevel(Properties.SourceCharacter, NumLevelUpsToApply);
				IPlayerInterface::Execute_AddToAttributePoints(Properties.SourceCharacter, AttributePointsReward * NumLevelUpsToApply);
				IPlayerInterface::Execute_AddToSpellPoints(Properties.SourceCharacter, SpellPointsReward * NumLevelUpsToApply);
				
				bResetHealth = true;
				bResetMana = true;
				
				IPlayerInterface::Execute_LevelUp(Properties.SourceCharacter);
			}
			
			IPlayerInterface::Execute_AddToXP(Properties.SourceCharacter, LocalXP);
		}
	}
}

void URageInMageAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	if (Attribute == GetMaxHealthAttribute() && bResetHealth)
	{
		SetHealth(GetMaxHealth());
		bResetHealth = false;
	}
	
	if (Attribute == GetMaxManaAttribute() && bResetMana)
	{
		SetMana(GetMaxMana());
		bResetMana = false;
	}

	// Heat: manage decay timer and stage GEs on any Heat change (including decay ticks)
	if (Attribute == GetHeatAttribute())
	{
		URageInMageAbilitySystemComponent* ASC = Cast<URageInMageAbilitySystemComponent>(GetOwningAbilitySystemComponent());
		if (ASC)
		{
			// Start or stop Heat decay timer
			if (!FMath::IsNearlyZero(NewValue, 0.5f))
			{
				ASC->StartHeatDecay();
			}
			else
			{
				ASC->StopHeatDecay();
			}

			// Handle stage changes for the decay path only
			// GE-driven changes are handled in HandleHeatChange (which sets bHeatStageHandledByGEPath)
			const int32 OldStage = GetHeatStage(OldValue);
			const int32 NewStage = GetHeatStage(NewValue);
			if (OldStage != NewStage && !bHeatStageHandledByGEPath)
			{
				// Remove old intermediate stage GE
				if (FMath::Abs(OldStage) >= 1 && FMath::Abs(OldStage) <= 3)
				{
					RemoveHeatStageGE(OldStage);
				}

				// Exiting Frozen (stage -4) via decay = natural thaw = SHATTER
				if (OldStage == -4 && !bFireThaw)
				{
					const FRageInMageGameplayTag& Tags = FRageInMageGameplayTag::Get();
					FGameplayTagContainer FrozenTag;
					FrozenTag.AddTag(Tags.Condition_Frozen);
					ASC->RemoveActiveEffectsWithGrantedTags(FrozenTag);
					FGameplayTagContainer FrozenStageTag;
					FrozenStageTag.AddTag(Tags.HeatStage_Frozen);
					ASC->RemoveActiveEffectsWithGrantedTags(FrozenStageTag);

					ApplyShatterDamage();
				}

				if (OldStage == -4)
				{
					bFireThaw = false;
				}

				// Apply new intermediate stage GE
				if (FMath::Abs(NewStage) >= 1 && FMath::Abs(NewStage) <= 3)
				{
					ApplyHeatStageGE(NewStage);
				}
			}
			bHeatStageHandledByGEPath = false;
		}
	}

	// Charge: manage decay timer on any Charge change (including decay ticks).
	// The Charge >= 100 -> Stunned threshold is handled by HandleMechanicsThreshold
	// via PostGameplayEffectExecute (ConditionInfo-driven), not here.
	if (Attribute == GetChargeAttribute())
	{
		if (URageInMageAbilitySystemComponent* ASC = Cast<URageInMageAbilitySystemComponent>(GetOwningAbilitySystemComponent()))
		{
			if (!FMath::IsNearlyZero(NewValue, 0.5f))
			{
				ASC->StartChargeDecay();
			}
			else
			{
				ASC->StopChargeDecay();
			}
		}
	}
}

void URageInMageAttributeSet::InitialiseTagsToAttributes()
{
	// Populate Map of Gameplay Tags to their Attributes
	const FRageInMageGameplayTag& GameplayTags = FRageInMageGameplayTag::Get();
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Agility, GetAgilityAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Dexterity, GetDexterityAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor, GetVigorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Wit, GetWitAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Endurance, GetEnduranceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_AttackSpeed, GetAttackSpeedAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MovementSpeed, GetMovementSpeedAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalChance, GetCriticalChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalDamage, GetCriticalDamageAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_PhysicalAttack, GetPhysicalAttackAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_PhysicalDefence, GetPhysicalDefenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_PhysicalDefencePenetration, GetPhysicalDefencePenetrationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MagicalDefencePenetration, GetMagicalDefencePenetrationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MagicalDefence, GetMagicalDefenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MagicalAttack, GetMagicalAttackAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxMana, GetMaxManaAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Poise, GetPoiseAttribute);
}


void URageInMageAttributeSet::ShowFloatingText(const FEffectProperties& Properties, float Damage, bool bIsCriticalHit, bool bIsVulnerableHit, bool bIsResistantHit) const
{
	if (Properties.SourceCharacter != Properties.TargetCharacter)
	{
		if (ARageInMagePlayerController* PC = Cast<ARageInMagePlayerController>(Properties.SourceController))
		{
			PC->ShowDamageNumber(Damage, Properties.TargetCharacter, bIsCriticalHit, bIsVulnerableHit, bIsResistantHit);
			return;
		}
		if (ARageInMagePlayerController* PC = Cast<ARageInMagePlayerController>(Properties.TargetController))
		{
			PC->ShowDamageNumber(Damage, Properties.TargetCharacter, bIsCriticalHit, bIsVulnerableHit, bIsResistantHit);
		}
	}
}

void URageInMageAttributeSet::SendXPEvent(const FEffectProperties& Properties) const
{
	if (Properties.TargetCharacter->Implements<UCombatInterface>())
	{
		const int32 TargetLevel = ICombatInterface::Execute_GetCharacterLevel(Properties.TargetCharacter);
		const ECharacterClass TargetClass = ICombatInterface::Execute_GetCharacterClass(Properties.TargetCharacter);
		const int32 XPReward = URageInMageAbilitySystemLibrary::GetXPRewardForClassAndLevel(TargetClass, TargetLevel, Properties.TargetCharacter);
		
		const FRageInMageGameplayTag& GameplayTag = FRageInMageGameplayTag::Get();
		FGameplayEventData Payload;
		Payload.EventTag = GameplayTag.Attributes_Mechanics_XP;
		Payload.EventMagnitude = XPReward;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Properties.SourceCharacter, Payload.EventTag, Payload);
	}
}


void URageInMageAttributeSet::HandleMechanicsThreshold(
	const FGameplayAttribute& Attribute, const FGameplayTag& MechanicsTag,
	float MinClamp, float MaxClamp, float Magnitude, const FEffectProperties& Properties)
{
	const float CurrentValue = Attribute.GetNumericValue(this);
	float NewValue = FMath::Clamp(CurrentValue + Magnitude, MinClamp, MaxClamp);

	// Use the property setter to persist accumulated value
	Attribute.SetNumericValueChecked(NewValue, StaticCast<URageInMageAttributeSet*>(this));

	// Look up condition from ConditionInfo DataAsset
	UConditionInfo* ConditionInfoData = URageInMageAbilitySystemLibrary::GetConditionInfo(Properties.TargetAvatarActor);
	if (!ConditionInfoData) return;

	const FRageInMageConditionInfo* CondInfo = ConditionInfoData->FindConditionForMechanicsThreshold(MechanicsTag, NewValue);
	if (CondInfo)
	{
		ApplyConditionFromData(CondInfo, Properties);
		if (CondInfo->bResetMechanicsOnTrigger)
		{
			float ResetValue = 0.f;
			Attribute.SetNumericValueChecked(ResetValue, StaticCast<URageInMageAttributeSet*>(this));
		}
	}
}

void URageInMageAttributeSet::ApplyConditionFromData(const FRageInMageConditionInfo* CondInfo, const FEffectProperties& Properties)
{
	if (!CondInfo || !CondInfo->ConditionEffect || !Properties.TargetASC) return;

	// Check if blocked by existing conditions on target
	FGameplayTagContainer OwnedTags;
	Properties.TargetASC->GetOwnedGameplayTags(OwnedTags);
	if (CondInfo->BlockedByConditions.Num() > 0 && OwnedTags.HasAny(CondInfo->BlockedByConditions))
	{
		return;
	}

	// Remove conditions this one overrides
	if (CondInfo->OverridesConditions.Num() > 0)
	{
		Properties.TargetASC->RemoveActiveEffectsWithGrantedTags(CondInfo->OverridesConditions);
	}

	// Apply the condition GE — use Source ASC as the instigator
	UAbilitySystemComponent* Instigator = Properties.SourceASC ? Properties.SourceASC : Properties.TargetASC;
	FGameplayEffectContextHandle ContextHandle = Instigator->MakeEffectContext();
	ContextHandle.AddSourceObject(Properties.SourceAvatarActor);
	const FGameplayEffectSpecHandle SpecHandle = Instigator->MakeOutgoingSpec(CondInfo->ConditionEffect, 1, ContextHandle);

	if (SpecHandle.IsValid())
	{
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, CondInfo->ConditionTag, CondInfo->BaseIntensity);
		Properties.TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		// Treat this condition as a stun: grant immunity for its own duration plus the grace period,
		// starting now rather than on natural expiry, so a refreshed/extended stun can't outrun it.
		// Mirrors URageInMageAbilitySystemLibrary::ApplyConditionToTarget, which this threshold pipeline bypasses.
		if (CondInfo->StunImmunityGraceSeconds > 0.f)
		{
			URageInMageAbilitySystemLibrary::ApplyStunImmunity(
				Instigator, Properties.TargetASC, Properties.TargetAvatarActor,
				CondInfo->BaseIntensity + CondInfo->StunImmunityGraceSeconds);
		}
	}
}

int32 URageInMageAttributeSet::GetHeatStage(float HeatValue) const
{
	// Positive heat stages (fire)
	if (HeatValue >= 100.f) return 4;   // Ignited
	if (HeatValue >= 75.f) return 3;    // Hot 3
	if (HeatValue >= 50.f) return 2;    // Hot 2
	if (HeatValue >= 25.f) return 1;    // Hot 1
	// Negative heat stages (cold)
	if (HeatValue <= -100.f) return -4; // Frozen
	if (HeatValue <= -75.f) return -3;  // Cold 3
	if (HeatValue <= -50.f) return -2;  // Cold 2
	if (HeatValue <= -25.f) return -1;  // Cold 1
	return 0; // Neutral
}

FGameplayTag URageInMageAttributeSet::GetHeatStageTag(int32 Stage) const
{
	const FRageInMageGameplayTag& Tags = FRageInMageGameplayTag::Get();
	switch (Stage)
	{
	case -4: return Tags.HeatStage_Frozen;
	case -3: return Tags.HeatStage_Cold3;
	case -2: return Tags.HeatStage_Cold2;
	case -1: return Tags.HeatStage_Cold1;
	case 1:  return Tags.HeatStage_Hot1;
	case 2:  return Tags.HeatStage_Hot2;
	case 3:  return Tags.HeatStage_Hot3;
	case 4:  return Tags.HeatStage_Ignited;
	default: return FGameplayTag();
	}
}

void URageInMageAttributeSet::RemoveHeatStageEffect(int32 OldStage, const FEffectProperties& Properties)
{
	if (OldStage == 0 || !Properties.TargetASC) return;

	const FGameplayTag OldTag = GetHeatStageTag(OldStage);
	if (OldTag.IsValid())
	{
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(OldTag);
		Properties.TargetASC->RemoveActiveEffectsWithGrantedTags(TagContainer);
	}
}

void URageInMageAttributeSet::ApplyHeatStageEffect(int32 NewStage, const FEffectProperties& Properties)
{
	if (NewStage == 0 || !Properties.TargetASC) return;

	UConditionInfo* ConditionInfoData = URageInMageAbilitySystemLibrary::GetConditionInfo(Properties.TargetAvatarActor);
	if (!ConditionInfoData) return;

	// For stages ±1 to ±3: apply from HeatStageEffects TMap on ConditionInfo
	if (FMath::Abs(NewStage) >= 1 && FMath::Abs(NewStage) <= 3)
	{
		const TSubclassOf<UGameplayEffect>* GEClass = ConditionInfoData->HeatStageEffects.Find(NewStage);
		if (GEClass && *GEClass)
		{
			UAbilitySystemComponent* Instigator = Properties.SourceASC ? Properties.SourceASC : Properties.TargetASC;
			FGameplayEffectContextHandle Context = Instigator->MakeEffectContext();
			Context.AddSourceObject(Properties.SourceAvatarActor ? Properties.SourceAvatarActor : Properties.TargetAvatarActor);
			const FGameplayEffectSpecHandle Spec = Instigator->MakeOutgoingSpec(*GEClass, 1, Context);
			if (Spec.IsValid())
			{
				Properties.TargetASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
			}
		}
	}
	// For stages ±4 (Ignited/Frozen), use the ConditionInfo threshold system
	else if (FMath::Abs(NewStage) == 4)
	{
		const FRageInMageGameplayTag& Tags = FRageInMageGameplayTag::Get();
		const FRageInMageConditionInfo* CondInfo = ConditionInfoData->FindConditionForMechanicsThreshold(
			Tags.Attributes_Mechanics_Heat, NewStage > 0 ? 100.f : -100.f);
		if (CondInfo)
		{
			ApplyConditionFromData(CondInfo, Properties);
		}
	}
}

void URageInMageAttributeSet::ApplyHeatStageGE(int32 Stage)
{
	if (Stage == 0 || FMath::Abs(Stage) == 4) return;

	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (!ASC) return;

	UConditionInfo* ConditionInfoData = URageInMageAbilitySystemLibrary::GetConditionInfo(ASC->GetAvatarActor());
	if (!ConditionInfoData) return;

	const TSubclassOf<UGameplayEffect>* GEClass = ConditionInfoData->HeatStageEffects.Find(Stage);
	if (!GEClass || !*GEClass) return;

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddSourceObject(ASC->GetAvatarActor());
	const FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(*GEClass, 1, Context);
	if (Spec.IsValid())
	{
		ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	}
}

void URageInMageAttributeSet::RemoveHeatStageGE(int32 Stage)
{
	if (Stage == 0 || FMath::Abs(Stage) == 4) return;

	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (!ASC) return;

	const FGameplayTag StageTag = GetHeatStageTag(Stage);
	if (StageTag.IsValid())
	{
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(StageTag);
		ASC->RemoveActiveEffectsWithGrantedTags(TagContainer);
	}
}

void URageInMageAttributeSet::ApplyShatterDamage()
{
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (!ASC) return;

	const float MaxHP = GetMaxHealth();
	const float BonusDamage = MaxHP * 0.2f;
	const float CurrentHealth = GetHealth();
	const float ResultHealth = FMath::Max(CurrentHealth - BonusDamage, 0.f);
	SetHealth(ResultHealth);

	// Show floating damage text
	if (AActor* AvatarActor = ASC->GetAvatarActor())
	{
		FEffectProperties ShatterProps;
		ShatterProps.TargetASC = ASC;
		ShatterProps.TargetAvatarActor = AvatarActor;
		ShatterProps.TargetCharacter = Cast<ACharacter>(AvatarActor);
		if (ShatterProps.TargetCharacter)
		{
			ShatterProps.TargetController = ShatterProps.TargetCharacter->GetController();
		}
		// SourceCharacter left nullptr so ShowFloatingText's Source != Target check passes
		ShatterProps.SourceASC = ASC;
		ShatterProps.SourceAvatarActor = AvatarActor;
		ShatterProps.SourceController = ShatterProps.TargetController;

		ShowFloatingText(ShatterProps, BonusDamage, false, false, false);
	}

	if (ResultHealth <= 0.f)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(ASC->GetAvatarActor()))
		{
			CombatInterface->Die();
		}
	}
}

void URageInMageAttributeSet::HandleHeatChange(float OldHeat, float NewHeat, const FEffectProperties& Properties)
{
	if (!Properties.TargetASC) return;

	bHeatStageHandledByGEPath = true;

	const FRageInMageGameplayTag& Tags = FRageInMageGameplayTag::Get();
	const int32 OldStage = GetHeatStage(OldHeat);
	const int32 NewStage = GetHeatStage(NewHeat);
	const bool bHeatIncreased = NewHeat > OldHeat;
	const bool bHeatDecreased = NewHeat < OldHeat;

	// --- Thaw/Extinguish Logic ---
	FGameplayTagContainer OwnedTags;
	Properties.TargetASC->GetOwnedGameplayTags(OwnedTags);

	// Fire hit on Frozen target -> Gentle thaw (no shatter damage)
	if (bHeatIncreased && OwnedTags.HasTagExact(Tags.Condition_Frozen))
	{
		bFireThaw = true;

		FGameplayTagContainer FrozenTag;
		FrozenTag.AddTag(Tags.Condition_Frozen);
		Properties.TargetASC->RemoveActiveEffectsWithGrantedTags(FrozenTag);

		// Also remove the Frozen stage tag
		FGameplayTagContainer FrozenStageTag;
		FrozenStageTag.AddTag(Tags.HeatStage_Frozen);
		Properties.TargetASC->RemoveActiveEffectsWithGrantedTags(FrozenStageTag);
		Properties.TargetASC->RemoveLooseGameplayTag(Tags.HeatStage_Frozen);
	}

	// Ice hit on Burning target -> Extinguish (remove Burning immediately + clear ignite stacks)
	if (bHeatDecreased && OwnedTags.HasTagExact(Tags.Condition_Burning))
	{
		FGameplayTagContainer BurningTag;
		BurningTag.AddTag(Tags.Condition_Burning);
		Properties.TargetASC->RemoveActiveEffectsWithGrantedTags(BurningTag);

		// Also remove the Ignited stage tag
		FGameplayTagContainer IgnitedStageTag;
		IgnitedStageTag.AddTag(Tags.HeatStage_Ignited);
		Properties.TargetASC->RemoveActiveEffectsWithGrantedTags(IgnitedStageTag);
		Properties.TargetASC->RemoveLooseGameplayTag(Tags.HeatStage_Ignited);

		// Clear all ignite stacks (extinguish)
		if (URageInMageAbilitySystemComponent* RageASC = Cast<URageInMageAbilitySystemComponent>(Properties.TargetASC))
		{
			RageASC->RemoveAllIgniteStacks();
		}
	}

	// --- Stage Change ---
	if (OldStage != NewStage)
	{
		// Freeze (stage -4) requires Ice damage — non-ice cold spells cap at stage -3
		int32 EffectiveNewStage = NewStage;
		if (NewStage == -4 && !URageInMageAbilitySystemLibrary::IsIceDamage(Properties.EffectContextHandle))
		{
			EffectiveNewStage = -3;
		}

		RemoveHeatStageEffect(OldStage, Properties);
		ApplyHeatStageEffect(EffectiveNewStage, Properties);

		// When heat crosses to Ignited (stage 4), add the first ignite stack
		if (EffectiveNewStage == 4 && OldStage < 4)
		{
			if (URageInMageAbilitySystemComponent* RageASC = Cast<URageInMageAbilitySystemComponent>(Properties.TargetASC))
			{
				RageASC->AddIgniteStack(RageASC->DefaultIgniteDPS, Properties.SourceAvatarActor);
			}
		}

		// Reset bFireThaw after leaving stage -4
		if (OldStage == -4)
		{
			bFireThaw = false;
		}
	}

}


void URageInMageAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Strength, OldStrength);
}

void URageInMageAttributeSet::OnRep_Dexterity(const FGameplayAttributeData& OldDexterity) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Dexterity, OldDexterity);
}

void URageInMageAttributeSet::OnRep_Agility(const FGameplayAttributeData& OldAgility) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Agility, OldAgility);
}

void URageInMageAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Intelligence, OldIntelligence);
}

void URageInMageAttributeSet::OnRep_Wit(const FGameplayAttributeData& OldWit) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Wit, OldWit);
}

void URageInMageAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Vigor, OldVigor);
}

void URageInMageAttributeSet::OnRep_Endurance(const FGameplayAttributeData& OldEndurance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Endurance, OldEndurance);
}

void URageInMageAttributeSet::OnRep_PhysicalAttack(const FGameplayAttributeData& OldPhysicalAttack) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, PhysicalAttack, OldPhysicalAttack);
}

void URageInMageAttributeSet::OnRep_MagicalAttack(const FGameplayAttributeData& OldMagicalAttack) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, MagicalAttack, OldMagicalAttack);
}

void URageInMageAttributeSet::OnRep_CriticalChance(const FGameplayAttributeData& OldCriticalChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, CriticalChance, OldCriticalChance);
}

void URageInMageAttributeSet::OnRep_CriticalDamage(const FGameplayAttributeData& OldCriticalDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, CriticalDamage, OldCriticalDamage);
}

void URageInMageAttributeSet::OnRep_AttackSpeed(const FGameplayAttributeData& OldAttackSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, AttackSpeed, OldAttackSpeed);
}

void URageInMageAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, MovementSpeed, OldMovementSpeed);
}

void URageInMageAttributeSet::OnRep_PhysicalDefence(const FGameplayAttributeData& OldPhysicalDefence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, PhysicalDefence, OldPhysicalDefence);
}

void URageInMageAttributeSet::OnRep_MagicalDefence(const FGameplayAttributeData& OldMagicalDefence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, MagicalDefence, OldMagicalDefence);
}

void URageInMageAttributeSet::OnRep_PhysicalDefencePenetration(const FGameplayAttributeData& OldPhysicalDefencePenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, PhysicalDefencePenetration, OldPhysicalDefencePenetration);
}

void URageInMageAttributeSet::OnRep_MagicalDefencePenetration(const FGameplayAttributeData& OldMagicalDefencePenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, MagicalDefencePenetration, OldMagicalDefencePenetration);
}

void URageInMageAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Health, OldHealth);
}

void URageInMageAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, MaxHealth, OldMaxHealth);
}

void URageInMageAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Mana, OldMana);
}

void URageInMageAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, MaxMana, OldMaxMana);
}

void URageInMageAttributeSet::OnRep_Poise(const FGameplayAttributeData& OldPoise) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Poise, OldPoise);
}

void URageInMageAttributeSet::OnRep_Resistance_Damage(const FGameplayAttributeData& OldResistance_Damage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Resistance_Damage, OldResistance_Damage);	
}

void URageInMageAttributeSet::OnRep_Resistance_PhysicalDamage(const FGameplayAttributeData& OldResistance_PhysicalDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Resistance_PhysicalDamage, OldResistance_PhysicalDamage);
}

void URageInMageAttributeSet::OnRep_Resistance_PhysicalDamage_Slashing(const FGameplayAttributeData& OldResistance_PhysicalDamage_Slashing) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Resistance_PhysicalDamage_Slashing, OldResistance_PhysicalDamage_Slashing);
}

void URageInMageAttributeSet::OnRep_Resistance_PhysicalDamage_Piercing(const FGameplayAttributeData& OldResistance_PhysicalDamage_Piercing) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Resistance_PhysicalDamage_Piercing, OldResistance_PhysicalDamage_Piercing);
}

void URageInMageAttributeSet::OnRep_Resistance_PhysicalDamage_Bludgeoning(const FGameplayAttributeData& OldResistance_PhysicalDamage_Bludgeoning) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Resistance_PhysicalDamage_Bludgeoning, OldResistance_PhysicalDamage_Bludgeoning);
}

void URageInMageAttributeSet::OnRep_Resistance_MagicalDamage(const FGameplayAttributeData& OldResistance_MagicalDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Resistance_MagicalDamage, OldResistance_MagicalDamage);
}

void URageInMageAttributeSet::OnRep_Resistance_MagicalDamage_Fire(const FGameplayAttributeData& OldResistance_MagicalDamage_Fire) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Resistance_MagicalDamage_Fire, OldResistance_MagicalDamage_Fire);
}

void URageInMageAttributeSet::OnRep_Resistance_MagicalDamage_Cold(const FGameplayAttributeData& OldResistance_MagicalDamage_Cold) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Resistance_MagicalDamage_Cold, OldResistance_MagicalDamage_Cold);
}

void URageInMageAttributeSet::OnRep_Resistance_MagicalDamage_Electric(const FGameplayAttributeData& OldResistance_MagicalDamage_Electric) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Resistance_MagicalDamage_Electric, OldResistance_MagicalDamage_Electric);
}

void URageInMageAttributeSet::OnRep_Resistance_MagicalDamage_Poison(const FGameplayAttributeData& OldResistance_MagicalDamage_Poison) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Resistance_MagicalDamage_Poison, OldResistance_MagicalDamage_Poison);
}

void URageInMageAttributeSet::OnRep_Resistance_MagicalDamage_Acid(const FGameplayAttributeData& OldResistance_MagicalDamage_Acid) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Resistance_MagicalDamage_Acid, OldResistance_MagicalDamage_Acid);
}

void URageInMageAttributeSet::OnRep_Resistance_MagicalDamage_Shadow(const FGameplayAttributeData& OldResistance_MagicalDamage_Shadow) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Resistance_MagicalDamage_Shadow, OldResistance_MagicalDamage_Shadow);
}

void URageInMageAttributeSet::OnRep_Resistance_MagicalDamage_Radiant(const FGameplayAttributeData& OldResistance_MagicalDamage_Radiant) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Resistance_MagicalDamage_Radiant, OldResistance_MagicalDamage_Radiant);
}

void URageInMageAttributeSet::OnRep_Resistance_MagicalDamage_Force(const FGameplayAttributeData& OldResistance_MagicalDamage_Force) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Resistance_MagicalDamage_Force, OldResistance_MagicalDamage_Force);
}

void URageInMageAttributeSet::OnRep_Resistance_MagicalDamage_Psychic(const FGameplayAttributeData& OldResistance_MagicalDamage_Psychic) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Resistance_MagicalDamage_Psychic, OldResistance_MagicalDamage_Psychic);
}

void URageInMageAttributeSet::OnRep_PhysicalDefencePenetrationPercentage(const FGameplayAttributeData& OldPhysicalDefencePenetrationPercentage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, PhysicalDefencePenetrationPercentage, OldPhysicalDefencePenetrationPercentage);
}

void URageInMageAttributeSet::OnRep_MagicalDefencePenetrationPercentage(const FGameplayAttributeData& OldMagicalDefencePenetrationPercentage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, MagicalDefencePenetrationPercentage, OldMagicalDefencePenetrationPercentage);
}

void URageInMageAttributeSet::OnRep_Heat(const FGameplayAttributeData& OldHeat) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Heat, OldHeat);
}

void URageInMageAttributeSet::OnRep_Charge(const FGameplayAttributeData& OldCharge) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Charge, OldCharge);
}

void URageInMageAttributeSet::OnRep_Momentum(const FGameplayAttributeData& OldMomentum) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Momentum, OldMomentum);
}

void URageInMageAttributeSet::OnRep_ImmovableMass(const FGameplayAttributeData& OldImmovableMass) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, ImmovableMass, OldImmovableMass);
}

void URageInMageAttributeSet::OnRep_OverGrowth(const FGameplayAttributeData& OldOverGrowth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, OverGrowth, OldOverGrowth);
}

void URageInMageAttributeSet::OnRep_Crescendo(const FGameplayAttributeData& OldCrescendo) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Crescendo, OldCrescendo);
}

void URageInMageAttributeSet::OnRep_Obscurity(const FGameplayAttributeData& OldObscurity) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Obscurity, OldObscurity);
}

void URageInMageAttributeSet::OnRep_BlackOmen(const FGameplayAttributeData& OldBlackOmen) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, BlackOmen, OldBlackOmen);
}

void URageInMageAttributeSet::OnRep_Retribution(const FGameplayAttributeData& OldRetribution) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, Retribution, OldRetribution);
}

void URageInMageAttributeSet::OnRep_ConstantCirculation(const FGameplayAttributeData& OldConstantCirculation) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, ConstantCirculation, OldConstantCirculation);
}

void URageInMageAttributeSet::OnRep_LethalToxins(const FGameplayAttributeData& OldLethalToxins) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, LethalToxins, OldLethalToxins);
}