// Copyright Reaplays


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Interaction/CombatInterface.h"
#include "Player/MagePlayerState.h"

void UOverlayWidgetController::BroadcastInitalValues()
{
	const URageInMageAttributeSet* MageAttributeSet = CastChecked<URageInMageAttributeSet>(AttributeSet);
	OnHealthChanged.Broadcast(MageAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(MageAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(MageAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(MageAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	AMagePlayerState* MagePlayerState = CastChecked<AMagePlayerState>(PlayerState);
	MagePlayerState->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXpChanged);
	MagePlayerState->OnLevelUpDelegate.AddLambda([this](int32 NewLevel) { OnPlayerLevelChangedDelegate.Broadcast(NewLevel); });
	
	const URageInMageAttributeSet* MageAttributeSet = CastChecked<URageInMageAttributeSet>(AttributeSet);

	
	// Health Delegate Dependency
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MageAttributeSet->GetHealthAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data)
		{
		OnHealthChanged.Broadcast(Data.NewValue);
		}
	);
	
	
	// Max Health Delegate Dependency
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MageAttributeSet->GetMaxHealthAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data)
		{
		OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);
	
	
	// Mana Delegate Dependency
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MageAttributeSet->GetManaAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data)
		{
		OnManaChanged.Broadcast(Data.NewValue);
		}
	);
	
	
	// Max Mana Delegate Dependency
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MageAttributeSet->GetMaxManaAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data)
		{
		OnMaxManaChanged.Broadcast(Data.NewValue);	
		}
	);

	
	if (URageInMageAbilitySystemComponent* RageInMageAbilitySystemComponent = Cast<URageInMageAbilitySystemComponent>(AbilitySystemComponent))
	{
		if (RageInMageAbilitySystemComponent->bStartupAbilitiesGiven)
		{
			OnInitialiseStartUpAbilities(RageInMageAbilitySystemComponent);
		}
		else
		{
			RageInMageAbilitySystemComponent->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnInitialiseStartUpAbilities);
		}
		RageInMageAbilitySystemComponent->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				for (const FGameplayTag& Tag : AssetTags)
				{
					// For example, say that Tag = Message.Potion.Health
					//"Message.Potion.Health".MatchesTag("Message") will return True, "Message".MatchesTag("Message.Potion.Health) will return False.
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if (Tag.MatchesTag(MessageTag))
					{
						const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				}
			}
		);
	}
}

void UOverlayWidgetController::OnInitialiseStartUpAbilities(
	URageInMageAbilitySystemComponent* RageInMageAbilitySystemComponent)
{
	if (!RageInMageAbilitySystemComponent->bStartupAbilitiesGiven) return;
	
	// Broadcast Class Visuals
	if (AActor* AvatarActor = AbilitySystemComponent->GetAvatarActor())
	{
		if (UCharacterClassInfo* CharacterClassInfo = URageInMageAbilitySystemLibrary::GetCharacterClassInfo(AvatarActor))
		{
			if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(AvatarActor))
			{
				const ECharacterClass CharacterClass = CombatInterface->Execute_GetCharacterClass(AvatarActor);
				const FCharacterClassDefaultInfo DefaultInfo = CharacterClassInfo->GetCharacterClassDefaultInfo(CharacterClass);

				// Must be Set before Ability Info so it can be received
				OnSetBGXPStyle.Broadcast(DefaultInfo.ProgressBarColor, DefaultInfo.BackGroundMaterialInstance);
			}
		}
	}
	
	// Broadcast Ability Info
	FForEachAbilitySpec BroadCastDelegate;
	BroadCastDelegate.BindLambda([this, RageInMageAbilitySystemComponent](const FGameplayAbilitySpec& AbilitySpec)
	{
		FRageInMageAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(RageInMageAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = RageInMageAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);
		Info.AbilityTypeTag = RageInMageAbilitySystemComponent->GetAbilityTypeTagFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
	});
	RageInMageAbilitySystemComponent->ForEachAbilitySpec(BroadCastDelegate);
}

void UOverlayWidgetController::OnXpChanged(int32 NewXP)
{
	AMagePlayerState* MagePlayerState = CastChecked<AMagePlayerState>(PlayerState);
	ULevelUpInfo* LevelUpInfo = MagePlayerState->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("Unable to find LevelUpInfo, Please fill out MagePlayerState Blueprint"));
	
	const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInfos.Num();
	
	if (Level <= MaxLevel && Level > 0)
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInfos[Level].LevelUpRequirement;
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInfos[Level - 1].LevelUpRequirement;
		
		const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
		const int32 XPToNextLevel = NewXP - PreviousLevelUpRequirement;
		
		const float XPBarPercentage = static_cast<float>(XPToNextLevel) / static_cast<float>(DeltaLevelRequirement);
		OnXPPercentChangedDelegate.Broadcast(XPBarPercentage);
	}
}
