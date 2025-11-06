// Copyright Reaplays


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "GameFramework/GameSession.h"

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

	
	Cast<URageInMageAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
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