// Copyright Reaplays


#include "RageInMageGameplayTag.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
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
	(FName("Attributes.Secondary.PhysicalDefencePenetration"), FString(
		"Decreases the amount of Physical Armour your target has against your physical attacks."));
	
	GameplayTag.Attributes_Secondary_MagicalDefencePenetration = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.MagicalDefencePenetration"), FString(
		"Decreases the amount of Magical Armour your target has against your magical attacks."));
	
	GameplayTag.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.MaxHealth"), FString("Maximum amount of damage you can take before death."));
	
	GameplayTag.Attributes_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.MaxMana"), FString("Maximum amount of an innate resource used to cast spells."));

	GameplayTag.Attributes_Secondary_Poise = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.Poise"), FString("Resistance to knockback and being shoved by other characters' collisions."));

	GameplayTag.Attributes_Secondary_CooldownReduction = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Secondary.CooldownReduction"), FString("Percentage reduction to the cooldown of every ability you cast."));


	/* Vital Tags */
	GameplayTag.Attributes_Vital_Health = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Vital.Health"), FString("Current amount of damage you can take before death."));
	
	GameplayTag.Attributes_Vital_Mana = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Vital.Mana"), FString("Current amount of an innate resource used to cast spells."));

	
	/* Item Attribute Tags */
	GameplayTag.Attributes_Item_PhysicalDefencePenetrationPercentage = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Item.PhysicalDefencePenetrationPercentage"), FString(
		"Decreases the amount of Physical Armour your target has against your physical attacks."));
	
	GameplayTag.Attributes_Item_MagicalDefencePenetrationPercentage = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Item.MagicalDefencePenetrationPercentage"), FString(
		"Decreases the amount of Magical Armour your target has against your magical attacks."));


	/* Mechanics Tags */
	GameplayTag.Attributes_Mechanics_Heat = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Mechanics.Heat"), FString("How Hot/Cold a Character is for determining Fire/Ice Effects."));
	
	GameplayTag.Attributes_Mechanics_Momentum = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Mechanics.Momentum"), FString("How much Momentum a Character has for determining Movement Effects."));
	
	GameplayTag.Attributes_Mechanics_ImmovableMass = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Mechanics.ImmovableMass"), FString("How Heavy a Character is for determining Earth Effects."));

	GameplayTag.Attributes_Mechanics_Charge = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Mechanics.Charge"), FString("How Charged a Character is for determining Lightning Effects."));
	
	GameplayTag.Attributes_Mechanics_Overgrowth = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Mechanics.Overgrowth"), FString("How much Overgrowth a Character has for determining Nature Effects."));
	
	GameplayTag.Attributes_Mechanics_Crescendo = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Mechanics.Crescendo"), FString("How much Crescendo a Character has for determining Sound Effects."));
	
	GameplayTag.Attributes_Mechanics_Obscurity = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Mechanics.Obscurity"), FString("How much Obscurity a Character has for determining Shadow Effects."));
	
	GameplayTag.Attributes_Mechanics_BlackOmen = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Mechanics.BlackOmen"), FString("How much Black Omen a Character has for determining Necromancer Effects."));
	
	GameplayTag.Attributes_Mechanics_Retribution = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Mechanics.Retribution"), FString("How much Retribution a Character has for determining Punishment Effects."));
	
	GameplayTag.Attributes_Mechanics_ConstantCirculation = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Mechanics.ConstantCirculation"), FString("How much Constant Circulation a Character has for determining Life Effects."));
	
	GameplayTag.Attributes_Mechanics_LethalToxins = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Mechanics.LethalToxins"), FString("How much Lethal Toxins a Character has for determining Poison Effects."));
	
	GameplayTag.Attributes_Mechanics_XP = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Mechanics.XP"), FString("How much XP a Character has for determining Leveling up."));

	/* Condition Attributes — per-character bonuses added to the shared ConditionInfo base values when a
	 * condition is applied. Passives and items modify these via GameplayEffects, so they replicate and
	 * revert automatically; the ConditionInfo DataAsset itself is never mutated. Each ConditionInfo row
	 * chooses which of these it reads, and whether to read it from the source or the target. */

	GameplayTag.Attributes_Conditions_DurationBonus = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Conditions.DurationBonus"), FString("Seconds added to an applied condition's duration."));

	GameplayTag.Attributes_Conditions_ImmunityBonus = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Conditions.ImmunityBonus"), FString("Seconds added to a condition's post-expiry immunity grace window."));

	GameplayTag.Attributes_Conditions_StackBonus = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Conditions.StackBonus"), FString("Extra stacks allowed for a stacking condition (e.g. ignite stacks)."));

	GameplayTag.Attributes_Conditions_DamageThresholdBonus = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Attributes.Conditions.DamageThresholdBonus"), FString("Extra damage a condition can absorb before it expires early."));


	/* Ability Tags */
	GameplayTag.Ability_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Attack"), FString("Tag for when Attacking."));
	
	GameplayTag.Ability_Attack_Melee = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Attack.Melee"), FString("Tag for when Melee Attacking."));
	
	GameplayTag.Ability_Attack_Ranged = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Attack.Ranged"), FString("Tag for when Ranged Attacking."));
	
	GameplayTag.Ability_Summon = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Summon"), FString("Tag for when Summoning."));
	
	
	/* Ability Progression Tags */
	GameplayTag.Ability_Progression_Locked = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Progression.Locked"), FString("Tag for when an Ability is Locked."));
	GameplayTag.Ability_Progression_Unlocked = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Progression.Unlocked"), FString("Tag for when an Ability is Unlocked."));
	GameplayTag.Ability_Progression_Unlockable = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Progression.Unlockable"), FString("Tag for when an Ability is Unlockable."));
	GameplayTag.Ability_Progression_Upgradable = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Progression.Upgradable"), FString("Tag for when an Ability is Upgradable."));
	
	/* Ability Type Tags */	
	GameplayTag.Ability_Type_Primary = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Type.Primary"), FString("Tag for when Activating a Primary Spell."));
	
	GameplayTag.Ability_Type_Secondary = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Type.Secondary"), FString("Tag for when Activating a Secondary Spell."));
	
	GameplayTag.Ability_Type_Movement = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Type.Movement"), FString("Tag for when Activating a Movement Spell."));
	
	GameplayTag.Ability_Type_Defensive = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Type.Defensive"), FString("Tag for when Activating a Defensive Spell."));
	
	GameplayTag.Ability_Type_CrowdControl = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Type.CrowdControl"), FString("Tag for when Activating a Crowd Control Spell."));
	
	GameplayTag.Ability_Type_Ultimate = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Type.Ultimate"), FString("Tag for when Activating an Ultimate Spell."));
	
	GameplayTag.Ability_Type_Passive1 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Type.Passive1"), FString("Tag for when Activating a Passive Spell."));
	
	GameplayTag.Ability_Type_Passive2 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Type.Passive2"), FString("Tag for when Activating a Passive Spell."));
	
	GameplayTag.Ability_Type_Passive3 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Type.Passive3"), FString("Tag for when Activating a Passive Spell."));
	
	/* Fire Mage Ability Tags */
	GameplayTag.Ability_Fire_SearingFlame = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Fire.SearingFlame"), FString("Tag for when casting Searing Flame."));
	
	GameplayTag.Ability_Fire_ChanneledFire = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Fire.ChanneledFire"), FString("Tag for when casting Channeled Fire."));
	
	GameplayTag.Ability_Fire_IgnitionLeap = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Fire.IgnitionLeap"), FString("Tag for when casting Ignition Leap."));
	
	GameplayTag.Ability_Fire_RisingFlameWave = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Fire.RisingFlameWave"), FString("Tag for when casting Rising Flame Wave."));	
	
	GameplayTag.Ability_Fire_BlazingFlame = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Fire.BlazingFlame"), FString("Tag for when casting Blazing Flame."));
	
	GameplayTag.Ability_Fire_FallOfTheScorchingSun = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Fire.FallOfTheScorchingSun"), FString("Tag for when casting Fall of the Scorching Sun."));
	
	
	/* Water Mage Ability Tags */
	GameplayTag.Ability_Water_SimpleIce = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Water.SimpleIce"), FString("Tag for when casting Simple Ice."));
	
	GameplayTag.Ability_Water_SwirlingYoyo = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Water.SwirlingYoyo"), FString("Tag for when casting Swirling Yoyo."));
	
	GameplayTag.Ability_Water_Excelled = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Water.Excelled"), FString("Tag for when casting Excelled."));
	
	GameplayTag.Ability_Water_RidingTide = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Water.RidingTide"), FString("Tag for when casting Riding Tide."));
	
	GameplayTag.Ability_Water_SoothingBubble = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Water.SoothingBubble"), FString("Tag for when casting Soothing Bubble."));
	
	GameplayTag.Ability_Water_DropRippleBlast = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Water.DropRippleBlast"), FString("Tag for when casting Drop Ripple Blast."));

	GameplayTag.Ability_Water_TearsDoFall = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Water.TearsDoFall"), FString("Tag for when casting Tears Do Fall."));
	
	
	/* Air Mage Ability Tags */
	GameplayTag.Ability_Air_SwiftGust = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Air.SwiftGust"), FString("Tag for when casting Swift Gust."));
	
	GameplayTag.Ability_Air_CatapultLaunch = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Air.CatapultLaunch"), FString("Tag for when casting Catapult Launch."));
	
	GameplayTag.Ability_Air_CleanWindDash = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Air.CleanWindDash"), FString("Tag for when casting Clean Wind Dash."));
	
	GameplayTag.Ability_Air_MountainVortex = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Air.MountainVortex"), FString("Tag for when casting Mountain Vortex."));
	
	GameplayTag.Ability_Air_BlueSkyWhirlwind = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Air.BlueSkyWhirlwind"), FString("Tag for when casting Blue Sky Whirlwind."));
	
	GameplayTag.Ability_Air_DanceOfTheWindDragon = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Air.DanceOfTheWindDragon"), FString("Tag for when casting Dance of the Wind Dragon."));
	
	
	/* Earth Mage Ability Tags */
	GameplayTag.Ability_Earth_SlingRock = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Earth.SlingRock"), FString("Tag for when casting Sling Rock."));

	GameplayTag.Ability_Earth_JaggedTerraForm = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Earth.JaggedTerraForm"), FString("Tag for when casting Jagged Terra Form."));

	GameplayTag.Ability_Earth_ChargingBull = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Earth.ChargingBull"), FString("Tag for when casting Charging Bull."));

	GameplayTag.Ability_Earth_RockSolid = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Earth.RockSolid"), FString("Tag for when casting Rock Solid."));

	GameplayTag.Ability_Earth_GemJail = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Earth.GemJail"), FString("Tag for when casting Gem Jail."));

	GameplayTag.Ability_Earth_TheBoulder = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Earth.TheBoulder"), FString("Tag for when casting The Boulder."));

	GameplayTag.Ability_Earth_LandSlide = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Earth.LandSlide"), FString("Tag for when casting Land Slide."));


	/* Lightning Mage Ability Tags */
	GameplayTag.Ability_Lightning_LightningFlash = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Lightning.LightningFlash"), FString("Tag for when casting Lightning Flash."));

	GameplayTag.Ability_Lightning_SparkingSphere = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Lightning.SparkingSphere"), FString("Tag for when casting Sparking Sphere."));

	GameplayTag.Ability_Lightning_ZipNZap = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Lightning.ZipNZap"), FString("Tag for when casting Zip N Zap."));

	GameplayTag.Ability_Lightning_StaticMegaSurge = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Lightning.StaticMegaSurge"), FString("Tag for when casting Static Mega Surge."));

	GameplayTag.Ability_Lightning_FlashAndAwe = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Lightning.FlashAndAwe"), FString("Tag for when casting Flash And Awe."));

	GameplayTag.Ability_Lightning_CeaselessWrath = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Lightning.CeaselessWrath"), FString("Tag for when casting Ceaseless Wrath."));


	/* Nature Mage Ability Tags */
	GameplayTag.Ability_Nature_ThornSpread = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Nature.ThornSpread"), FString("Tag for when casting Thorn Spread."));

	GameplayTag.Ability_Nature_VenusFlyTrap = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Nature.VenusFlyTrap"), FString("Tag for when casting Venus Fly Trap."));

	GameplayTag.Ability_Nature_CreepingVines = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Nature.CreepingVines"), FString("Tag for when casting Creeping Vines."));

	GameplayTag.Ability_Nature_SepalSurprise = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Nature.SepalSurprise"), FString("Tag for when casting Sepal Surprise."));

	GameplayTag.Ability_Nature_LassoOfThorns = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Nature.LassoOfThorns"), FString("Tag for when casting Lasso Of Thorns."));

	GameplayTag.Ability_Nature_NaturesSpikyGrasp = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Nature.NaturesSpikyGrasp"), FString("Tag for when casting Natures Spiky Grasp."));


	/* Sound Mage Ability Tags */
	GameplayTag.Ability_Sound_Pleasc = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Sound.Pleasc"), FString("Tag for when casting Pleasc."));

	GameplayTag.Ability_Sound_GunAnail = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Sound.GunAnail"), FString("Tag for when casting Gun Anail."));

	GameplayTag.Ability_Sound_DeanAite = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Sound.DeanAite"), FString("Tag for when casting Dean Aite."));

	GameplayTag.Ability_Sound_AmMall = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Sound.AmMall"), FString("Tag for when casting Am Mall."));

	GameplayTag.Ability_Sound_GunGhluasad = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Sound.GunGhluasad"), FString("Tag for when casting Gun Ghluasad."));

	GameplayTag.Ability_Sound_TollahdFainne = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Sound.TollahdFainne"), FString("Tag for when casting Tollahd Fainne."));


	/* Shadow Mage Ability Tags */
	GameplayTag.Ability_Shadow_DustBomb = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Shadow.DustBomb"), FString("Tag for when casting Dust Bomb."));

	GameplayTag.Ability_Shadow_JokersFacade = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Shadow.JokersFacade"), FString("Tag for when casting Jokers Facade."));

	GameplayTag.Ability_Shadow_SmokeScreenShuffle = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Shadow.SmokeScreenShuffle"), FString("Tag for when casting Smoke Screen Shuffle."));

	GameplayTag.Ability_Shadow_ShadowGuide = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Shadow.ShadowGuide"), FString("Tag for when casting Shadow Guide."));

	GameplayTag.Ability_Shadow_SteamRush = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Shadow.SteamRush"), FString("Tag for when casting Steam Rush."));

	GameplayTag.Ability_Shadow_HiddenShadowTechnique = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Shadow.HiddenShadowTechnique"), FString("Tag for when casting Hidden Shadow Technique."));


	/* Necromancer Mage Ability Tags */
	GameplayTag.Ability_Necromancer_BubbleTrouble = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Necromancer.BubbleTrouble"), FString("Tag for when casting Bubble Trouble."));

	GameplayTag.Ability_Necromancer_BitsNBones = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Necromancer.BitsNBones"), FString("Tag for when casting Bits 'n' Bones."));

	GameplayTag.Ability_Necromancer_OldSkeller = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Necromancer.OldSkeller"), FString("Tag for when casting Old Skeller."));

	GameplayTag.Ability_Necromancer_LivingBlockade = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Necromancer.LivingBlockade"), FString("Tag for when casting Living Blockade."));

	GameplayTag.Ability_Necromancer_WrithingGrasp = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Necromancer.WrithingGrasp"), FString("Tag for when casting Writhing Grasp."));

	GameplayTag.Ability_Necromancer_RiseOfTheBlackCurse = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Necromancer.RiseOfTheBlackCurse"), FString("Tag for when casting Rise Of The Black Curse."));


	/* Holy Mage Ability Tags */
	GameplayTag.Ability_Holy_BeamingBlade = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Holy.BeamingBlade"), FString("Tag for when casting Beaming Blade."));

	GameplayTag.Ability_Holy_SacredSmite = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Holy.SacredSmite"), FString("Tag for when casting Sacred Smite."));

	GameplayTag.Ability_Holy_RadiantAscent = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Holy.RadiantAscent"), FString("Tag for when casting Radiant Ascent."));

	GameplayTag.Ability_Holy_DivineSanctuary = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Holy.DivineSanctuary"), FString("Tag for when casting Divine Sanctuary."));

	GameplayTag.Ability_Holy_ChainsOfAtonement = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Holy.ChainsOfAtonement"), FString("Tag for when casting Chains of Atonement."));

	GameplayTag.Ability_Holy_AngelicReckoning = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Holy.AngelicReckoning"), FString("Tag for when casting Angelic Reckoning."));
	
	
	/* Life Mage Ability Tags */
	GameplayTag.Ability_Life_EssenceTap = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Life.EssenceTap"), FString("Tag for when casting Essence Tap."));
	
	GameplayTag.Ability_Life_CircleOfLife = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Life.CircleOfLife"), FString("Tag for when casting Circle of Life."));
	
	GameplayTag.Ability_Life_LuckyEscape = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Life.LuckyEscape"), FString("Tag for when casting Lucky Escape."));
	
	GameplayTag.Ability_Life_Elevate = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Life.Elevate"), FString("Tag for when casting Elevate."));
	
	GameplayTag.Ability_Life_Dispel = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Life.Dispel"), FString("Tag for when casting Dispel."));
	
	GameplayTag.Ability_Life_ImpenetrableBarrier = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Life.ImpenetrableBarrier"), FString("Tag for when casting Impenetrable Barrier."));
	
	
	/* Poison Mage Ability Tags */
	GameplayTag.Ability_Poison_PoisonMissile = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Poison.PoisonMissile"), FString("Ability Tag for Poison Missile."));
	
	GameplayTag.Ability_Poison_VirulentSmog = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Poison.VirulentSmog"), FString("Ability Tag for Virulent Smog."));
	
	GameplayTag.Ability_Poison_MiasmaSlide = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Poison.MiasmaSlide"), FString("Ability Tag for Miasma Slide."));
	
	GameplayTag.Ability_Poison_MorbidShroud = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Poison.MorbidShroud"), FString("Ability Tag for Morbid Shroud."));
	
	GameplayTag.Ability_Poison_NeuralAffliction = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Poison.NeuralAffliction"), FString("Ability Tag for Neural Affliction."));
	
	GameplayTag.Ability_Poison_PlagueDominion = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Poison.PlagueDominion"), FString("Ability Tag for Plague Dominion."));
	
	
	/* Fire Mage Cooldown Tags */
	GameplayTag.Cooldown_Fire_SearingFlame = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Fire.SearingFlame"), FString("Tag for when Searing Flame on Cooldown."));
	
	GameplayTag.Cooldown_Fire_ChanneledFire = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Fire.ChanneledFire"), FString("Tag for when Channeled Fire on Cooldown."));
	
	GameplayTag.Cooldown_Fire_IgnitionLeap = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Fire.IgnitionLeap"), FString("Tag for when Ignition Leap on Cooldown."));
	
	GameplayTag.Cooldown_Fire_RisingFlameWave = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Fire.RisingFlameWave"), FString("Tag for when Rising Flame Wave on Cooldown."));	
	
	GameplayTag.Cooldown_Fire_BlazingFlame = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Fire.BlazingFlame"), FString("Tag for when Blazing Flame on Cooldown."));
	
	GameplayTag.Cooldown_Fire_FallOfTheScorchingSun = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Fire.FallOfTheScorchingSun"), FString("Tag for when Fall of the Scorching Sun on Cooldown."));
	
	
	/* Water Mage Cooldown Tags */
	GameplayTag.Cooldown_Water_SimpleIce = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Water.SimpleIce"), FString("Tag for when Simple Ice on Cooldown."));
	
	GameplayTag.Cooldown_Water_SwirlingYoyo = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Water.SwirlingYoyo"), FString("Tag for when Swirling Yoyo on Cooldown."));
	
	GameplayTag.Cooldown_Water_RidingTide = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Water.RidingTide"), FString("Cooldown Tag for Riding Tide."));
	
	GameplayTag.Cooldown_Water_SoothingBubble = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Water.SoothingBubble"), FString("Cooldown Tag for Soothing Bubble."));
	
	GameplayTag.Cooldown_Water_DropRippleBlast = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Water.DropRippleBlast"), FString("Cooldown Tag for Drop Ripple Blast."));

	GameplayTag.Cooldown_Water_TearsDoFall = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Water.TearsDoFall"), FString("Cooldown Tag for Tears Do Fall."));


	/* Air Mage Cooldown Tags */
	GameplayTag.Cooldown_Air_SwiftGust = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Air.SwiftGust"), FString("Cooldown Tag for Swift Gust."));
	
	GameplayTag.Cooldown_Air_CatapultLaunch = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Air.CatapultLaunch"), FString("Cooldown Tag for Catapult Launch."));
	
	GameplayTag.Cooldown_Air_CleanWindDash = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Air.CleanWindDash"), FString("Cooldown Tag for Clean Wind Dash."));
	
	GameplayTag.Cooldown_Air_MountainVortex = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Air.MountainVortex"), FString("Cooldown Tag for Mountain Vortex."));
	
	GameplayTag.Cooldown_Air_BlueSkyWhirlwind = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Air.BlueSkyWhirlwind"), FString("Cooldown Tag for Blue Sky Whirlwind."));
	
	GameplayTag.Cooldown_Air_DanceOfTheWindDragon = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Air.DanceOfTheWindDragon"), FString("Cooldown Tag for Dance Of The Wind Dragon."));


	/* Earth Mage Cooldown Tags */
	GameplayTag.Cooldown_Earth_SlingRock = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Earth.SlingRock"), FString("Cooldown Tag for Sling Rock."));

	GameplayTag.Cooldown_Earth_JaggedTerraForm = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Earth.JaggedTerraForm"), FString("Cooldown Tag for Jagged Terra Form."));

	GameplayTag.Cooldown_Earth_ChargingBull = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Earth.ChargingBull"), FString("Cooldown Tag for Charging Bull."));

	GameplayTag.Cooldown_Earth_RockSolid = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Earth.RockSolid"), FString("Cooldown Tag for Rock Solid."));

	GameplayTag.Cooldown_Earth_GemJail = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Earth.GemJail"), FString("Cooldown Tag for Gem Jail."));

	GameplayTag.Cooldown_Earth_TheBoulder = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Earth.TheBoulder"), FString("Cooldown Tag for The Boulder."));


	/* Lightning Mage Cooldown Tags */
	GameplayTag.Cooldown_Lightning_LightningFlash = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Lightning.LightningFlash"), FString("Cooldown Tag for Lightning Flash."));

	GameplayTag.Cooldown_Lightning_SparkingSphere = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Lightning.SparkingSphere"), FString("Cooldown Tag for Sparking Sphere."));

	GameplayTag.Cooldown_Lightning_ZipNZap = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Lightning.ZipNZap"), FString("Cooldown Tag for Zip N Zap."));

	GameplayTag.Cooldown_Lightning_StaticMegaSurge = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Lightning.StaticMegaSurge"), FString("Cooldown Tag for Static Mega Surge."));

	GameplayTag.Cooldown_Lightning_FlashAndAwe = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Lightning.FlashAndAwe"), FString("Cooldown Tag for Flash And Awe."));

	GameplayTag.Cooldown_Lightning_CeaselessWrath = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Lightning.CeaselessWrath"), FString("Cooldown Tag for Ceaseless Wrath."));


	/* Nature Mage Cooldown Tags */
	GameplayTag.Cooldown_Nature_ThornSpread = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Nature.ThornSpread"), FString("Cooldown Tag for Thorn Spread."));

	GameplayTag.Cooldown_Nature_VenusFlyTrap = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Nature.VenusFlyTrap"), FString("Cooldown Tag for Venus Fly Trap."));

	GameplayTag.Cooldown_Nature_CreepingVines = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Nature.CreepingVines"), FString("Cooldown Tag for Creeping Vines."));

	GameplayTag.Cooldown_Nature_SepalSurprise = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Nature.SepalSurprise"), FString("Cooldown Tag for Sepal Surprise."));

	GameplayTag.Cooldown_Nature_LassoOfThorns = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Nature.LassoOfThorns"), FString("Cooldown Tag for Lasso Of Thorns."));

	GameplayTag.Cooldown_Nature_NaturesSpikyGrasp = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Nature.NaturesSpikyGrasp"), FString("Cooldown Tag for Natures Spiky Grasp."));


	/* Sound Mage Cooldown Tags */
	GameplayTag.Cooldown_Sound_Pleasc = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Sound.Pleasc"), FString("Cooldown Tag for Pleasc."));

	GameplayTag.Cooldown_Sound_GunAnail = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Sound.GunAnail"), FString("Cooldown Tag for Gun Anail."));

	GameplayTag.Cooldown_Sound_DeanAite = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Sound.DeanAite"), FString("Cooldown Tag for Dean Aite."));

	GameplayTag.Cooldown_Sound_AmMall = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Sound.AmMall"), FString("Cooldown Tag for Am Mall."));

	GameplayTag.Cooldown_Sound_GunGhluasad = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Sound.GunGhluasad"), FString("Cooldown Tag for Gun Ghluasad."));

	GameplayTag.Cooldown_Sound_TollahdFainne = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Sound.TollahdFainne"), FString("Cooldown Tag for Tollahd Fainne."));


	/* Shadow Mage Cooldown Tags */
	GameplayTag.Cooldown_Shadow_DustBomb = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Shadow.DustBomb"), FString("Cooldown Tag for Dust Bomb."));

	GameplayTag.Cooldown_Shadow_JokersFacade = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Shadow.JokersFacade"), FString("Cooldown Tag for Jokers Facade."));

	GameplayTag.Cooldown_Shadow_SmokeScreenShuffle = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Shadow.SmokeScreenShuffle"), FString("Cooldown Tag for Smoke Screen Shuffle."));

	GameplayTag.Cooldown_Shadow_ShadowGuide = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Shadow.ShadowGuide"), FString("Cooldown Tag for Shadow Guide."));

	GameplayTag.Cooldown_Shadow_SteamRush = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Shadow.SteamRush"), FString("Cooldown Tag for Steam Rush."));

	GameplayTag.Cooldown_Shadow_HiddenShadowTechnique = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Shadow.HiddenShadowTechnique"), FString("Cooldown Tag for Hidden Shadow Technique."));


	/* Necromancer Mage Cooldown Tags */
	GameplayTag.Cooldown_Necromancer_BubbleTrouble = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Necromancer.BubbleTrouble"), FString("Cooldown Tag for Bubble Trouble."));

	GameplayTag.Cooldown_Necromancer_BitsNBones = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Necromancer.BitsNBones"), FString("Cooldown Tag for Bits 'n' Bones."));

	GameplayTag.Cooldown_Necromancer_OldSkeller = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Necromancer.OldSkeller"), FString("Cooldown Tag for Old Skeller."));

	GameplayTag.Cooldown_Necromancer_LivingBlockade = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Necromancer.LivingBlockade"), FString("Cooldown Tag for Living Blockade."));

	GameplayTag.Cooldown_Necromancer_WrithingGrasp = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Necromancer.WrithingGrasp"), FString("Cooldown Tag for Writhing Grasp."));

	GameplayTag.Cooldown_Necromancer_RiseOfTheBlackCurse = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Necromancer.RiseOfTheBlackCurse"), FString("Cooldown Tag for Rise Of The Black Curse."));


	/* Holy Mage Cooldown Tags */
	GameplayTag.Cooldown_Holy_BeamingBlade = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Holy.BeamingBlade"), FString("Cooldown Tag for Beaming Blade."));

	GameplayTag.Cooldown_Holy_SacredSmite = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Holy.SacredSmite"), FString("Cooldown Tag for Sacred Smite."));

	GameplayTag.Cooldown_Holy_RadiantAscent = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Holy.RadiantAscent"), FString("Cooldown Tag for Radiant Ascent."));

	GameplayTag.Cooldown_Holy_DivineSanctuary = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Holy.DivineSanctuary"), FString("Cooldown Tag for Divine Sanctuary."));

	GameplayTag.Cooldown_Holy_ChainsOfAtonement = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Holy.ChainsOfAtonement"), FString("Cooldown Tag for Chains of Atonement."));

	GameplayTag.Cooldown_Holy_AngelicReckoning = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Holy.AngelicReckoning"), FString("Cooldown Tag for Angelic Reckoning."));
	
	
	/* Life Mage Cooldown Tags */
	GameplayTag.Cooldown_Life_EssenceTap = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Life.EssenceTap"), FString("Cooldown Tag for Essence Tap."));
	
	GameplayTag.Cooldown_Life_CircleOfLife = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Life.CircleOfLife"), FString("Cooldown Tag for Circle of Life."));
	
	GameplayTag.Cooldown_Life_LuckyEscape = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Life.LuckyEscape"), FString("Cooldown Tag for Lucky Escape."));
	
	GameplayTag.Cooldown_Life_Elevate = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Life.Elevate"), FString("Cooldown Tag for Elevate."));
	
	GameplayTag.Cooldown_Life_Dispel = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Life.Dispel"), FString("Cooldown Tag for Dispel."));
	
	GameplayTag.Cooldown_Life_ImpenetrableBarrier = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Life.ImpenetrableBarrier"), FString("Cooldown Tag for Impenetrable Barrier."));
	
	
	/* Poison Mage Cooldown Tags */
	GameplayTag.Cooldown_Poison_PoisonMissile = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Poison.PoisonMissile"), FString("Cooldown Tag for Poison Missile."));
	
	GameplayTag.Cooldown_Poison_VirulentSmog = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Poison.VirulentSmog"), FString("Cooldown Tag for Virulent Smog."));
	
	GameplayTag.Cooldown_Poison_MiasmaSlide = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Poison.MiasmaSlide"), FString("Cooldown Tag for Miasma Slide."));
	
	GameplayTag.Cooldown_Poison_MorbidShroud = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Poison.MorbidShroud"), FString("Cooldown Tag for Morbid Shroud."));
	
	GameplayTag.Cooldown_Poison_NeuralAffliction = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Poison.NeuralAffliction"), FString("Cooldown Tag for Neural Affliction."));
	
	GameplayTag.Cooldown_Poison_PlagueDominion = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Poison.PlagueDominion"), FString("Cooldown Tag for Plague Dominion."));
	
	
	/* Map of Ability Type to Ability Tag */
	/* Fire */
	GameplayTag.AbilityTypeToFireAbilityTag.Add(GameplayTag.Ability_Type_Primary, GameplayTag.Ability_Fire_SearingFlame);
	GameplayTag.AbilityTypeToFireAbilityTag.Add(GameplayTag.Ability_Type_Secondary, GameplayTag.Ability_Fire_ChanneledFire);
	GameplayTag.AbilityTypeToFireAbilityTag.Add(GameplayTag.Ability_Type_Movement, GameplayTag.Ability_Fire_IgnitionLeap);
	GameplayTag.AbilityTypeToFireAbilityTag.Add(GameplayTag.Ability_Type_Defensive, GameplayTag.Ability_Fire_RisingFlameWave);
	GameplayTag.AbilityTypeToFireAbilityTag.Add(GameplayTag.Ability_Type_CrowdControl, GameplayTag.Ability_Fire_BlazingFlame);
	GameplayTag.AbilityTypeToFireAbilityTag.Add(GameplayTag.Ability_Type_Ultimate, GameplayTag.Ability_Fire_FallOfTheScorchingSun);
	/* Water */
	GameplayTag.AbilityTypeToWaterAbilityTag.Add(GameplayTag.Ability_Type_Primary, GameplayTag.Ability_Water_SimpleIce);
	GameplayTag.AbilityTypeToWaterAbilityTag.Add(GameplayTag.Ability_Type_Secondary, GameplayTag.Ability_Water_SwirlingYoyo);
	GameplayTag.AbilityTypeToWaterAbilityTag.Add(GameplayTag.Ability_Type_Movement, GameplayTag.Ability_Water_RidingTide);
	GameplayTag.AbilityTypeToWaterAbilityTag.Add(GameplayTag.Ability_Type_Defensive, GameplayTag.Ability_Water_SoothingBubble);
	GameplayTag.AbilityTypeToWaterAbilityTag.Add(GameplayTag.Ability_Type_CrowdControl, GameplayTag.Ability_Water_DropRippleBlast);
	GameplayTag.AbilityTypeToWaterAbilityTag.Add(GameplayTag.Ability_Type_Ultimate, GameplayTag.Ability_Water_TearsDoFall);
	/* Air */
	GameplayTag.AbilityTypeToAirAbilityTag.Add(GameplayTag.Ability_Type_Primary, GameplayTag.Ability_Air_SwiftGust);
	GameplayTag.AbilityTypeToAirAbilityTag.Add(GameplayTag.Ability_Type_Secondary, GameplayTag.Ability_Air_CatapultLaunch);
	GameplayTag.AbilityTypeToAirAbilityTag.Add(GameplayTag.Ability_Type_Movement, GameplayTag.Ability_Air_CleanWindDash);
	GameplayTag.AbilityTypeToAirAbilityTag.Add(GameplayTag.Ability_Type_Defensive, GameplayTag.Ability_Air_MountainVortex);
	GameplayTag.AbilityTypeToAirAbilityTag.Add(GameplayTag.Ability_Type_CrowdControl, GameplayTag.Ability_Air_BlueSkyWhirlwind);
	GameplayTag.AbilityTypeToAirAbilityTag.Add(GameplayTag.Ability_Type_Ultimate, GameplayTag.Ability_Air_DanceOfTheWindDragon);
	/* Earth */
	GameplayTag.AbilityTypeToEarthAbilityTag.Add(GameplayTag.Ability_Type_Primary, GameplayTag.Ability_Earth_SlingRock);
	GameplayTag.AbilityTypeToEarthAbilityTag.Add(GameplayTag.Ability_Type_Secondary, GameplayTag.Ability_Earth_JaggedTerraForm);
	GameplayTag.AbilityTypeToEarthAbilityTag.Add(GameplayTag.Ability_Type_Movement, GameplayTag.Ability_Earth_ChargingBull);
	GameplayTag.AbilityTypeToEarthAbilityTag.Add(GameplayTag.Ability_Type_Defensive, GameplayTag.Ability_Earth_RockSolid);
	GameplayTag.AbilityTypeToEarthAbilityTag.Add(GameplayTag.Ability_Type_CrowdControl, GameplayTag.Ability_Earth_GemJail);
	GameplayTag.AbilityTypeToEarthAbilityTag.Add(GameplayTag.Ability_Type_Ultimate, GameplayTag.Ability_Earth_TheBoulder);
	/* Lightning */
	GameplayTag.AbilityTypeToLightningAbilityTag.Add(GameplayTag.Ability_Type_Primary, GameplayTag.Ability_Lightning_LightningFlash);
	GameplayTag.AbilityTypeToLightningAbilityTag.Add(GameplayTag.Ability_Type_Secondary, GameplayTag.Ability_Lightning_SparkingSphere);
	GameplayTag.AbilityTypeToLightningAbilityTag.Add(GameplayTag.Ability_Type_Movement, GameplayTag.Ability_Lightning_ZipNZap);
	GameplayTag.AbilityTypeToLightningAbilityTag.Add(GameplayTag.Ability_Type_Defensive, GameplayTag.Ability_Lightning_StaticMegaSurge);
	GameplayTag.AbilityTypeToLightningAbilityTag.Add(GameplayTag.Ability_Type_CrowdControl, GameplayTag.Ability_Lightning_FlashAndAwe);
	GameplayTag.AbilityTypeToLightningAbilityTag.Add(GameplayTag.Ability_Type_Ultimate, GameplayTag.Ability_Lightning_CeaselessWrath);
	/* Nature */
	GameplayTag.AbilityTypeToNatureAbilityTag.Add(GameplayTag.Ability_Type_Primary, GameplayTag.Ability_Nature_ThornSpread);
	GameplayTag.AbilityTypeToNatureAbilityTag.Add(GameplayTag.Ability_Type_Secondary, GameplayTag.Ability_Nature_VenusFlyTrap);
	GameplayTag.AbilityTypeToNatureAbilityTag.Add(GameplayTag.Ability_Type_Movement, GameplayTag.Ability_Nature_CreepingVines);
	GameplayTag.AbilityTypeToNatureAbilityTag.Add(GameplayTag.Ability_Type_Defensive, GameplayTag.Ability_Nature_SepalSurprise);
	GameplayTag.AbilityTypeToNatureAbilityTag.Add(GameplayTag.Ability_Type_CrowdControl, GameplayTag.Ability_Nature_LassoOfThorns);
	GameplayTag.AbilityTypeToNatureAbilityTag.Add(GameplayTag.Ability_Type_Ultimate, GameplayTag.Ability_Nature_NaturesSpikyGrasp);
	/* Sound */
	GameplayTag.AbilityTypeToSoundAbilityTag.Add(GameplayTag.Ability_Type_Primary, GameplayTag.Ability_Sound_Pleasc);
	GameplayTag.AbilityTypeToSoundAbilityTag.Add(GameplayTag.Ability_Type_Secondary, GameplayTag.Ability_Sound_GunAnail);
	GameplayTag.AbilityTypeToSoundAbilityTag.Add(GameplayTag.Ability_Type_Movement, GameplayTag.Ability_Sound_DeanAite);
	GameplayTag.AbilityTypeToSoundAbilityTag.Add(GameplayTag.Ability_Type_Defensive, GameplayTag.Ability_Sound_AmMall);
	GameplayTag.AbilityTypeToSoundAbilityTag.Add(GameplayTag.Ability_Type_CrowdControl, GameplayTag.Ability_Sound_GunGhluasad);
	GameplayTag.AbilityTypeToSoundAbilityTag.Add(GameplayTag.Ability_Type_Ultimate, GameplayTag.Ability_Sound_TollahdFainne);
	/* Shadow */
	GameplayTag.AbilityTypeToShadowAbilityTag.Add(GameplayTag.Ability_Type_Primary, GameplayTag.Ability_Shadow_DustBomb);
	GameplayTag.AbilityTypeToShadowAbilityTag.Add(GameplayTag.Ability_Type_Secondary, GameplayTag.Ability_Shadow_JokersFacade);
	GameplayTag.AbilityTypeToShadowAbilityTag.Add(GameplayTag.Ability_Type_Movement, GameplayTag.Ability_Shadow_SmokeScreenShuffle);
	GameplayTag.AbilityTypeToShadowAbilityTag.Add(GameplayTag.Ability_Type_Defensive, GameplayTag.Ability_Shadow_ShadowGuide);
	GameplayTag.AbilityTypeToShadowAbilityTag.Add(GameplayTag.Ability_Type_CrowdControl, GameplayTag.Ability_Shadow_SteamRush);
	GameplayTag.AbilityTypeToShadowAbilityTag.Add(GameplayTag.Ability_Type_Ultimate, GameplayTag.Ability_Shadow_HiddenShadowTechnique);
	/* Necromancer */
	GameplayTag.AbilityTypeToNecromancerAbilityTag.Add(GameplayTag.Ability_Type_Primary, GameplayTag.Ability_Necromancer_BubbleTrouble);
	GameplayTag.AbilityTypeToNecromancerAbilityTag.Add(GameplayTag.Ability_Type_Secondary, GameplayTag.Ability_Necromancer_BitsNBones);
	GameplayTag.AbilityTypeToNecromancerAbilityTag.Add(GameplayTag.Ability_Type_Movement, GameplayTag.Ability_Necromancer_OldSkeller);
	GameplayTag.AbilityTypeToNecromancerAbilityTag.Add(GameplayTag.Ability_Type_Defensive, GameplayTag.Ability_Necromancer_LivingBlockade);
	GameplayTag.AbilityTypeToNecromancerAbilityTag.Add(GameplayTag.Ability_Type_CrowdControl, GameplayTag.Ability_Necromancer_WrithingGrasp);
	GameplayTag.AbilityTypeToNecromancerAbilityTag.Add(GameplayTag.Ability_Type_Ultimate, GameplayTag.Ability_Necromancer_RiseOfTheBlackCurse);
	/* Holy */
	GameplayTag.AbilityTypeToHolyAbilityTag.Add(GameplayTag.Ability_Type_Primary, GameplayTag.Ability_Holy_BeamingBlade);
	GameplayTag.AbilityTypeToHolyAbilityTag.Add(GameplayTag.Ability_Type_Secondary, GameplayTag.Ability_Holy_SacredSmite);
	GameplayTag.AbilityTypeToHolyAbilityTag.Add(GameplayTag.Ability_Type_Movement, GameplayTag.Ability_Holy_RadiantAscent);
	GameplayTag.AbilityTypeToHolyAbilityTag.Add(GameplayTag.Ability_Type_Defensive, GameplayTag.Ability_Holy_DivineSanctuary);
	GameplayTag.AbilityTypeToHolyAbilityTag.Add(GameplayTag.Ability_Type_CrowdControl, GameplayTag.Ability_Holy_ChainsOfAtonement);
	GameplayTag.AbilityTypeToHolyAbilityTag.Add(GameplayTag.Ability_Type_Ultimate, GameplayTag.Ability_Holy_AngelicReckoning);
	/* Life */
	GameplayTag.AbilityTypeToLifeAbilityTag.Add(GameplayTag.Ability_Type_Primary, GameplayTag.Ability_Life_EssenceTap);
	GameplayTag.AbilityTypeToLifeAbilityTag.Add(GameplayTag.Ability_Type_Secondary, GameplayTag.Ability_Life_CircleOfLife);
	GameplayTag.AbilityTypeToLifeAbilityTag.Add(GameplayTag.Ability_Type_Movement, GameplayTag.Ability_Life_LuckyEscape);
	GameplayTag.AbilityTypeToLifeAbilityTag.Add(GameplayTag.Ability_Type_Defensive, GameplayTag.Ability_Life_Elevate);
	GameplayTag.AbilityTypeToLifeAbilityTag.Add(GameplayTag.Ability_Type_CrowdControl, GameplayTag.Ability_Life_Dispel);
	GameplayTag.AbilityTypeToLifeAbilityTag.Add(GameplayTag.Ability_Type_Ultimate, GameplayTag.Ability_Life_ImpenetrableBarrier);
	/* Poison */
	GameplayTag.AbilityTypeToPoisonAbilityTag.Add(GameplayTag.Ability_Type_Primary, GameplayTag.Ability_Poison_PoisonMissile);
	GameplayTag.AbilityTypeToPoisonAbilityTag.Add(GameplayTag.Ability_Type_Secondary, GameplayTag.Ability_Poison_VirulentSmog);
	GameplayTag.AbilityTypeToPoisonAbilityTag.Add(GameplayTag.Ability_Type_Movement, GameplayTag.Ability_Poison_MiasmaSlide);
	GameplayTag.AbilityTypeToPoisonAbilityTag.Add(GameplayTag.Ability_Type_Defensive, GameplayTag.Ability_Poison_MorbidShroud);
	GameplayTag.AbilityTypeToPoisonAbilityTag.Add(GameplayTag.Ability_Type_CrowdControl, GameplayTag.Ability_Poison_NeuralAffliction);
	GameplayTag.AbilityTypeToPoisonAbilityTag.Add(GameplayTag.Ability_Type_Ultimate, GameplayTag.Ability_Poison_PlagueDominion);
	
	
	/* M&K Input Tags */
	GameplayTag.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.LMB"), FString("Input Tag for Left Mouse Button."));
	
	GameplayTag.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.RMB"), FString("Input Tag for Right Mouse Button."));
	
	GameplayTag.InputTag_W = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.W"), FString("Input Tag for W Key."));
	
	GameplayTag.InputTag_A = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.A"), FString("Input Tag for A Key."));
	
	GameplayTag.InputTag_S = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.S"), FString("Input Tag for S Key."));
	
	GameplayTag.InputTag_D = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.D"), FString("Input Tag for D Key."));
	
	GameplayTag.InputTag_Q = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.Q"), FString("Input Tag for Q Key."));
	
	GameplayTag.InputTag_E = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.E"), FString("Input Tag for E Key."));
	
	GameplayTag.InputTag_Shift = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.Shift"), FString("Input Tag for Shift Key."));
	
	GameplayTag.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.1"), FString("Input Tag for  1 Key."));
	
	GameplayTag.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.2"), FString("Input Tag for 2 Key."));
	
	GameplayTag.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.3"), FString("Input Tag for 3 Key."));
	
	GameplayTag.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.4"), FString("Input Tag for 4 Key."));
	
	
	/* Controller Input Tags */
	GameplayTag.InputTag_Controller_LeftStick = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.Controller.LeftStick"), FString("Input Tag for Controller Left Stick."));

	GameplayTag.InputTag_Controller_RightStick = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.Controller.RightStick"), FString("Input Tag for Controller Right Stick."));

	GameplayTag.InputTag_Controller_LeftShoulder = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.Controller.LeftShoulder"), FString("Input Tag for Controller Left Shoulder Button."));

	GameplayTag.InputTag_Controller_RightShoulder = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.Controller.RightShoulder"), FString("Input Tag for Controller Right Shoulder Button."));

	GameplayTag.InputTag_Controller_A = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.Controller.A"), FString("Input Tag for Controller A Button."));

	GameplayTag.InputTag_Controller_B = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.Controller.B"), FString("Input Tag for Controller B Button."));

	GameplayTag.InputTag_Controller_X = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.Controller.X"), FString("Input Tag for Controller X Button."));

	GameplayTag.InputTag_Controller_Y = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.Controller.Y"), FString("Input Tag for Controller Y Button."));

	GameplayTag.InputTag_Controller_LT = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.Controller.LT"), FString("Input Tag for Controller Left Trigger."));

	GameplayTag.InputTag_Controller_RT = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.Controller.RT"), FString("Input Tag for Controller Right Trigger."));

	GameplayTag.InputTag_Controller_Up = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.Controller.Up"), FString("Input Tag for Controller D-Pad Up."));

	GameplayTag.InputTag_Controller_Down = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.Controller.Down"), FString("Input Tag for Controller D-Pad Down."));

	GameplayTag.InputTag_Controller_Left = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.Controller.Left"), FString("Input Tag for Controller D-Pad Left."));

	GameplayTag.InputTag_Controller_Right = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.Controller.Right"), FString("Input Tag for Controller D-Pad Right."));

	GameplayTag.InputTag_Controller_Start = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.Controller.Start"), FString("Input Tag for Controller Start Button."));

	GameplayTag.InputTag_Controller_Back = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("InputTag.Controller.Back"), FString("Input Tag for Controller Back Button."));


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

	GameplayTag.DamageType_MagicalDamage_Ice = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("DamageType.MagicalDamage.Ice"), FString("Amount of Ice Damage Done. Can freeze targets at -100 Heat."));

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
	GameplayTag.Resistance_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Resistance.Damage"), FString("Overall resistance applied to all incoming damage, regardless of type."));

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
	GameplayTag.DamageTypeToResistance.Add(GameplayTag.DamageType_MagicalDamage_Ice, GameplayTag.Resistance_MagicalDamage_Cold);
	GameplayTag.DamageTypeToResistance.Add(GameplayTag.DamageType_MagicalDamage_Electric, GameplayTag.Resistance_MagicalDamage_Electric);
	GameplayTag.DamageTypeToResistance.Add(GameplayTag.DamageType_MagicalDamage_Poison, GameplayTag.Resistance_MagicalDamage_Poison);
	GameplayTag.DamageTypeToResistance.Add(GameplayTag.DamageType_MagicalDamage_Acid, GameplayTag.Resistance_MagicalDamage_Acid);
	GameplayTag.DamageTypeToResistance.Add(GameplayTag.DamageType_MagicalDamage_Shadow, GameplayTag.Resistance_MagicalDamage_Shadow);
	GameplayTag.DamageTypeToResistance.Add(GameplayTag.DamageType_MagicalDamage_Radiant, GameplayTag.Resistance_MagicalDamage_Radiant);
	GameplayTag.DamageTypeToResistance.Add(GameplayTag.DamageType_MagicalDamage_Force, GameplayTag.Resistance_MagicalDamage_Force);
	GameplayTag.DamageTypeToResistance.Add(GameplayTag.DamageType_MagicalDamage_Psychic , GameplayTag.Resistance_MagicalDamage_Psychic);


	/* Map of Damage Type Tags to Mechanics Attribute Tags */
	GameplayTag.DamageTypeToMechanics.Add(GameplayTag.DamageType_MagicalDamage_Fire, GameplayTag.Attributes_Mechanics_Heat);
	GameplayTag.DamageTypeToMechanics.Add(GameplayTag.DamageType_MagicalDamage_Cold, GameplayTag.Attributes_Mechanics_Heat);
	GameplayTag.DamageTypeToMechanics.Add(GameplayTag.DamageType_MagicalDamage_Ice, GameplayTag.Attributes_Mechanics_Heat);
	GameplayTag.DamageTypeToMechanics.Add(GameplayTag.DamageType_MagicalDamage_Electric, GameplayTag.Attributes_Mechanics_Charge);
	GameplayTag.DamageTypeToMechanics.Add(GameplayTag.DamageType_MagicalDamage_Poison, GameplayTag.Attributes_Mechanics_LethalToxins);
	GameplayTag.DamageTypeToMechanics.Add(GameplayTag.DamageType_MagicalDamage_Force, GameplayTag.Attributes_Mechanics_Momentum);
	GameplayTag.DamageTypeToMechanics.Add(GameplayTag.DamageType_MagicalDamage_Shadow, GameplayTag.Attributes_Mechanics_Obscurity);
	GameplayTag.DamageTypeToMechanics.Add(GameplayTag.DamageType_MagicalDamage_Radiant, GameplayTag.Attributes_Mechanics_Retribution);
	GameplayTag.DamageTypeToMechanics.Add(GameplayTag.DamageType_MagicalDamage_Acid, GameplayTag.Attributes_Mechanics_BlackOmen);
	GameplayTag.DamageTypeToMechanics.Add(GameplayTag.DamageType_MagicalDamage_Psychic, GameplayTag.Attributes_Mechanics_Crescendo);


	/* M&K InputTag -> Controller InputTag default mapping */
	GameplayTag.InputTagToControllerInputTag.Add(GameplayTag.InputTag_LMB, GameplayTag.InputTag_Controller_LT);
	GameplayTag.InputTagToControllerInputTag.Add(GameplayTag.InputTag_RMB, GameplayTag.InputTag_Controller_RT);
	GameplayTag.InputTagToControllerInputTag.Add(GameplayTag.InputTag_Shift, GameplayTag.InputTag_Controller_RightShoulder);
	GameplayTag.InputTagToControllerInputTag.Add(GameplayTag.InputTag_1, GameplayTag.InputTag_Controller_A);
	GameplayTag.InputTagToControllerInputTag.Add(GameplayTag.InputTag_2, GameplayTag.InputTag_Controller_B);
	GameplayTag.InputTagToControllerInputTag.Add(GameplayTag.InputTag_3, GameplayTag.InputTag_Controller_X);
	GameplayTag.InputTagToControllerInputTag.Add(GameplayTag.InputTag_4, GameplayTag.InputTag_Controller_Y);


	/* Mechanics Stage Tags */
	GameplayTag.MechanicsStage_Heat_Cold_1 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Heat.Cold1"), FString("Heat -25 to -49: 10% Movement Slow."));

	GameplayTag.MechanicsStage_Heat_Cold_2 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Heat.Cold2"), FString("Heat -50 to -74: 20% Movement Slow."));

	GameplayTag.MechanicsStage_Heat_Cold_3 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Heat.Cold3"), FString("Heat -75 to -99: 30% Movement Slow."));

	GameplayTag.MechanicsStage_Heat_Frozen = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Heat.Frozen"), FString("Heat -100 or lower: Frozen, unable to Move or make any Action."));

	GameplayTag.MechanicsStage_Heat_Hot_1 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Heat.Hot1"), FString("Heat 25 to 49: 10% increased Fire Damage taken."));

	GameplayTag.MechanicsStage_Heat_Hot_2 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Heat.Hot2"), FString("Heat 50 to 74: 20% increased Fire Damage taken."));

	GameplayTag.MechanicsStage_Heat_Hot_3 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Heat.Hot3"), FString("Heat 75 to 99: 30% increased Fire Damage taken."));

	GameplayTag.MechanicsStage_Heat_Ignited = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Heat.Ignited"), FString("Heat 100 or higher: Burning DoT and 40% increased Fire Damage taken."));
	
	GameplayTag.MechanicsStage_Charge_Charged = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Charge.Charged"), FString("Charge 50 or higher: Electric Damage can now chain to you."));
	
	GameplayTag.MechanicsStage_Charge_OverCharged = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Charge.OverCharged"), FString("Charge 100 or higher: Electric Damage can now chain from you to nearby allies."));
	
	GameplayTag.MechanicsStage_Momentum_0 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Momentum.0"), FString("Momentum 0: No effect."));
	
	GameplayTag.MechanicsStage_Momentum_1 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Momentum.1"), FString("Momentum 1: 10% increased Movement Speed."));

	GameplayTag.MechanicsStage_Momentum_2 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Momentum.2"), FString("Momentum 2: 20% increased Movement Speed."));
	
	GameplayTag.MechanicsStage_Momentum_3 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Momentum.3"), FString("Momentum 3: 30% increased Movement Speed."));
	
	GameplayTag.MechanicsStage_ImmovableMass_0 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.ImmovableMass.0"), FString("Immovable Mass 0: No effect."));
	
	GameplayTag.MechanicsStage_ImmovableMass_1 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.ImmovableMass.1"), FString("Immovable Mass 1: 20% increased Damage and Knockback Resistance and MovementSpeed - 15%."));
	
	GameplayTag.MechanicsStage_ImmovableMass_2 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.ImmovableMass.2"), FString("Immovable Mass 2: 40% increased Damage and Knockback Resistance and MovementSpeed -30%."));
	
	GameplayTag.MechanicsStage_ImmovableMass_3 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.ImmovableMass.3"), FString("Immovable Mass 3: 60% increased Damage and Knockback Resistance and MovementSpeed halved."));
	
	GameplayTag.MechanicsStage_Overgrowth_0 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Overgrowth.0"), FString("Overgrowth 0: No effect."));
	
	GameplayTag.MechanicsStage_Overgrowth_1 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Overgrowth.1"), FString("Overgrowth 1: Spell AoE Size increased by 15%."));
	
	GameplayTag.MechanicsStage_Overgrowth_2 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Overgrowth.2"), FString("Overgrowth 2: Spell AoE Size increased by 25%. Attack Speed increased by 10%. Plant Attack Speed increased by 10% and pulse a small heal while you are standing in their zone."));
	
	GameplayTag.MechanicsStage_Overgrowth_3 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Overgrowth.3"), FString("Overgrowth 3: Spell AoE Size increased by 35%. Attack Speed increased by 20%. Plant Attack Speed increased by 20% and pulse a medium heal while you are standing in their zone."));
	
	GameplayTag.MechanicsStage_Crescendo_0 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Crescendo.0"), FString("Crescendo 0: No effect."));
	
	GameplayTag.MechanicsStage_Crescendo_1 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Crescendo.1"), FString("Crescendo 1: Spell AoE Size increased by 25%."));
	
	GameplayTag.MechanicsStage_Crescendo_2 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Crescendo.2"), FString("Crescendo 2: Spell AoE Size increased by 35%. Attack Speed increased by 10%. Plant Attack Speed increased by 10% and pulse a small heal while you are standing in their zone."));
	
	GameplayTag.MechanicsStage_Crescendo_3 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Crescendo.3"), FString("Crescendo 3: Spell AoE Size increased by 45%. Attack Speed increased by 20%. Plant Attack Speed increased by 20% and pulse a medium heal while you are standing in their zone."));

	GameplayTag.MechanicsStage_Obscurity_0 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Obscurity.0"), FString("Obscurity 0: No effect."));
	
	GameplayTag.MechanicsStage_Obscurity_1 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Obscurity.1"), FString("Obscurity 1: Clones/Decoys last 1 second longer. Enemies cannot see you casting during the first 20% of your cast animation."));
	
	GameplayTag.MechanicsStage_Obscurity_2 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Obscurity.2"), FString("Obscurity 2: Clones/Decoys last 2 seconds longer. Enemies cannot see you casting during the first 40% of your cast animation. Decoy swap cooldown reduced by 1 second."));
	
	GameplayTag.MechanicsStage_Obscurity_3 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Obscurity.3"), FString(
		"Obscurity 3: Clones/Decoys last 3 seconds longer. Enemies cannot see you casting during the first 60% of your cast animation. Decoy swap cooldown reduced by 2 seconds. The first Spell cast in this stage gains a bonus effect."));
	
	GameplayTag.MechanicsStage_Retribution_0 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Retribution.0"), FString("Retribution 0: No effect."));

	GameplayTag.MechanicsStage_Retribution_1 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Retribution.1"), FString("Retribution 1: Spell AoE Size increased by 35%. Attack Speed increased by 10%. Plant Attack Speed increased by 10% and pulse a small heal while you are standing in their zone."));

	GameplayTag.MechanicsStage_Retribution_2 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Retribution.2"), FString("Retribution 2: Spell AoE Size increased by 45%. Attack Speed increased by 20%. Plant Attack Speed increased by 20% and pulse a medium heal while you are standing in their zone."));

	GameplayTag.MechanicsStage_Retribution_3 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.Retribution.3"), FString("Retribution 3: Spell AoE Size increased by 55%. Attack Speed increased by 30%. Plant Attack Speed increased by 30% and pulse a large heal while you are standing in their zone."));
	
	GameplayTag.MechanicsStage_ConstantCirculation_0 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.ConstantCirculation.0"), FString("Constant Circulation 0: No effect."));
	
	GameplayTag.MechanicsStage_ConstantCirculation_1 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.ConstantCirculation.1"), FString("Constant Circulation 1: Spell AoE Size increased by 15%."));
	
	GameplayTag.MechanicsStage_ConstantCirculation_2 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.ConstantCirculation.2"), FString("Constant Circulation 2: Spell AoE Size increased by 25%."));
	
	GameplayTag.MechanicsStage_ConstantCirculation_3 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("MechanicsStage.ConstantCirculation.3"), FString("Constant Circulation 3: Spell AoE Size increased by 35%."));
	
	/* Gameplay Cue Tags */
	GameplayTag.GameplayCue_Heat_Glow = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("GameplayCue.Heat.Glow"), FString("Gameplay Cue for Heat glow visual effect on character."));


	/* Effect Tags */
	GameplayTag.Effects_HitReaction = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Effects.HitReaction"), FString("Tag for when Hit Reacting."));

	
	/* Debuff Effect Tags */
	GameplayTag.Condition_Airborne = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Airborne"), FString("Tag for when not touching the ground."));
	
	GameplayTag.Condition_Burning = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Burning"), FString("Tag for when Burning, doing Damage Over Time."));
	
	GameplayTag.Condition_Poisoned = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Poisoned"), FString("Tag for when Poisoned, doing Damage Over Time."));
	
	GameplayTag.Condition_Healing = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Healing"), FString("Tag for when Healing, increasing Health Over Time."));
	
	GameplayTag.Condition_Slowed = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Slowed"), FString("Tag for when Slowed, Slows Movement Speed."));
	
	GameplayTag.Condition_Rooted = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Rooted"), FString("Tag for when Rooted, unable to Move."));
	
	GameplayTag.Condition_Grappled = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Grappled"), FString("Tag for when Grappled, unable to Move or make any Action but can Struggle free."));
	
	GameplayTag.Condition_Petrified = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Petrified"), FString("Tag for when Petrified, unable to Move or make any Action but can Struggle free."));
	
	GameplayTag.Condition_Constricted = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Constricted"), FString("Tag for when Constricted, unable to Move or make any Action."));
	
	GameplayTag.Condition_Frozen = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Frozen"), FString("Tag for when Frozen, unable to Move or make any Action."));
	
	GameplayTag.Condition_Stunned = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Stunned"), FString("Tag for when Stunned, unable to Move or make any Action."));

	GameplayTag.Condition_StunImmune = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.StunImmune"), FString("Tag for a brief grace period after Stunned ends, preventing immediate re-stunning (stun-lock protection)."));

	/* Per-condition immunity grace tags. Each CC grants only its OWN immunity and lists only that same
	 * tag in BlockedByConditions, so a target can be chained with DIFFERENT crowd control
	 * (stun -> petrify -> freeze) but never re-hit by the same one inside its grace window. */

	GameplayTag.Condition_FrozenImmune = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.FrozenImmune"), FString("Grace period after Frozen ends, preventing immediate re-freezing."));

	GameplayTag.Condition_PetrifiedImmune = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.PetrifiedImmune"), FString("Grace period after Petrified ends, preventing immediate re-petrification."));

	GameplayTag.Condition_GrappledImmune = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.GrappledImmune"), FString("Grace period after Grappled ends, preventing immediate re-grappling."));

	GameplayTag.Condition_ConstrictedImmune = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.ConstrictedImmune"), FString("Grace period after Constricted ends, preventing immediate re-constriction."));

	GameplayTag.Condition_ParalysedImmune = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.ParalysedImmune"), FString("Grace period after Paralysed ends, preventing immediate re-paralysis."));

	GameplayTag.Condition_ShockedImmune = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.ShockedImmune"), FString("Grace period after Shocked ends, preventing immediate re-shocking."));

	GameplayTag.Condition_Paralysed = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Paralysed"), FString("Tag for when Paralysed, unable to Move or make any Action."));
	
	GameplayTag.Condition_Pushed = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Pushed"), FString("Tag for when Pushed, Moved by Force."));
	
	GameplayTag.Condition_Slammed = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Slammed"), FString("Tag for when Slammed, Hit another Actor or Wall with Force."));
	
	GameplayTag.Condition_Confused = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Confused"), FString("Tag for when Confused, Inverting Player's Controls."));
	
	GameplayTag.Condition_Reflecting = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Reflecting"), FString("Tag for when Reflecting, sends Projectiles back the way they came from."));
	
	GameplayTag.Condition_SoulBound = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.SoulBound"), FString("Tag for when SoulBound, Tethered to the Soul by the Lich."));
	
	GameplayTag.Condition_Invisible = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Invisible"), FString("Tag for when Invisible, can not be seen."));
	
	GameplayTag.Condition_Silenced = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Silenced"), FString("Tag for when Silenced, can not use Abilities."));
	
	GameplayTag.Condition_Untouchable = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Untouchable"), FString("Tag for when Untouchable, can not be Damaged by Abilities."));
	
	GameplayTag.Condition_Immune = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Immune"), FString("Tag for when Immune to everything, can not be Damaged NOR affected by any Condition."));

	GameplayTag.Condition_DamageImmune = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.DamageImmune"), FString("Tag for when Immune to Damage, can not be Damaged by certain Type(s)."));

	GameplayTag.Condition_Shocked = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Shocked"), FString("Tag for when Shocked, unable to Move or make any Action."));

	GameplayTag.Condition_Charged = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Charged"), FString("Tag for when Charged, able for Electric Spells to potentially chain to you."));

	GameplayTag.Condition_OverCharged = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.OverCharged"), FString("Tag for when OverCharged, able for Electric Spells to chain from you."));

	GameplayTag.Condition_Wet = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Wet"), FString("Target is Wet. Increases Cold damage taken and contributes to Frozen threshold."));

	GameplayTag.Condition_Frostbite = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Condition.Frostbite"), FString("Target has Frostbite. Simple Ice freezes on hit and deals bonus damage."));

	/* Status Tags */
	GameplayTag.Status_Shielded = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Status.Shielded"), FString("Target is protected by a damage-absorbing shield."));

	GameplayTag.Status_Channeling = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Status.Channeling"), FString("Actor is channeling a spell. Blocks activation of other abilities."));

	GameplayTag.Status_Reflecting = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Status.Reflecting"), FString("Target zaps incoming attackers back. Generic reflect-on-hit status."));

	/* Movement State Tags */
	GameplayTag.State_Dashing = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("State.Dashing"), FString("Granted while a dash/charge movement ability is active. Drives increased ImmovableMass decay for the Earth mage."));


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
	(FName("Montage.Attack.1"), FString("Tag for Montage Attack 1."));
	
	GameplayTag.Montage_Attack_2 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Montage.Attack.2"), FString("Tag for Montage Attack 2."));
	
	GameplayTag.Montage_Attack_3 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Montage.Attack.3"), FString("Tag for Montage Attack 3."));
	
	GameplayTag.Montage_Attack_4 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Montage.Attack.4"), FString("Tag for Montage Attack 4."));
	
	GameplayTag.Montage_Attack_5 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Montage.Attack.5"), FString("Tag for Montage Attack 5."));
	
	GameplayTag.Montage_Attack_6 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Montage.Attack.6"), FString("Tag for Montage Attack 6."));
	
	GameplayTag.Montage_Attack_7 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Montage.Attack.7"), FString("Tag for Montage Attack 7."));
	
	GameplayTag.Montage_Attack_8 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Montage.Attack.8"), FString("Tag for Montage Attack 8."));
	
	GameplayTag.Montage_Attack_9 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Montage.Attack.9"), FString("Tag for Montage Attack 9."));
	
	GameplayTag.Montage_Attack_10 = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Montage.Attack.10"), FString("Tag for Montage Attack 10."));
	
	
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

