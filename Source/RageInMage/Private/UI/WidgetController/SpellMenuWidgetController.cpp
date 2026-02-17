// Copyright Reaplays

#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/RageInMagePlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	ARageInMagePlayerState* PS = GetRagePS();
	check(PS);
	
	BroadcastAbilityInfo();	
	URageInMageAbilitySystemComponent* ASC = GetRageASC();
	check(ASC);

	if (ASC->bStartupAbilitiesGiven)
	{
		BroadcastAllAbilities();
	}
	SpellPointsChangedDelegate.Broadcast(PS->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	URageInMageAbilitySystemComponent* ASC = GetRageASC();
	check(ASC);
	check(AbilityInfo);

	// Wait for abilities to be granted if not already
	if (!ASC->bStartupAbilitiesGiven)
	{
		ASC->AbilitiesGivenDelegate.AddUObject(this, &USpellMenuWidgetController::BroadcastAllAbilities);
	}
}

void USpellMenuWidgetController::SpendSpellPoint(const FGameplayTag& AbilityTag)
{
	URageInMageAbilitySystemComponent* ASC = GetRageASC();
	check(ASC);

	ASC->SpendSpellPoint(AbilityTag);
}

void USpellMenuWidgetController::BroadcastAllAbilities()
{
	URageInMageAbilitySystemComponent* ASC = GetRageASC();
	if (!ASC || !ASC->bStartupAbilitiesGiven) return;

	FForEachAbilitySpec BroadcastDelegate;
	BroadcastDelegate.BindLambda([this, ASC](const FGameplayAbilitySpec& AbilitySpec)
	{
		FRageInMageAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(ASC->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = ASC->GetInputTagFromSpec(AbilitySpec);
		Info.AbilityTypeTag = ASC->GetAbilityTypeTagFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
	});

	ASC->ForEachAbilitySpec(BroadcastDelegate);
}