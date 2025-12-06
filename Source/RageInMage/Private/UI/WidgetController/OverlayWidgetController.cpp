// Copyright Reaplays


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

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
	const URageInMageAttributeSet* MageAttributeSet = CastChecked<URageInMageAttributeSet>(AttributeSet);

	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MageAttributeSet->GetHealthAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data)
		{
		OnHealthChanged.Broadcast(Data.NewValue);
		}
	);
	
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MageAttributeSet->GetMaxHealthAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data)
		{
		OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);
	
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MageAttributeSet->GetManaAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data)
		{
		OnManaChanged.Broadcast(Data.NewValue);
		}
	);
	
	
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

				CachedBGXPMaterialInstance = DefaultInfo.BackGroundMaterialInstance;
				OnSetBGXPStyle.Broadcast(DefaultInfo.ProgressBarColor, CachedBGXPMaterialInstance);
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
		AbilityInfoDelegate.Broadcast(Info, CachedBGXPMaterialInstance);
	});
	RageInMageAbilitySystemComponent->ForEachAbilitySpec(BroadCastDelegate);
}
