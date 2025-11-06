// Copyright Reaplays


#include "RageInMageGameplayTags.h"
#include "GameplayTagsManager.h"

FRageInMageGameplayTags FRageInMageGameplayTags::GameplayTags;

void FRageInMageGameplayTags::InitializeNativeGameplayTags()
{
	/* Primary Attribute Tags */
	GameplayTags.Attributes_Primary_Agility = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Primary.Agility"), FString("Increases Attack Speed and Movement Speed."));
	
	GameplayTags.Attributes_Primary_Dexterity = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Primary.Dexterity"), FString("Increases Critical Chance and Critical Damage."));
	
	GameplayTags.Attributes_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Primary.Strength"), FString("Increases Physical Attack Damage."));
	
	GameplayTags.Attributes_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Primary.Intelligence"), FString("Increases Magical Attack Damage and Maximum Mana."));
	
	GameplayTags.Attributes_Primary_Endurance = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Primary.Endurance"), FString("Increases Physical and Magical Defence."));
	
	GameplayTags.Attributes_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Primary.Vigor"), FString("Increases Maximum Health."));
	
	GameplayTags.Attributes_Primary_Wit = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Primary.Wit"), FString("Increases Physical and Magical Defence Penetration."));

	
	/* Secondary Attribute Tags */
	GameplayTags.Attributes_Secondary_PhysicalAttack = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.PhysicalAttack"), FString("Increases how much damage your attacks do."));
	
	GameplayTags.Attributes_Secondary_MagicalAttack = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.MagicalAttack"), FString("Increases how much damage your magical attacks do."));
	
	GameplayTags.Attributes_Secondary_CriticalChance = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.CriticalChance"), FString("Increases the chance of performing a critical hit."));
	
	GameplayTags.Attributes_Secondary_CriticalDamage = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.CriticalDamage"), FString("Increases the damage of a critical hit."));
	
	GameplayTags.Attributes_Secondary_AttackSpeed = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.AttackSpeed"), FString("Increases how fast your attacks are."));
	
	GameplayTags.Attributes_Secondary_MovementSpeed = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.MovementSpeed"), FString("Increases how fast your character moves."));
	
	GameplayTags.Attributes_Secondary_PhysicalDefence = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.PhysicalDefence"), FString("Decreases the amount of physical damage you take."));
	
	GameplayTags.Attributes_Secondary_MagicalDefence = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.MagicalDefence"), FString("Decreases the amount of magical damage you take."));
	
	GameplayTags.Attributes_Secondary_PhysicalDefencePenetration = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.PhysicalDefencePenetration"), FString("Decreases the amount of Physical Armour your target has against your physical attacks."));
	
	GameplayTags.Attributes_Secondary_MagicalDefencePenetration = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.MagicalDefencePenetration"), FString("Decreases the amount of Magical Armour your target has against your magical attacks."));
	
	GameplayTags.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.MaxHealth"), FString("Maximum amount of damage you can take before death."));
	
	GameplayTags.Attributes_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.MaxMana"), FString("Maximum amount of an innate resource used to cast spells."));

	
	/* Vital Tags */
	GameplayTags.Attributes_Vital_Health = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Vital.Health"), FString("Current amount of damage you can take before death."));
	
	GameplayTags.Attributes_Vital_Mana = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Vital.Mana"), FString("Current amount of an innate resource used to cast spells."));

	
	/* Item Attribute Tags */
	GameplayTags.Attributes_Item_PhysicalDefencePenetrationPercentage = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Item.PhysicalDefencePenetrationPercentage"), FString("Decreases the amount of Physical Armour your target has against your physical attacks."));
	
	GameplayTags.Attributes_Item_MagicalDefencePenetrationPercentage = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Item.MagicalDefencePenetrationPercentage"), FString("Decreases the amount of Magical Armour your target has against your magical attacks."));


	/* Mechanics Tags */
	GameplayTags.Attributes_Mechanics_Heat = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Mechanics.Heat"), FString("How Hot/Cold a Character is for determining Fire/Ice Effects."));

	GameplayTags.Attributes_Mechanics_Charge = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Mechanics.Charge"), FString("How Charged a Character is for determining Lightning Effects."));


	/* Ability Tags */
	GameplayTags.Ability_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Attack"), FString("Tag for when Attacking."));
	
	
	/* Input Tags */
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.LMB"), FString("Input Tag for Left Mouse Button."));
	
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.RMB"), FString("Input Tag for Right Mouse Button."));
	
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.1"), FString("Input Tag for  1 Key."));
	
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.2"), FString("Input Tag for 2 Key."));
	
	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.3"), FString("Input Tag for 3 Key."));
	
	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.4"), FString("Input Tag for 4 Key."));

	
	/* Damage Type Tags */
	GameplayTags.DamageType_PhysicalDamage = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.PhysicalDamage"), FString("Amount of Physical Damage Done."));
	
	GameplayTags.DamageType_PhysicalDamage_Slashing = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.PhysicalDamage.Slashing"), FString("Amount of Slashing Damage Done."));
	
	GameplayTags.DamageType_PhysicalDamage_Piercing = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.PhysicalDamage.Piercing"), FString("Amount of Piercing Damage Done."));
	
	GameplayTags.DamageType_PhysicalDamage_Bludgeoning = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.PhysicalDamage.Bludgeoning"), FString("Amount of Bludgeoning Damage Done."));
	
	GameplayTags.DamageType_MagicalDamage = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.MagicalDamage"), FString("Amount of Magical Damage Done."));
	
	GameplayTags.DamageType_MagicalDamage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.MagicalDamage.Fire"), FString("Amount of Fire Damage Done."));
	
	GameplayTags.DamageType_MagicalDamage_Cold = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.MagicalDamage.Cold"), FString("Amount of Cold Damage Done."));
	
	GameplayTags.DamageType_MagicalDamage_Electric = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.MagicalDamage.Electric"), FString("Amount of Electric Damage Done."));
	
	GameplayTags.DamageType_MagicalDamage_Poison = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.MagicalDamage.Poison"), FString("Amount of Poison Damage Done."));
	
	GameplayTags.DamageType_MagicalDamage_Acid = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.MagicalDamage.Acid"), FString("Amount of Acid Damage Done."));
	
	GameplayTags.DamageType_MagicalDamage_Shadow = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.MagicalDamage.Shadow"), FString("Amount of Shadow Damage Done."));
	
	GameplayTags.DamageType_MagicalDamage_Radiant = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.MagicalDamage.Radiant"), FString("Amount of Radiant Damage Done."));
	
	GameplayTags.DamageType_MagicalDamage_Force = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.MagicalDamage.Force"), FString("Amount of Force Damage Done."));
	
	GameplayTags.DamageType_MagicalDamage_Psychic = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.MagicalDamage.Psychic"), FString("Amount of Psychic Damage Done."));


	/* Resistance Tags */
	GameplayTags.Resistance_PhysicalDamage = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.PhysicalDamage"), FString("Amount of Physical Damage Done."));
	
	GameplayTags.Resistance_PhysicalDamage_Slashing = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.PhysicalDamage.Slashing"), FString("Amount of Slashing Damage Done."));
	
	GameplayTags.Resistance_PhysicalDamage_Piercing = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.PhysicalDamage.Piercing"), FString("Amount of Piercing Damage Done."));
	
	GameplayTags.Resistance_PhysicalDamage_Bludgeoning = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.PhysicalDamage.Bludgeoning"), FString("Amount of Bludgeoning Damage Done."));
	
	GameplayTags.Resistance_MagicalDamage = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.MagicalDamage"), FString("Amount of Magical Damage Done."));
	
	GameplayTags.Resistance_MagicalDamage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.MagicalDamage.Fire"), FString("Amount of Fire Damage Done."));
	
	GameplayTags.Resistance_MagicalDamage_Cold = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.MagicalDamage.Cold"), FString("Amount of Cold Damage Done."));
	
	GameplayTags.Resistance_MagicalDamage_Electric = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.MagicalDamage.Electric"), FString("Amount of Electric Damage Done."));
	
	GameplayTags.Resistance_MagicalDamage_Poison = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.MagicalDamage.Poison"), FString("Amount of Poison Damage Done."));
	
	GameplayTags.Resistance_MagicalDamage_Acid = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.MagicalDamage.Acid"), FString("Amount of Acid Damage Done."));
	
	GameplayTags.Resistance_MagicalDamage_Shadow = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.MagicalDamage.Shadow"), FString("Amount of Shadow Damage Done."));
	
	GameplayTags.Resistance_MagicalDamage_Radiant = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.MagicalDamage.Radiant"), FString("Amount of Radiant Damage Done."));
	
	GameplayTags.Resistance_MagicalDamage_Force = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.MagicalDamage.Force"), FString("Amount of Force Damage Done."));
	
	GameplayTags.Resistance_MagicalDamage_Psychic = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.MagicalDamage.Psychic"), FString("Amount of Psychic Damage Done."));


	/* Map of Damage Types to Resistances */
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.DamageType_PhysicalDamage, GameplayTags.Resistance_PhysicalDamage);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.DamageType_PhysicalDamage_Slashing, GameplayTags.Resistance_PhysicalDamage_Slashing);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.DamageType_PhysicalDamage_Piercing, GameplayTags.Resistance_PhysicalDamage_Piercing);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.DamageType_PhysicalDamage_Bludgeoning, GameplayTags.Resistance_PhysicalDamage_Bludgeoning);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.DamageType_MagicalDamage, GameplayTags.Resistance_MagicalDamage);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.DamageType_MagicalDamage_Fire, GameplayTags.Resistance_MagicalDamage_Fire);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.DamageType_MagicalDamage_Cold, GameplayTags.Resistance_MagicalDamage_Cold);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.DamageType_MagicalDamage_Electric, GameplayTags.Resistance_MagicalDamage_Electric);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.DamageType_MagicalDamage_Poison, GameplayTags.Resistance_MagicalDamage_Poison);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.DamageType_MagicalDamage_Acid, GameplayTags.Resistance_MagicalDamage_Acid);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.DamageType_MagicalDamage_Shadow, GameplayTags.Resistance_MagicalDamage_Shadow);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.DamageType_MagicalDamage_Radiant, GameplayTags.Resistance_MagicalDamage_Radiant);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.DamageType_MagicalDamage_Force, GameplayTags.Resistance_MagicalDamage_Force);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.DamageType_MagicalDamage_Psychic , GameplayTags.Resistance_MagicalDamage_Psychic);
	
	
	/* Effect Tags */
	GameplayTags.Effects_HitReaction = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.HitReaction"), FString("Tag for when Hit Reacting."));

	
	/* Debuff Effect Tags */
	GameplayTags.Effects_Debuff_Airborne = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Airborne"), FString("Tag for when not touching the ground."));
	
	GameplayTags.Effects_Debuff_Burning = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Burning"), FString("Tag for when Burning, doing Damage Over Time."));
	
	GameplayTags.Effects_Debuff_Poisoned = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Poisoned"), FString("Tag for when Poisoned, doing Damage Over Time."));
	
	GameplayTags.Effects_Debuff_Healing = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Healing"), FString("Tag for when Healing, increasing Health Over Time."));
	
	GameplayTags.Effects_Debuff_Slowed = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Slowed"), FString("Tag for when Slowed, Slows Movement Speed."));
	
	GameplayTags.Effects_Debuff_Rooted = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Rooted"), FString("Tag for when Rooted, unable to Move."));
	
	GameplayTags.Effects_Debuff_Grappled = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Grappled"), FString("Tag for when Grappled, unable to Move or make any Action but can Struggle free."));
	
	GameplayTags.Effects_Debuff_Petrified = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Petrified"), FString("Tag for when Petrified, unable to Move or make any Action but can Struggle free."));
	
	GameplayTags.Effects_Debuff_Constricted = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Constricted"), FString("Tag for when Constricted, unable to Move or make any Action."));
	
	GameplayTags.Effects_Debuff_Frozen = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Frozen"), FString("Tag for when Frozen, unable to Move or make any Action."));
	
	GameplayTags.Effects_Debuff_Stunned = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Stunned"), FString("Tag for when Stunned, unable to Move or make any Action."));
	
	GameplayTags.Effects_Debuff_Paralysed = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Paralysed"), FString("Tag for when Paralysed, unable to Move or make any Action."));
	
	GameplayTags.Effects_Debuff_Pushed = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Pushed"), FString("Tag for when Pushed, Moved by Force."));
	
	GameplayTags.Effects_Debuff_Slammed = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Slammed"), FString("Tag for when Slammed, Hit another Actor or Wall with Force."));
	
	GameplayTags.Effects_Debuff_Confused = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Confused"), FString("Tag for when Confused, Inverting Player's Controls."));
	
	GameplayTags.Effects_Debuff_Reflecting = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Reflecting"), FString("Tag for when Reflecting, sends Projectiles back the way they came from."));
	
	GameplayTags.Effects_Debuff_SoulBound = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.SoulBound"), FString("Tag for when SoulBound, Tethered to the Soul by the Lich."));
	
	GameplayTags.Effects_Debuff_Invisible = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Invisible"), FString("Tag for when Invisible, can not be seen."));
	
	GameplayTags.Effects_Debuff_Silenced = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Silenced"), FString("Tag for when Silenced, can not use Abilities."));
	
	GameplayTags.Effects_Debuff_Untouchable = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Untouchable"), FString("Tag for when Untouchable, can not be Damaged by Abilities."));
	
	GameplayTags.Effects_Debuff_Immune = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Immune"), FString("Tag for when Immune, can not be Damaged by certain Type(s)."));

	GameplayTags.Effects_Debuff_Shocked = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Shocked"), FString("Tag for when Shocked, unable to Move or make any Action."));

	GameplayTags.Effects_Debuff_Charged = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Charged"), FString("Tag for when Charged, able for Electric Spells to potentially chain to you."));

	GameplayTags.Effects_Debuff_OverCharged = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.OverCharged"), FString("Tag for when OverCharged, able for Electric Spells to chain from you."));;
}
