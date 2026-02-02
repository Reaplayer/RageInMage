// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * RageInMageGameplayTag
 *
 * Singleton containing native gameplay tags
 */
struct FRageInMageGameplayTag
{
public:
static const FRageInMageGameplayTag& Get() {return GameplayTag;}
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


	/* Mechanics Attributes */
	FGameplayTag Attributes_Mechanics_Heat;
	FGameplayTag Attributes_Mechanics_Momentum;
	FGameplayTag Attributes_Mechanics_ImmovableMass;
	FGameplayTag Attributes_Mechanics_Charge;
	FGameplayTag Attributes_Mechanics_Overgrowth;
	FGameplayTag Attributes_Mechanics_Crescendo;
	FGameplayTag Attributes_Mechanics_Obscurity;
	FGameplayTag Attributes_Mechanics_BlackOmen;
	FGameplayTag Attributes_Mechanics_Retribution;
	FGameplayTag Attributes_Mechanics_ConstantCirculation;
	FGameplayTag Attributes_Mechanics_LethalToxins;
	FGameplayTag Attributes_Mechanics_XP;


	/* Ability Tags */
	FGameplayTag Ability_Attack;
	FGameplayTag Ability_Attack_Melee;
	FGameplayTag Ability_Attack_Ranged;
	FGameplayTag Ability_Summon;
	
	
	/* Ability Type Tags */
	FGameplayTag AbilityType_Primary;
	FGameplayTag AbilityType_Secondary;
	FGameplayTag AbilityType_Movement;
	FGameplayTag AbilityType_Defensive;
	FGameplayTag AbilityType_CrowdControl;
	FGameplayTag AbilityType_Ultimate;
	FGameplayTag AbilityType_Passive1;
	FGameplayTag AbilityType_Passive2;
	FGameplayTag AbilityType_Passive3;
	
	/* Fire Mage Ability Tags */
	FGameplayTag Ability_Fire_SearingFlame;
	FGameplayTag Ability_Fire_ChanneledFire;
	FGameplayTag Ability_Fire_IgnitionLeap;
	FGameplayTag Ability_Fire_RisingFlameWave;
	FGameplayTag Ability_Fire_BlazingFlame;
	FGameplayTag Ability_Fire_FallOfTheScorchingSun;
	
	
	/* Water Mage Ability Tags */
	FGameplayTag Ability_Water_SimpleIce;
	FGameplayTag Ability_Water_SwirlingYoyo;
	FGameplayTag Ability_Water_Excelled;
	FGameplayTag Ability_Water_RidingTide;
	FGameplayTag Ability_Water_SoothingBubble;
	FGameplayTag Ability_Water_DropRippleBlast;
	FGameplayTag Ability_Water_TearsDoFall;
	
	
	/* Air Mage Ability Tags */
	FGameplayTag Ability_Air_SwiftGust;
	FGameplayTag Ability_Air_CatapultLaunch;
	FGameplayTag Ability_Air_CleanWindDash;
	FGameplayTag Ability_Air_MountainVortex;
	FGameplayTag Ability_Air_BlueSkyWhirlwind;
	FGameplayTag Ability_Air_DanceOfTheWindDragon;


