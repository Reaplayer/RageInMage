# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

RageInMage is a multiplayer action RPG built on **Unreal Engine 5.6** using the **Gameplay Ability System (GAS)**. It features 12 elemental magic schools (Fire, Water, Air, Earth, Lightning, Nature, Sound, Shadow, Necromancer, Holy, Life, Poison), each with 6 abilities (72 total), and 21 character classes. Copyright Krymson Reaplays.

## Build Commands

```bash
# Build from command line (adjust UE path to your install)
"C:/Program Files/Epic Games/UE_5.6/Engine/Build/BatchFiles/Build.bat" RageInMage Win64 Development "C:/Users/reapw/Documents/UnrealProjects/RageInMage5.6/RageInMage.uproject" -waitmutex

# Build editor target
"C:/Program Files/Epic Games/UE_5.6/Engine/Build/BatchFiles/Build.bat" RageInMageEditor Win64 Development "C:/Users/reapw/Documents/UnrealProjects/RageInMage5.6/RageInMage.uproject" -waitmutex

# Generate project files
"C:/Program Files/Epic Games/UE_5.6/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.exe" -projectfiles -project="C:/Users/reapw/Documents/UnrealProjects/RageInMage5.6/RageInMage.uproject" -game -engine
```

The project can also be built via Visual Studio using `RageInMage.sln`.

## Module Configuration

Single module: **RageInMage** (Runtime, Default loading phase).

- **Public dependencies:** Core, CoreUObject, Engine, InputCore, NavigationSystem, AIModule, Niagara, EnhancedInput, GameplayAbilities
- **Private dependencies:** GameplayTags, GameplayTasks, Slate, SlateCore, GameplayAbilitiesEditor
- **Plugins:** GameplayAbilities, MotionWarping, LineOfSight, WaterAdvanced, Water, ModelingToolsEditorMode (editor only)
- Game target uses `IncludeOrderVersion::Unreal5_5`; Editor target uses `Unreal5_6`

## Architecture

### Character Hierarchy

`ARageInMageCharacterBase` (implements `IAbilitySystemInterface`, `ICombatInterface`)
- `ARageInMagePlayerCharacter` — top-down camera, level-based progression, implements `IPlayerInterface`
- `ARageInMageEnemyCharacter` — AI-driven via behavior trees, health bar widget, implements `IEnemyInterface`, supports summoning (max 5)

Characters have a weapon skeletal mesh component with socket system (WeaponTip, LeftHand, RightHand, Tail) for ability VFX attachment.

### GAS (Gameplay Ability System)

This is the core framework. Key classes:

- **`URageInMageAbilitySystemComponent`** — Custom ASC with input tag management, startup/passive ability granting, and attribute upgrade support
- **`URageInMageAttributeSet`** — 40+ replicated attributes across categories:
  - **Primary (7):** Strength, Dexterity, Agility, Intelligence, Wit, Vigor, Endurance
  - **Secondary (10+):** PhysicalAttack, MagicalAttack, CriticalChance, CriticalDamage, AttackSpeed, MovementSpeed, defences, penetrations, MaxHealth, MaxMana
  - **Vital (2):** Health, Mana
  - **Resistance (14):** Physical (Overall/Slashing/Piercing/Bludgeoning), Magical (Overall/Fire/Cold/Electric/Poison/Acid/Shadow/Radiant/Force/Psychic)
  - **Mechanics (11):** Heat, Momentum, ImmovableMass, Charge, Overgrowth, Crescendo, Obscurity, BlackOmen, Retribution, ConstantCirculation, LethalToxins
  - **Meta:** IncomingDamage, IncomingXP
- **`FRageInMageGameplayEffectContext`** — Extended context tracking critical/vulnerable/resistant hit flags
- **`UExecCalc_Damage`** — Damage execution calculation with crit/vulnerability/resistance logic

### Ability Hierarchy

`URageInMageGameplayAbility` (base — has StartupInputTag, StartupAbilityTag, StartupAbilityTypeTag)
- `URageInMageDamageGameplayAbility` — damage-dealing base with DamageTypeTags map and montage selection
  - `URageInMageProjectileSpell` — projectile abilities with motion tracking and target leading
  - `URageInMageMeleeAttack` — melee abilities
- `URageInMageSummonGameplayAbility` — spawns minions in circular formation

### Gameplay Tags (`FRageInMageGameplayTag`)

