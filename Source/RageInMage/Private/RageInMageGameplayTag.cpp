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
	
	GameplayTag.Ability_Summon = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Summon"), FString("Tag for when Summoning."));
	
	
	/* Ability Type Tags */	
	GameplayTag.AbilityType_Basic = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("AbilityType.Basic"), FString("Tag for when Casting a Basic Spell."));
	
	GameplayTag.AbilityType_Secondary = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("AbilityType.Secondary"), FString("Tag for when Casting a Secondary Spell."));
	
	GameplayTag.AbilityType_Movement = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("AbilityType.Movement"), FString("Tag for when Casting a Movement Spell."));
	
	GameplayTag.AbilityType_Defensive = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("AbilityType.Defensive"), FString("Tag for when Casting a Defensive Spell."));
	
	GameplayTag.AbilityType_CrowdControl = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("AbilityType.CrowdControl"), FString("Tag for when Casting a Crowd Control Spell."));
	
	GameplayTag.AbilityType_Ultimate = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("AbilityType.Ultimate"), FString("Tag for when Casting an Ultimate Spell."));
		
	
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

	GameplayTag.Ability_Earth_JewelFistShatter = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Earth.JewelFistShatter"), FString("Tag for when casting Jewel Fist Shatter."));

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

	GameplayTag.Ability_Holy_HolyBolt_Chain = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Holy.HolyBolt.Chain"), FString("Tag for when casting Holy Bolt Chain."));

	GameplayTag.Ability_Holy_DivineSanctuary = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Holy.DivineSanctuary"), FString("Tag for when casting Divine Sanctuary."));

	GameplayTag.Ability_Holy_HolyBolt_Storm = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Holy.HolyBolt.Storm"), FString("Tag for when casting Holy Bolt Storm."));

	GameplayTag.Ability_Holy_HolyShield = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Ability.Holy.HolyShield"), FString("Tag for when casting Holy Shield."));
	
	
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

	GameplayTag.Cooldown_Earth_JewelFistShatter = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Earth.JewelFistShatter"), FString("Cooldown Tag for Jewel Fist Shatter."));

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

	GameplayTag.Cooldown_Holy_HolyBolt_Chain = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Holy.HolyBolt.Chain"), FString("Cooldown Tag for Holy Bolt Chain."));

	GameplayTag.Cooldown_Holy_DivineSanctuary = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Holy.DivineSanctuary"), FString("Cooldown Tag for Divine Sanctuary."));

	GameplayTag.Cooldown_Holy_HolyBolt_Storm = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Holy.HolyBolt.Storm"), FString("Cooldown Tag for Holy Bolt Storm."));

	GameplayTag.Cooldown_Holy_HolyShield = UGameplayTagsManager::Get().AddNativeGameplayTag
	(FName("Cooldown.Holy.HolyShield"), FString("Cooldown Tag for Holy Shield."));
	
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
