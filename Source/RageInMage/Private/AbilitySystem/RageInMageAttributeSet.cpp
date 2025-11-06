// Copyright Reaplays


#include "AbilitySystem/RageInMageAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "RageInMageGameplayTags.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"
#include "Player/MagePlayerController.h"

URageInMageAttributeSet::URageInMageAttributeSet()
{
	// Populate Map of Gameplay Tags to their Attributes
	const FRageInMageGameplayTags& GameplayTags = FRageInMageGameplayTags::Get();
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
}

void URageInMageAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URageInMageAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
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
			const float NewHealth = GetHealth() - LocalDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
			if (NewHealth <= 0.f)
			{
				if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Properties.TargetAvatarActor))
				{
					CombatInterface->Die();
				}
			}
			else
			{
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FRageInMageGameplayTags::Get().Effects_HitReaction);
				Properties.TargetASC->TryActivateAbilitiesByTag(TagContainer);
			}
			const bool bCritHit = URageInMageAbilitySystemLibrary::IsCriticalHit(Properties.EffectContextHandle);
			const bool bVulnerableHit = URageInMageAbilitySystemLibrary::IsVulnerableHit(Properties.EffectContextHandle);
			const bool bResistantHit = URageInMageAbilitySystemLibrary::IsResistantHit(Properties.EffectContextHandle);
			ShowFloatingText(Properties, LocalDamage, bCritHit, bVulnerableHit, bResistantHit);
		}
	}
	// Set Heat Mechanic Change
	if (Data.EvaluatedData.Attribute == GetHeatAttribute())
	{
		const float LocalHeat = FMath::Clamp(GetHeat() + Data.EvaluatedData.Magnitude, -120.f, 120.f);
		SetHeat(0.f);
		if (LocalHeat >= 100.f)
		{
			
		}
		else if (LocalHeat < 0.f)
		{
			if (LocalHeat <= -100.f)
			{
				
			}
			
		}
	}
	// Set Charge Mechanic Change
	if (Data.EvaluatedData.Attribute == GetChargeAttribute())
	{
		const float LocalCharge = FMath::Clamp(GetCharge() + Data.EvaluatedData.Magnitude, 0.f, 120.f);
		SetCharge(0.f);
		if (LocalCharge >= 30.f)
		{
			if (LocalCharge >= 100.f)
			{
				
			}
			
		}
	}
}

void URageInMageAttributeSet::ShowFloatingText(const FEffectProperties& Properties, float Damage, bool bIsCriticalHit, bool bIsVulnerableHit, bool bIsResistantHit) const
{
	if (Properties.SourceCharacter != Properties.TargetCharacter)
	{
		if (AMagePlayerController* PC = Cast<AMagePlayerController>(Properties.SourceController))
		{
			PC->ShowDamageNumber(Damage, Properties.TargetCharacter, bIsCriticalHit, bIsVulnerableHit, bIsResistantHit);
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

void URageInMageAttributeSet::OnRep_PhysicalDefencePenetration(
	const FGameplayAttributeData& OldPhysicalDefencePenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, PhysicalDefencePenetration, OldPhysicalDefencePenetration);
}

void URageInMageAttributeSet::OnRep_MagicalDefencePenetration(
	const FGameplayAttributeData& OldMagicalDefencePenetration) const
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

void URageInMageAttributeSet::OnRep_PhysicalDefencePenetrationPercentage(
	const FGameplayAttributeData& OldPhysicalDefencePenetrationPercentage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URageInMageAttributeSet, PhysicalDefencePenetrationPercentage, OldPhysicalDefencePenetrationPercentage);
}

void URageInMageAttributeSet::OnRep_MagicalDefencePenetrationPercentage(
	const FGameplayAttributeData& OldMagicalDefencePenetrationPercentage) const
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