// Copyright Reaplays


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/RageInMagePlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	URageInMageAttributeSet* LocalRageAS = GetRageAS();
	check(LocalRageAS);
	
	OnHealthChanged.Broadcast(LocalRageAS->GetHealth());
	OnMaxHealthChanged.Broadcast(LocalRageAS->GetMaxHealth());
	OnManaChanged.Broadcast(LocalRageAS->GetMana());
	OnMaxManaChanged.Broadcast(LocalRageAS->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	ARageInMagePlayerState* PS = GetRagePS();
	URageInMageAbilitySystemComponent* ASC = GetRageASC();
	URageInMageAttributeSet* AS = GetRageAS();

	check(PS);
	check(ASC);
	check(AS);

	// Bind to player state XP/Level changes
	PS->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXpChanged);
	PS->OnLevelUpDelegate.AddLambda([this](int32 NewLevel)
	{
		OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
	});

	// Bind to attribute changes
	ASC->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		});

	ASC->GetGameplayAttributeValueChangeDelegate(AS->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		});

	ASC->GetGameplayAttributeValueChangeDelegate(AS->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		});

	ASC->GetGameplayAttributeValueChangeDelegate(AS->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		});

	// Bind to abilities being granted (for spell globe icons on the overlay)
	if (ASC->bStartupAbilitiesGiven)
	{
		OnInitialiseStartUpAbilities(ASC);
	}
	else
	{
		ASC->AbilitiesGivenDelegate.AddLambda([this, ASC]()
		{
			OnInitialiseStartUpAbilities(ASC);
		});
	}

	// Bind to gameplay effect messages (potions, etc.)
	ASC->EffectAssetTags.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag : AssetTags)
			{
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
				if (Tag.MatchesTag(MessageTag))
				{
					const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
					if (Row)
					{
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				}
			}
		});
}

void UOverlayWidgetController::OnXpChanged(int32 NewXP)
{
	if (RagePS)
	{
		ULevelUpInfo* LevelUpInfo = RagePS->LevelUpInfo;
		checkf(LevelUpInfo, TEXT("Unable to find LevelUpInfo, Please fill out MagePlayerState Blueprint"));
		
		const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
		const int32 MaxLevel = LevelUpInfo->LevelUpInfos.Num();

		if (Level <= MaxLevel && Level > 0)
		{
			const int32 LevelUpRequirement = LevelUpInfo->LevelUpInfos[Level].LevelUpRequirement;
			const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInfos[Level - 1].LevelUpRequirement;

			const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
			const int32 XPToNextLevel = NewXP - PreviousLevelUpRequirement;

			TargetXPPercent = static_cast<float>(XPToNextLevel) / static_cast<float>(DeltaLevelRequirement);

			// Start the interpolation timer if not already running
			UWorld* World = PlayerController ? PlayerController->GetWorld() : GetWorld();
			if (World && !World->GetTimerManager().IsTimerActive(XPInterpTimerHandle))
			{
				World->GetTimerManager().SetTimer(XPInterpTimerHandle, this, &UOverlayWidgetController::InterpXPPercent, 0.016f, true);
			}
		}
	}
}

void UOverlayWidgetController::InterpXPPercent()
{
	if (FMath::IsNearlyEqual(CurrentXPPercent, TargetXPPercent, 0.001f))
	{
		CurrentXPPercent = TargetXPPercent;
		OnXPPercentChangedDelegate.Broadcast(CurrentXPPercent);

		// Stop the timer since we've reached the target
		UWorld* World = PlayerController ? PlayerController->GetWorld() : GetWorld();
		if (World)
		{
			World->GetTimerManager().ClearTimer(XPInterpTimerHandle);
		}
	}
	else
	{
		CurrentXPPercent = FMath::FInterpTo(CurrentXPPercent, TargetXPPercent, 0.016f, XPInterpSpeed);
		OnXPPercentChangedDelegate.Broadcast(CurrentXPPercent);
	}
}

void UOverlayWidgetController::OnInitialiseStartUpAbilities(URageInMageAbilitySystemComponent* InRageInMageAbilitySystemComponent)
{
	if (!InRageInMageAbilitySystemComponent || !InRageInMageAbilitySystemComponent->bStartupAbilitiesGiven) return;
	if (!AbilityInfo) return;

	FForEachAbilitySpec BroadcastDelegate;
	BroadcastDelegate.BindLambda([this, InRageInMageAbilitySystemComponent](const FGameplayAbilitySpec& AbilitySpec)
	{
		FRageInMageAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(
			URageInMageAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = URageInMageAbilitySystemComponent::GetInputTagFromSpec(AbilitySpec);
		Info.AbilityTypeTag = URageInMageAbilitySystemComponent::GetAbilityTypeTagFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
	});
	InRageInMageAbilitySystemComponent->ForEachAbilitySpec(BroadcastDelegate);
}