	/* Earth Mage Ability Tags */
	FGameplayTag Ability_Earth_SlingRock;
	FGameplayTag Ability_Earth_JewelFistShatter;
	FGameplayTag Ability_Earth_ChargingBull;
	FGameplayTag Ability_Earth_RockSolid;
	FGameplayTag Ability_Earth_GemJail;
	FGameplayTag Ability_Earth_TheBoulder;
	FGameplayTag Ability_Earth_LandSlide;

	
	/* Lightning Mage Ability Tags */
	FGameplayTag Ability_Lightning_LightningFlash;
	FGameplayTag Ability_Lightning_SparkingSphere;
	FGameplayTag Ability_Lightning_ZipNZap;
	FGameplayTag Ability_Lightning_StaticMegaSurge;
	FGameplayTag Ability_Lightning_FlashAndAwe;
	FGameplayTag Ability_Lightning_CeaselessWrath;
	
	
	/* Nature Mage Ability Tags */
	FGameplayTag Ability_Nature_ThornSpread;
	FGameplayTag Ability_Nature_VenusFlyTrap;
	FGameplayTag Ability_Nature_CreepingVines;
	FGameplayTag Ability_Nature_SepalSurprise;
	FGameplayTag Ability_Nature_LassoOfThorns;
	FGameplayTag Ability_Nature_NaturesSpikyGrasp;
	
	
	/* Sound Mage Ability Tags */
	FGameplayTag Ability_Sound_Pleasc;
	FGameplayTag Ability_Sound_GunAnail;
	FGameplayTag Ability_Sound_DeanAite;
	FGameplayTag Ability_Sound_AmMall;
	FGameplayTag Ability_Sound_GunGhluasad;
	FGameplayTag Ability_Sound_TollahdFainne;
	
	
	/* Shadow Mage Ability Tags */
	FGameplayTag Ability_Shadow_DustBomb;
	FGameplayTag Ability_Shadow_JokersFacade;
	FGameplayTag Ability_Shadow_SmokeScreenShuffle;
	FGameplayTag Ability_Shadow_ShadowGuide;
	FGameplayTag Ability_Shadow_SteamRush;
	FGameplayTag Ability_Shadow_HiddenShadowTechnique;
	
	
	/* Necromancer Mage Ability Tags */
	FGameplayTag Ability_Necromancer_BubbleTrouble;
	FGameplayTag Ability_Necromancer_BitsNBones;
	FGameplayTag Ability_Necromancer_OldSkeller;
	FGameplayTag Ability_Necromancer_LivingBlockade;
	FGameplayTag Ability_Necromancer_WrithingGrasp;
	FGameplayTag Ability_Necromancer_RiseOfTheBlackCurse;
	
	
	/* Holy Mage Ability Tags */
	FGameplayTag Ability_Holy_BeamingBlade;
	FGameplayTag Ability_Holy_SacredSmite;
	FGameplayTag Ability_Holy_RadiantAscent;
	FGameplayTag Ability_Holy_DivineSanctuary;
	FGameplayTag Ability_Holy_ChainsOfAtonement;
	FGameplayTag Ability_Holy_AngelicReckoning;
	
	
	/* Life Mage Ability Tags*/
	FGameplayTag Ability_Life_EssenceTap;
	FGameplayTag Ability_Life_CircleOfLife;
	FGameplayTag Ability_Life_LuckyEscape;
	FGameplayTag Ability_Life_Elevate;
	FGameplayTag Ability_Life_Dispel;
	FGameplayTag Ability_Life_ImpenetrableBarrier;
	
	
	/* Fire Mage Cooldown Tags */
	FGameplayTag Cooldown_Fire_SearingFlame;
	FGameplayTag Cooldown_Fire_ChanneledFire;
	FGameplayTag Cooldown_Fire_IgnitionLeap;
	FGameplayTag Cooldown_Fire_RisingFlameWave;
	FGameplayTag Cooldown_Fire_BlazingFlame;
	FGameplayTag Cooldown_Fire_FallOfTheScorchingSun;
	
	
	/* Water Mage Cooldown Tags */
	FGameplayTag Cooldown_Water_SimpleIce;
	FGameplayTag Cooldown_Water_SwirlingYoyo;
	FGameplayTag Cooldown_Water_RidingTide;
	FGameplayTag Cooldown_Water_SoothingBubble;
	FGameplayTag Cooldown_Water_DropRippleBlast;
	FGameplayTag Cooldown_Water_TearsDoFall;
	
	
	/* Air Mage Cooldown Tags */
	FGameplayTag Cooldown_Air_SwiftGust;
	FGameplayTag Cooldown_Air_CatapultLaunch;
	FGameplayTag Cooldown_Air_CleanWindDash;
	FGameplayTag Cooldown_Air_MountainVortex;
	FGameplayTag Cooldown_Air_BlueSkyWhirlwind;
	FGameplayTag Cooldown_Air_DanceOfTheWindDragon;