All tags are initialized in a single static struct. Tag categories:
- **Ability tags:** `Abilities.<School>.<AbilityName>` (12 schools x 6 abilities)
- **Ability type tags:** Primary, Secondary, Movement, Defensive, CrowdControl, Ultimate, Passive1-3
- **Damage types:** `DamageType.<Type>` (12 types)
- **Debuffs/CC:** 20+ effects (Burning, Frozen, Stunned, Airborne, Petrified, etc.)
- **Input tags:** `InputTag.<Key>` for keyboard and controller
- **Combat sockets:** Weapon, LeftHand, RightHand, Tail
- **Montage tags:** Attack_1 through Attack_10
- **Team tags:** 11 team colors

### UI Architecture

`ARageInMageHUD` is the central factory for widget controllers (lazy initialization, per-player caching).

Widget controller hierarchy:
- `URageInMageWidgetControllerBase` — stores GAS references (PC, PS, ASC, AS), binds delegates
  - `UOverlayWidgetController` — health/mana bars, XP interpolation, level display, message system
  - `UTabbedMenuWidgetController` — shared tabbed menu infrastructure
    - `UAttributeMenuWidgetController` — attribute info broadcasting, point spending
    - `USpellMenuWidgetController` — spell list management, spell point spending

All widgets extend `URageInMageUserWidget` which receives its controller via `SetWidgetController()`.

### Data Assets

- **`UCharacterClassInfo`** — per-class defaults: primary attribute effects, startup abilities, XP rewards, UI styling (21 classes)
- **`UAbilityInfo`** — ability metadata: tags, cooldowns, types, input bindings, descriptions
- **`UAttributeInfo`** — attribute metadata: tags, names, descriptions
- **`ULevelUpInfo`** — per-level XP thresholds, attribute/spell point rewards

### Player Progression

`ARageInMagePlayerState` manages XP, Level, AttributePoints, SpellPoints (all replicated). Level-up triggers multicast RPC for particle effects.

### Input System

Uses UE5 Enhanced Input. `URageInMageConfig` (DataAsset) maps `UInputAction` to `FGameplayTag`. `URageInMageEInputComponent` provides template-based binding. Flow: Input action -> PlayerController -> gameplay tag -> ASC -> ability activation.

### Networking

- All attributes replicated with OnRep callbacks
- Player progression stats replicated on PlayerState
- Multicast RPCs for death/level-up effects
- Client RPCs for damage number display
- Server-validated attribute upgrades

### Key Utility Class

`URageInMageAbilitySystemLibrary` (Blueprint Function Library) provides:
- Widget controller access helpers
- Default attribute initialization by character class
- Startup ability granting
- Hit type queries (critical/vulnerable/resistant)
- Combat queries: `GetLivePlayersWithinRadius()`, `IsFriendly()`, `IsBothEnemy()`
- XP reward calculation

### Interfaces

- **`ICombatInterface`** — combat socket locations, hit reaction montages, death state, attack montages, summon management
- **`IPlayerInterface`** — XP/level/point management and reward queries
- **`IEnemyInterface`** — highlight actors, combat target tracking

### Other Systems

- **`ARageInMageSphereProjectile`** — sphere collision, projectile movement, Niagara impact effects, 15s lifespan
- **`ARageInMageEffectActor`** — overlap-triggered gameplay effects with configurable apply/remove policies
- **`ARageInMageAIController`** — behavior tree + blackboard for enemy AI
- **`URageInMageAssetManager`** — custom asset manager (singleton, registered in DefaultEngine.ini)
- **MMCs:** `MMC_MaxHealth`, `MMC_MaxMana` — modifier magnitude calculators for derived stats
- **`UWaitCooldownChange`** — async task for cooldown UI updates

## Conventions

- All C++ classes use the `RageInMage` prefix (e.g., `ARageInMageCharacterBase`, `URageInMageAttributeSet`)
- Gameplay tags follow dot notation: `Abilities.Fire.SearingFlame`, `DamageType.Magical.Fire`
- Attributes use British spelling for defence: `PhysicalDefence`, `MagicalDefence`
- Character classes are defined in `ECharacterClass` enum (21 entries including mage variants, Summoner/Summon, MiniBoss/Boss)
- Heavy use of Blueprint-exposed properties (`UPROPERTY(EditAnywhere, BlueprintReadOnly)`) for data-driven configuration
- Delegate-based communication between systems (no direct coupling between UI and gameplay)
