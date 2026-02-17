// Copyright Reaplays

#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/RageInMagePlayerState.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	URageInMageAttributeSet* AS = GetRageAS();
	ARageInMagePlayerState* PS = GetRagePS();

	check(AttributeInfo);
	check(AS);
	check(PS);

	// Broadcast attribute values on both server and client.
	// By the time a widget calls this, replicated attributes should be populated.
	for (auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}

	// Broadcast attribute/spell points
	AttributePointsChangedDelegate.Broadcast(PS->GetAttributePoints());
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	URageInMageAttributeSet* AS = GetRageAS();
	URageInMageAbilitySystemComponent* ASC = GetRageASC();
	ARageInMagePlayerState* PS = GetRagePS();

	check(AttributeInfo);
	check(AS);
	check(ASC);
	check(PS);

	// Bind to attribute value changes
	for (auto& Pair : AS->TagsToAttributes)
	{
		FDelegateHandle Handle = ASC->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			});
		AttributeChangeDelegateHandles.Add(Pair.Value(), Handle);
	}

	// Bind to attribute/spell point changes
	AttributePointsDelegateHandle = PS->OnAttributePointsChangedDelegate.AddLambda([this](int32 Points)
	{
		AttributePointsChangedDelegate.Broadcast(Points);
	});
}

void UAttributeMenuWidgetController::UnbindFromDependencies()
{
	if (URageInMageAbilitySystemComponent* ASC = GetRageASC())
	{
		for (auto& Pair : AttributeChangeDelegateHandles)
		{
			ASC->GetGameplayAttributeValueChangeDelegate(Pair.Key).Remove(Pair.Value);
		}
	}
	AttributeChangeDelegateHandles.Empty();

	if (ARageInMagePlayerState* PS = GetRagePS())
	{
		PS->OnAttributePointsChangedDelegate.Remove(AttributePointsDelegateHandle);
		PS->OnSpellPointsChangedDelegate.Remove(SpellPointsDelegateHandle);
	}
}

void UAttributeMenuWidgetController::BeginDestroy()
{
	UnbindFromDependencies();
	Super::BeginDestroy();
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	URageInMageAbilitySystemComponent* ASC = GetRageASC();
	check(ASC);

	ASC->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute)
{
	FRageInMageAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(GetRageAS());
	AttributeInfoDelegate.Broadcast(Info);
}