const TMap<FGameplayTag, FGameplayTag>* FRageInMageGameplayTag::GetAbilityTypeMapForClass(
	ECharacterClass CharacterClass) const
{
	switch (CharacterClass)
	{
	case ECharacterClass::FireMage:       return &AbilityTypeToFireAbilityTag;
	case ECharacterClass::WaterMage:      return &AbilityTypeToWaterAbilityTag;
	case ECharacterClass::AirMage:        return &AbilityTypeToAirAbilityTag;
	case ECharacterClass::EarthMage:      return &AbilityTypeToEarthAbilityTag;
	case ECharacterClass::LightningMage:  return &AbilityTypeToLightningAbilityTag;
	case ECharacterClass::NatureMage:     return &AbilityTypeToNatureAbilityTag;
	case ECharacterClass::SoundMage:      return &AbilityTypeToSoundAbilityTag;
	case ECharacterClass::ShadowMage:     return &AbilityTypeToShadowAbilityTag;
	case ECharacterClass::Necromancer:    return &AbilityTypeToNecromancerAbilityTag;
	case ECharacterClass::HolyMage:       return &AbilityTypeToHolyAbilityTag;
	case ECharacterClass::LifeMage:       return &AbilityTypeToLifeAbilityTag;
	case ECharacterClass::PoisonMage:     return &AbilityTypeToPoisonAbilityTag;
	default:                              return nullptr;
	}
}
