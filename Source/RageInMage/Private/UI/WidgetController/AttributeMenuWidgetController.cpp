// Copyright Reaplays


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "Player/MagePlayerState.h"
#include "AbilitySystem/Data/AttributeInfo.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	URageInMageAttributeSet* AS = CastChecked<URageInMageAttributeSet>(AttributeSet);

	check(AttributeInfo);
	
	for (auto& Pair : AS->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda
		([this, Pair](const FOnAttributeChangeData& Data)
		{
			BroadcastAttributeInfo(Pair.Key, Pair.Value());
		}
		);
	}
	
	AMagePlayerState* MagePlayerState = CastChecked<AMagePlayerState>(PlayerState);
	MagePlayerState->OnAttributePointsChangedDelegate.AddLambda([this](int32 Points){ AttributePointsChangedDelegate.Broadcast(Points); });
	MagePlayerState->OnSpellPointsChangedDelegate.AddLambda([this](int32 Points){ SpellPointsChangedDelegate.Broadcast(Points); });
}

void UAttributeMenuWidgetController::BroadcastInitalValues()
{
	URageInMageAttributeSet* AS = CastChecked<URageInMageAttributeSet>(AttributeSet);

	check(AttributeInfo);
	
	for (auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
	
	AMagePlayerState* MagePlayerState = CastChecked<AMagePlayerState>(PlayerState);
	AttributePointsChangedDelegate.Broadcast(MagePlayerState->GetAttributePoints());
	SpellPointsChangedDelegate.Broadcast(MagePlayerState->GetSpellPoints());
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	URageInMageAbilitySystemComponent* RageInMageAbilitySystemComponent = CastChecked<URageInMageAbilitySystemComponent>(AbilitySystemComponent);
	RageInMageAbilitySystemComponent->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
                                                            const FGameplayAttribute& Attribute)
{
	FRageInMageAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = 	Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