	/* Earth Mage Cooldown Tags */
	FGameplayTag Cooldown_Earth_SlingRock;
	FGameplayTag Cooldown_Earth_JewelFistShatter;
	FGameplayTag Cooldown_Earth_ChargingBull;
	FGameplayTag Cooldown_Earth_RockSolid;
	FGameplayTag Cooldown_Earth_GemJail;
	FGameplayTag Cooldown_Earth_TheBoulder;

	
	/* Lightning Mage Cooldown Tags */
	FGameplayTag Cooldown_Lightning_LightningFlash;
	FGameplayTag Cooldown_Lightning_SparkingSphere;
	FGameplayTag Cooldown_Lightning_ZipNZap;
	FGameplayTag Cooldown_Lightning_StaticMegaSurge;
	FGameplayTag Cooldown_Lightning_FlashAndAwe;
	FGameplayTag Cooldown_Lightning_CeaselessWrath;
	
	
	/* Nature Mage Cooldown Tags */
	FGameplayTag Cooldown_Nature_ThornSpread;
	FGameplayTag Cooldown_Nature_VenusFlyTrap;
	FGameplayTag Cooldown_Nature_CreepingVines;
	FGameplayTag Cooldown_Nature_SepalSurprise;
	FGameplayTag Cooldown_Nature_LassoOfThorns;
	FGameplayTag Cooldown_Nature_NaturesSpikyGrasp;
	
	
	/* Sound Mage Cooldown Tags */
	FGameplayTag Cooldown_Sound_Pleasc;
	FGameplayTag Cooldown_Sound_GunAnail;
	FGameplayTag Cooldown_Sound_DeanAite;
	FGameplayTag Cooldown_Sound_AmMall;
	FGameplayTag Cooldown_Sound_GunGhluasad;
	FGameplayTag Cooldown_Sound_TollahdFainne;
	
	
	/* Shadow Mage Cooldown Tags */
	FGameplayTag Cooldown_Shadow_DustBomb;
	FGameplayTag Cooldown_Shadow_JokersFacade;
	FGameplayTag Cooldown_Shadow_SmokeScreenShuffle;
	FGameplayTag Cooldown_Shadow_ShadowGuide;
	FGameplayTag Cooldown_Shadow_SteamRush;
	FGameplayTag Cooldown_Shadow_HiddenShadowTechnique;
	
	
	/* Necromancer Mage Cooldown Tags */
	FGameplayTag Cooldown_Necromancer_BubbleTrouble;
	FGameplayTag Cooldown_Necromancer_BitsNBones;
	FGameplayTag Cooldown_Necromancer_OldSkeller;
	FGameplayTag Cooldown_Necromancer_LivingBlockade;
	FGameplayTag Cooldown_Necromancer_WrithingGrasp;
	FGameplayTag Cooldown_Necromancer_RiseOfTheBlackCurse;
	
	
	/* Holy Mage Cooldown Tags */
	FGameplayTag Cooldown_Holy_BeamingBlade;
	FGameplayTag Cooldown_Holy_SacredSmite;
	FGameplayTag Cooldown_Holy_RadiantAscent;
	FGameplayTag Cooldown_Holy_DivineSanctuary;
	FGameplayTag Cooldown_Holy_ChainsOfAtonement;
	FGameplayTag Cooldown_Holy_AngelicReckoning;
	
	
	/* Life Mage Cooldown Tags*/
	FGameplayTag Cooldown_Life_EssenceTap;
	FGameplayTag Cooldown_Life_CircleOfLife;
	FGameplayTag Cooldown_Life_LuckyEscape;
	FGameplayTag Cooldown_Life_Elevate;
	FGameplayTag Cooldown_Life_Dispel;
	FGameplayTag Cooldown_Life_ImpenetrableBarrier;
	
	
	/* M&K Input Tags */
	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_W;
	FGameplayTag InputTag_A;
	FGameplayTag InputTag_S;
	FGameplayTag InputTag_D;
	FGameplayTag InputTag_Q;
	FGameplayTag InputTag_E;
	FGameplayTag InputTag_Shift;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;
	
	
	/* Controller Input Tags */
	FGameplayTag InputTag_Controller_LeftStick;
	FGameplayTag InputTag_Controller_RightStick;
	FGameplayTag InputTag_Controller_LeftShoulder;
	FGameplayTag InputTag_Controller_RightShoulder;
	FGameplayTag InputTag_Controller_A;
	FGameplayTag InputTag_Controller_B;
	FGameplayTag InputTag_Controller_X;
	FGameplayTag InputTag_Controller_Y;
	FGameplayTag InputTag_Controller_LT;
	FGameplayTag InputTag_Controller_RT;
	FGameplayTag InputTag_Controller_Up;
	FGameplayTag InputTag_Controller_Down;
	FGameplayTag InputTag_Controller_Left;
	FGameplayTag InputTag_Controller_Right;
	FGameplayTag InputTag_Controller_Start;
	FGameplayTag InputTag_Controller_Back;
	
	
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

	
	/* Resistance Tags */
	FGameplayTag Resistance_PhysicalDamage;
	FGameplayTag Resistance_PhysicalDamage_Slashing;
	FGameplayTag Resistance_PhysicalDamage_Piercing;
	FGameplayTag Resistance_PhysicalDamage_Bludgeoning;
	FGameplayTag Resistance_MagicalDamage;
	FGameplayTag Resistance_MagicalDamage_Fire;
	FGameplayTag Resistance_MagicalDamage_Cold;
	FGameplayTag Resistance_MagicalDamage_Electric;
	FGameplayTag Resistance_MagicalDamage_Poison;
	FGameplayTag Resistance_MagicalDamage_Acid;
	FGameplayTag Resistance_MagicalDamage_Shadow;
	FGameplayTag Resistance_MagicalDamage_Radiant;
	FGameplayTag Resistance_MagicalDamage_Force;
	FGameplayTag Resistance_MagicalDamage_Psychic;

