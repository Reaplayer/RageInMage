// Copyright Reaplays


#include "AbilitySystem/RageInMageAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "RageInMageGameplayTags.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MagePlayerController.h"

URageInMageAttributeSet::URageInMageAttributeSet()
{
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

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void URageInMageAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data,
	FEffectProperties& Properties) const
{
	// Source = causer of the effect, Target = Target of the effect (Owner of this AS)
	
	Properties.EffectContextHandle = Data.EffectSpec.GetContext();
	
	Properties.SourceASC = Properties.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Properties.SourceASC) && Properties.SourceASC->AbilityActorInfo.IsValid() && Properties.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
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
		Properties.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Properties.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();

		//Checking whether this is an AI controller
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

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	};
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalDamage = GetIncomingDamage();
		SetIncomingDamage(0.f);
		if (LocalDamage > 0.f)
		{
			const float NewHealth = GetHealth() - LocalDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
			const bool bFatal = NewHealth <= 0.f;
			if (bFatal)
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

			ShowFloatingText(Properties, LocalDamage);
		}
	}
}

void URageInMageAttributeSet::ShowFloatingText(const FEffectProperties& Properties, float Damage) const
{
	if (Properties.SourceCharacter != Properties.TargetCharacter)
	{
		if (AMagePlayerController* PC = Cast<AMagePlayerController>(UGameplayStatics::GetPlayerController(Properties.SourceController, 0)))
		{
			PC->ShowDamageNumber(Damage, Properties.TargetCharacter);
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