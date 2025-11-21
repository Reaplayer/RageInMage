// Copyright Reaplays


#include "RageInMageGameplayTag.h"
#include "GameplayTagsManager.h"

FRageInMageGameplayTag FRageInMageGameplayTag::GameplayTag;

void FRageInMageGameplayTag::InitializeNativeGameplayTags()
{
	/* Primary Attribute Tags */
	GameplayTag.Attributes_Primary_Agility = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Primary.Agility"), FString("Increases Attack Speed and Movement Speed."));
	
	GameplayTag.Attributes_Primary_Dexterity = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Primary.Dexterity"), FString("Increases Critical Chance and Critical Damage."));
	
	GameplayTag.Attributes_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Primary.Strength"), FString("Increases Physical Attack Damage."));
	
	GameplayTag.Attributes_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Primary.Intelligence"), FString("Increases Magical Attack Damage and Maximum Mana."));
	
	GameplayTag.Attributes_Primary_Endurance = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Primary.Endurance"), FString("Increases Physical and Magical Defence."));
	
	GameplayTag.Attributes_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Primary.Vigor"), FString("Increases Maximum Health."));
	
	GameplayTag.Attributes_Primary_Wit = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Primary.Wit"), FString("Increases Physical and Magical Defence Penetration."));

	
	/* Secondary Attribute Tags */
	GameplayTag.Attributes_Secondary_PhysicalAttack = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.PhysicalAttack"), FString("Increases how much damage your attacks do."));
	
	GameplayTag.Attributes_Secondary_MagicalAttack = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.MagicalAttack"), FString("Increases how much damage your magical attacks do."));
	
	GameplayTag.Attributes_Secondary_CriticalChance = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.CriticalChance"), FString("Increases the chance of performing a critical hit."));
	
	GameplayTag.Attributes_Secondary_CriticalDamage = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.CriticalDamage"), FString("Increases the damage of a critical hit."));
	
	GameplayTag.Attributes_Secondary_AttackSpeed = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.AttackSpeed"), FString("Increases how fast your attacks are."));
	
	GameplayTag.Attributes_Secondary_MovementSpeed = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.MovementSpeed"), FString("Increases how fast your character moves."));
	
	GameplayTag.Attributes_Secondary_PhysicalDefence = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.PhysicalDefence"), FString("Decreases the amount of physical damage you take."));
	
	GameplayTag.Attributes_Secondary_MagicalDefence = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.MagicalDefence"), FString("Decreases the amount of magical damage you take."));
	
	GameplayTag.Attributes_Secondary_PhysicalDefencePenetration = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.PhysicalDefencePenetration"), FString("Decreases the amount of Physical Armour your target has against your physical attacks."));
	
	GameplayTag.Attributes_Secondary_MagicalDefencePenetration = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.MagicalDefencePenetration"), FString("Decreases the amount of Magical Armour your target has against your magical attacks."));
	
	GameplayTag.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.MaxHealth"), FString("Maximum amount of damage you can take before death."));
	
	GameplayTag.Attributes_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.MaxMana"), FString("Maximum amount of an innate resource used to cast spells."));

	
	/* Vital Tags */
	GameplayTag.Attributes_Vital_Health = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Vital.Health"), FString("Current amount of damage you can take before death."));
	
	GameplayTag.Attributes_Vital_Mana = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Vital.Mana"), FString("Current amount of an innate resource used to cast spells."));

	
	/* Item Attribute Tags */
	GameplayTag.Attributes_Item_PhysicalDefencePenetrationPercentage = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Item.PhysicalDefencePenetrationPercentage"), FString("Decreases the amount of Physical Armour your target has against your physical attacks."));
	
	GameplayTag.Attributes_Item_MagicalDefencePenetrationPercentage = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Item.MagicalDefencePenetrationPercentage"), FString("Decreases the amount of Magical Armour your target has against your magical attacks."));


	/* Mechanics Tags */
	GameplayTag.Attributes_Mechanics_Heat = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Mechanics.Heat"), FString("How Hot/Cold a Character is for determining Fire/Ice Effects."));

	GameplayTag.Attributes_Mechanics_Charge = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Mechanics.Charge"), FString("How Charged a Character is for determining Lightning Effects."));


	/* Ability Tags */
	GameplayTag.Ability_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Attack"), FString("Tag for when Attacking."));
	GameplayTag.Ability_Attack_Melee = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Attack.Melee"), FString("Tag for when Melee Attacking."));
	GameplayTag.Ability_Attack_Ranged = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Attack.Ranged"), FString("Tag for when Ranged Attacking."));
	
	
	/* Input Tags */
	GameplayTag.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.LMB"), FString("Input Tag for Left Mouse Button."));
	
	GameplayTag.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.RMB"), FString("Input Tag for Right Mouse Button."));
	
	GameplayTag.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.1"), FString("Input Tag for  1 Key."));
	
	GameplayTag.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.2"), FString("Input Tag for 2 Key."));
	
	GameplayTag.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.3"), FString("Input Tag for 3 Key."));
	
	GameplayTag.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.4"), FString("Input Tag for 4 Key."));

	
	/* Damage Type Tags */
	GameplayTag.DamageType_PhysicalDamage = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.PhysicalDamage"), FString("Amount of Physical Damage Done."));
	
	GameplayTag.DamageType_PhysicalDamage_Slashing = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.PhysicalDamage.Slashing"), FString("Amount of Slashing Damage Done."));
	
	GameplayTag.DamageType_PhysicalDamage_Piercing = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.PhysicalDamage.Piercing"), FString("Amount of Piercing Damage Done."));
	
	GameplayTag.DamageType_PhysicalDamage_Bludgeoning = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.PhysicalDamage.Bludgeoning"), FString("Amount of Bludgeoning Damage Done."));
	
	GameplayTag.DamageType_MagicalDamage = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.MagicalDamage"), FString("Amount of Magical Damage Done."));
	
	GameplayTag.DamageType_MagicalDamage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.MagicalDamage.Fire"), FString("Amount of Fire Damage Done."));
	
	GameplayTag.DamageType_MagicalDamage_Cold = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.MagicalDamage.Cold"), FString("Amount of Cold Damage Done."));
	
	GameplayTag.DamageType_MagicalDamage_Electric = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.MagicalDamage.Electric"), FString("Amount of Electric Damage Done."));
	
	GameplayTag.DamageType_MagicalDamage_Poison = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.MagicalDamage.Poison"), FString("Amount of Poison Damage Done."));
	
	GameplayTag.DamageType_MagicalDamage_Acid = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.MagicalDamage.Acid"), FString("Amount of Acid Damage Done."));
	
	GameplayTag.DamageType_MagicalDamage_Shadow = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.MagicalDamage.Shadow"), FString("Amount of Shadow Damage Done."));
	
	GameplayTag.DamageType_MagicalDamage_Radiant = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.MagicalDamage.Radiant"), FString("Amount of Radiant Damage Done."));
	
	GameplayTag.DamageType_MagicalDamage_Force = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.MagicalDamage.Force"), FString("Amount of Force Damage Done."));
	
	GameplayTag.DamageType_MagicalDamage_Psychic = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.MagicalDamage.Psychic"), FString("Amount of Psychic Damage Done."));


	/* Resistance Tags */
	GameplayTag.Resistance_PhysicalDamage = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.PhysicalDamage"), FString("Amount of Physical Damage Done."));
	
	GameplayTag.Resistance_PhysicalDamage_Slashing = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.PhysicalDamage.Slashing"), FString("Amount of Slashing Damage Done."));
	
	GameplayTag.Resistance_PhysicalDamage_Piercing = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.PhysicalDamage.Piercing"), FString("Amount of Piercing Damage Done."));
	
	GameplayTag.Resistance_PhysicalDamage_Bludgeoning = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.PhysicalDamage.Bludgeoning"), FString("Amount of Bludgeoning Damage Done."));
	
	GameplayTag.Resistance_MagicalDamage = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.MagicalDamage"), FString("Amount of Magical Damage Done."));
	
	GameplayTag.Resistance_MagicalDamage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.MagicalDamage.Fire"), FString("Amount of Fire Damage Done."));
	
	GameplayTag.Resistance_MagicalDamage_Cold = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.MagicalDamage.Cold"), FString("Amount of Cold Damage Done."));
	
	GameplayTag.Resistance_MagicalDamage_Electric = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.MagicalDamage.Electric"), FString("Amount of Electric Damage Done."));
	
	GameplayTag.Resistance_MagicalDamage_Poison = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.MagicalDamage.Poison"), FString("Amount of Poison Damage Done."));
	
	GameplayTag.Resistance_MagicalDamage_Acid = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.MagicalDamage.Acid"), FString("Amount of Acid Damage Done."));
	
	GameplayTag.Resistance_MagicalDamage_Shadow = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.MagicalDamage.Shadow"), FString("Amount of Shadow Damage Done."));
	
	GameplayTag.Resistance_MagicalDamage_Radiant = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.MagicalDamage.Radiant"), FString("Amount of Radiant Damage Done."));
	
	GameplayTag.Resistance_MagicalDamage_Force = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.MagicalDamage.Force"), FString("Amount of Force Damage Done."));
	
	GameplayTag.Resistance_MagicalDamage_Psychic = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.MagicalDamage.Psychic"), FString("Amount of Psychic Damage Done."));


	/* Map of Damage Type Tags to Resistance Tags */
	GameplayTag.DamageTypeToResistance.Add(GameplayTag.DamageType_PhysicalDamage, GameplayTag.Resistance_PhysicalDamage);
	GameplayTag.DamageTypeToResistance.Add(GameplayTag.DamageType_PhysicalDamage_Slashing, GameplayTag.Resistance_PhysicalDamage_Slashing);
	GameplayTag.DamageTypeToResistance.Add(GameplayTag.DamageType_PhysicalDamage_Piercing, GameplayTag.Resistance_PhysicalDamage_Piercing);
	GameplayTag.DamageTypeToResistance.Add(GameplayTag.DamageType_PhysicalDamage_Bludgeoning, GameplayTag.Resistance_PhysicalDamage_Bludgeoning);
	GameplayTag.DamageTypeToResistance.Add(GameplayTag.DamageType_MagicalDamage, GameplayTag.Resistance_MagicalDamage);
	GameplayTag.DamageTypeToResistance.Add(GameplayTag.DamageType_MagicalDamage_Fire, GameplayTag.Resistance_MagicalDamage_Fire);
	GameplayTag.DamageTypeToResistance.Add(GameplayTag.DamageType_MagicalDamage_Cold, GameplayTag.Resistance_MagicalDamage_Cold);
	GameplayTag.DamageTypeToResistance.Add(GameplayTag.DamageType_MagicalDamage_Electric, GameplayTag.Resistance_MagicalDamage_Electric);
	GameplayTag.DamageTypeToResistance.Add(GameplayTag.DamageType_MagicalDamage_Poison, GameplayTag.Resistance_MagicalDamage_Poison);
	GameplayTag.DamageTypeToResistance.Add(GameplayTag.DamageType_MagicalDamage_Acid, GameplayTag.Resistance_MagicalDamage_Acid);
	GameplayTag.DamageTypeToResistance.Add(GameplayTag.DamageType_MagicalDamage_Shadow, GameplayTag.Resistance_MagicalDamage_Shadow);
	GameplayTag.DamageTypeToResistance.Add(GameplayTag.DamageType_MagicalDamage_Radiant, GameplayTag.Resistance_MagicalDamage_Radiant);
	GameplayTag.DamageTypeToResistance.Add(GameplayTag.DamageType_MagicalDamage_Force, GameplayTag.Resistance_MagicalDamage_Force);
	GameplayTag.DamageTypeToResistance.Add(GameplayTag.DamageType_MagicalDamage_Psychic , GameplayTag.Resistance_MagicalDamage_Psychic);
	
	
	/* Effect Tags */
	GameplayTag.Effects_HitReaction = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.HitReaction"), FString("Tag for when Hit Reacting."));

	
	/* Debuff Effect Tags */
	GameplayTag.Effects_Debuff_Airborne = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Airborne"), FString("Tag for when not touching the ground."));
	
	GameplayTag.Effects_Debuff_Burning = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Burning"), FString("Tag for when Burning, doing Damage Over Time."));
	
	GameplayTag.Effects_Debuff_Poisoned = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Poisoned"), FString("Tag for when Poisoned, doing Damage Over Time."));
	
	GameplayTag.Effects_Debuff_Healing = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Healing"), FString("Tag for when Healing, increasing Health Over Time."));
	
	GameplayTag.Effects_Debuff_Slowed = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Slowed"), FString("Tag for when Slowed, Slows Movement Speed."));
	
	GameplayTag.Effects_Debuff_Rooted = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Rooted"), FString("Tag for when Rooted, unable to Move."));
	
	GameplayTag.Effects_Debuff_Grappled = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Grappled"), FString("Tag for when Grappled, unable to Move or make any Action but can Struggle free."));
	
	GameplayTag.Effects_Debuff_Petrified = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Petrified"), FString("Tag for when Petrified, unable to Move or make any Action but can Struggle free."));
	
	GameplayTag.Effects_Debuff_Constricted = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Constricted"), FString("Tag for when Constricted, unable to Move or make any Action."));
	
	GameplayTag.Effects_Debuff_Frozen = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Frozen"), FString("Tag for when Frozen, unable to Move or make any Action."));
	
	GameplayTag.Effects_Debuff_Stunned = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Stunned"), FString("Tag for when Stunned, unable to Move or make any Action."));
	
	GameplayTag.Effects_Debuff_Paralysed = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Paralysed"), FString("Tag for when Paralysed, unable to Move or make any Action."));
	
	GameplayTag.Effects_Debuff_Pushed = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Pushed"), FString("Tag for when Pushed, Moved by Force."));
	
	GameplayTag.Effects_Debuff_Slammed = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Slammed"), FString("Tag for when Slammed, Hit another Actor or Wall with Force."));
	
	GameplayTag.Effects_Debuff_Confused = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Confused"), FString("Tag for when Confused, Inverting Player's Controls."));
	
	GameplayTag.Effects_Debuff_Reflecting = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Reflecting"), FString("Tag for when Reflecting, sends Projectiles back the way they came from."));
	
	GameplayTag.Effects_Debuff_SoulBound = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.SoulBound"), FString("Tag for when SoulBound, Tethered to the Soul by the Lich."));
	
	GameplayTag.Effects_Debuff_Invisible = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Invisible"), FString("Tag for when Invisible, can not be seen."));
	
	GameplayTag.Effects_Debuff_Silenced = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Silenced"), FString("Tag for when Silenced, can not use Abilities."));
	
	GameplayTag.Effects_Debuff_Untouchable = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Untouchable"), FString("Tag for when Untouchable, can not be Damaged by Abilities."));
	
	GameplayTag.Effects_Debuff_Immune = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Immune"), FString("Tag for when Immune, can not be Damaged by certain Type(s)."));

	GameplayTag.Effects_Debuff_Shocked = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Shocked"), FString("Tag for when Shocked, unable to Move or make any Action."));

	GameplayTag.Effects_Debuff_Charged = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.Charged"), FString("Tag for when Charged, able for Electric Spells to potentially chain to you."));

	GameplayTag.Effects_Debuff_OverCharged = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.Debuff.OverCharged"), FString("Tag for when OverCharged, able for Electric Spells to chain from you."));


	/* Combat Socket Tags */
	GameplayTag.CombatSocket_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("CombatSocket.Weapon"), FString("Tag for when attacking with Weapon."));
	
	GameplayTag.CombatSocket_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("CombatSocket.LeftHand"), FString("Tag for when attacking with Left Hand."));
	
	GameplayTag.CombatSocket_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("CombatSocket.RightHand"), FString("Tag for when attacking with Right Hand."));
	
	GameplayTag.CombatSocket_Tail = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("CombatSocket.Tail"), FString("Tag for when attacking with Tail."));
	
	
	/* Montage Attack Tags */
	GameplayTag.Montage_Attack_1 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Montage.Attack.1"), FString("Tag for Montage Attacks."));
	
	GameplayTag.Montage_Attack_2 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Montage.Attack.2"), FString("Tag for Montage Attacks."));
	
	GameplayTag.Montage_Attack_3 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Montage.Attack.3"), FString("Tag for Montage Attacks."));
	
	GameplayTag.Montage_Attack_4 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Montage.Attack.4"), FString("Tag for Montage Attacks."));
	
	GameplayTag.Montage_Attack_5 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Montage.Attack.5"), FString("Tag for Montage Attacks."));
	
	GameplayTag.Montage_Attack_6 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Montage.Attack.6"), FString("Tag for Montage Attacks."));
	
	GameplayTag.Montage_Attack_7 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Montage.Attack.7"), FString("Tag for Montage Attacks."));
	
	GameplayTag.Montage_Attack_8 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Montage.Attack.8"), FString("Tag for Montage Attacks."));
	
	GameplayTag.Montage_Attack_9 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Montage.Attack.9"), FString("Tag for Montage Attacks."));
	
	GameplayTag.Montage_Attack_10 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Montage.Attack.10"), FString("Tag for Montage Attacks."));
	
	
	/* Team Tags */
	GameplayTag.Team_Red = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Team.Red"), FString("Tag for Red Team"));
	
	GameplayTag.Team_Blue = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Team.Blue"), FString("Tag for Blue Team"));
	
	GameplayTag.Team_Green = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Team.Green"), FString("Tag for Green Team"));
	
	GameplayTag.Team_Yellow = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Team.Yellow"), FString("Tag for Yellow Team"));
	
	GameplayTag.Team_Purple = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Team.Purple"), FString("Tag for Purple Team"));
	
	GameplayTag.Team_Orange = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Team.Orange"), FString("Tag for Orange Team"));
	
	GameplayTag.Team_Pink = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Team.Pink"), FString("Tag for Pink Team"));
	
	GameplayTag.Team_Brown = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Team.Brown"), FString("Tag for Brown Team"));
	
	GameplayTag.Team_Black = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Team.Black"), FString("Tag for Black Team"));
	
	GameplayTag.Team_White = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Team.White"), FString("Tag for White Team"));
	
	GameplayTag.Team_Cyan = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Team.Cyan"), FString("Tag for Cyan Team"));
}