	TMap<FGameplayTag, FGameplayTag> DamageTypeToResistance;

	
	/* Effect Tags */
	FGameplayTag Effects_HitReaction;

	
	/* Crowd Control Effect Tags */
	FGameplayTag Effects_Debuff_Healing;
	FGameplayTag Effects_Debuff_Burning;
	FGameplayTag Effects_Debuff_Slowed;
	FGameplayTag Effects_Debuff_Frozen;
	FGameplayTag Effects_Debuff_Paralysed;
	FGameplayTag Effects_Debuff_Stunned;
	FGameplayTag Effects_Debuff_Rooted;
	FGameplayTag Effects_Debuff_Pushed;
	FGameplayTag Effects_Debuff_Invisible;
	FGameplayTag Effects_Debuff_Airborne;
	FGameplayTag Effects_Debuff_Untouchable;
	FGameplayTag Effects_Debuff_Immune;
	FGameplayTag Effects_Debuff_Petrified;
	FGameplayTag Effects_Debuff_Reflecting;
	FGameplayTag Effects_Debuff_Grappled;
	FGameplayTag Effects_Debuff_Silenced;
	FGameplayTag Effects_Debuff_SoulBound;
	FGameplayTag Effects_Debuff_Slammed;
	FGameplayTag Effects_Debuff_Constricted;
	FGameplayTag Effects_Debuff_Confused;
	FGameplayTag Effects_Debuff_Poisoned;
	FGameplayTag Effects_Debuff_Shocked;
	FGameplayTag Effects_Debuff_Charged;
	FGameplayTag Effects_Debuff_OverCharged;


	/* Combat Socket Tags */
	FGameplayTag CombatSocket_Weapon;
	FGameplayTag CombatSocket_LeftHand;
	FGameplayTag CombatSocket_RightHand;
	FGameplayTag CombatSocket_Tail;
	
	
	/* Montage Attack Tags */
	FGameplayTag Montage_Attack_1;
	FGameplayTag Montage_Attack_2;
	FGameplayTag Montage_Attack_3;
	FGameplayTag Montage_Attack_4;
	FGameplayTag Montage_Attack_5;
	FGameplayTag Montage_Attack_6;
	FGameplayTag Montage_Attack_7;
	FGameplayTag Montage_Attack_8;
	FGameplayTag Montage_Attack_9;
	FGameplayTag Montage_Attack_10;


	/* Team Tags */
	FGameplayTag Team_Red;
	FGameplayTag Team_Blue;
	FGameplayTag Team_Green;
	FGameplayTag Team_Yellow;
	FGameplayTag Team_Purple;
	FGameplayTag Team_Orange;
	FGameplayTag Team_Pink;
	FGameplayTag Team_Brown;
	FGameplayTag Team_Black;
	FGameplayTag Team_White;
	FGameplayTag Team_Cyan;

protected:

private:
	static FRageInMageGameplayTag GameplayTag;
};