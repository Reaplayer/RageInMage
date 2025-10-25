// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * RageInMageGameplayTags
 *
 * Singleton containing native gameplay tags
 */
struct FRageInMageGameplayTags
{
public:
static const FRageInMageGameplayTags& Get() {return GameplayTags;}
	static void InitializeNativeGameplayTags();

	/* Primary Attributes */
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Dexterity;
	FGameplayTag Attributes_Primary_Agility;
	FGameplayTag Attributes_Primary_Wit;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Endurance;
	FGameplayTag Attributes_Primary_Vigor;

	/* Secondary Attributes */
	FGameplayTag Attributes_Secondary_PhysicalAttack;
	FGameplayTag Attributes_Secondary_MagicalAttack;
	FGameplayTag Attributes_Secondary_CriticalChance;
	FGameplayTag Attributes_Secondary_CriticalDamage;
	FGameplayTag Attributes_Secondary_AttackSpeed;
	FGameplayTag Attributes_Secondary_MovementSpeed;
	FGameplayTag Attributes_Secondary_PhysicalDefence;
	FGameplayTag Attributes_Secondary_MagicalDefence;
	FGameplayTag Attributes_Secondary_PhysicalDefencePenetration;
	FGameplayTag Attributes_Secondary_MagicalDefencePenetration;
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;

	/* Vital Attributes */
	FGameplayTag Attributes_Vital_Health;
	FGameplayTag Attributes_Vital_Mana;

	/* Item Attributes */
	FGameplayTag Attributes_Item_PhysicalDefencePenetrationPercentage;
	FGameplayTag Attributes_Item_MagicalDefencePenetrationPercentage;


	/* Input Tags */
	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;

	/* Damage Type Tags */
	FGameplayTag DamageType_PhysicalDamage;
	FGameplayTag DamageType_PhysicalDamage_Slashing;
	FGameplayTag DamageType_PhysicalDamage_Piercing;
	FGameplayTag DamageType_PhysicalDamage_Bludgeoning;
	FGameplayTag DamageType_MagicalDamage;
	FGameplayTag DamageType_MagicalDamage_Fire;
	FGameplayTag DamageType_MagicalDamage_Cold;
	FGameplayTag DamageType_MagicalDamage_Electric;
	FGameplayTag DamageType_MagicalDamage_Poison;
	FGameplayTag DamageType_MagicalDamage_Acid;
	FGameplayTag DamageType_MagicalDamage_Shadow;
	FGameplayTag DamageType_MagicalDamage_Radiant;
	FGameplayTag DamageType_MagicalDamage_Force;
	FGameplayTag DamageType_MagicalDamage_Psychic;

	TArray<FGameplayTag> DamageTypes;
	
	/* Effect Tags */
	FGameplayTag Effects_HitReaction;

	/* Crowd Control Effect Tags */
	FGameplayTag Effects_CrowdControl_Healing;
	FGameplayTag Effects_CrowdControl_Burning;
	FGameplayTag Effects_CrowdControl_Slowed;
	FGameplayTag Effects_CrowdControl_Frozen;
	FGameplayTag Effects_CrowdControl_Paralysed;
	FGameplayTag Effects_CrowdControl_Stunned;
	FGameplayTag Effects_CrowdControl_Rooted;
	FGameplayTag Effects_CrowdControl_Pushed;
	FGameplayTag Effects_CrowdControl_Invisible;
	FGameplayTag Effects_CrowdControl_Airborne;
	FGameplayTag Effects_CrowdControl_Untouchable;
	FGameplayTag Effects_CrowdControl_Immune;
	FGameplayTag Effects_CrowdControl_Petrified;
	FGameplayTag Effects_CrowdControl_Reflecting;
	FGameplayTag Effects_CrowdControl_Grappled;
	FGameplayTag Effects_CrowdControl_Silenced;
	FGameplayTag Effects_CrowdControl_SoulBound;
	FGameplayTag Effects_CrowdControl_Slammed;
	FGameplayTag Effects_CrowdControl_Constricted;
	FGameplayTag Effects_CrowdControl_Confused;
	FGameplayTag Effects_CrowdControl_Poisoned;
	

protected:

private:
	static FRageInMageGameplayTags GameplayTags;
};