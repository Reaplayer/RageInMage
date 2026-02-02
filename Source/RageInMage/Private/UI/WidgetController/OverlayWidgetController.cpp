// Copyright Reaplays


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Interaction/CombatInterface.h"
#include "Player/RageInMagePlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const URageInMageAttributeSet* MageAttributeSet = CastChecked<URageInMageAttributeSet>(AttributeSet);
	OnHealthChanged.Broadcast(MageAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(MageAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(MageAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(MageAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	if (RagePlayerState)
	{
		RagePlayerState->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXpChanged);
		RagePlayerState->OnLevelUpDelegate.AddLambda([this](int32 NewLevel) { OnPlayerLevelChangedDelegate.Broadcast(NewLevel); });
	}
	
	if (AbilitySystemComponent)
	{
		// Health Delegate Dependency
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(RageAS->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data){OnHealthChanged.Broadcast(Data.NewValue);});
	
	
		// Max Health Delegate Dependency
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(RageAS->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data){OnMaxHealthChanged.Broadcast(Data.NewValue);});
	
	
		// Mana Delegate Dependency
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(RageAS->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data){OnManaChanged.Broadcast(Data.NewValue);});
	
	
		// Max Mana Delegate Dependency
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(RageAS->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data){OnMaxManaChanged.Broadcast(Data.NewValue);});
	}
	
	if (RageASC)
	{
		if (RageASC->bStartupAbilitiesGiven)
		{
			OnInitialiseStartUpAbilities(RageASC);
		}
		else
		{
			RageASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnInitialiseStartUpAbilities);
		}
		RageASC->EffectAssetTags.AddLambda(
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
	RageWidgetController->SetOverlayWidgetController(this);
}

void UOverlayWidgetController::OnInitialiseStartUpAbilities(
	URageInMageAbilitySystemComponent* InRageInMageAbilitySystemComponent)
{
	if (!InRageInMageAbilitySystemComponent->bStartupAbilitiesGiven) return;
	
	// Broadcast Class Visuals
	if (AActor* AvatarActor = AbilitySystemComponent->GetAvatarActor())
	{
		if (UCharacterClassInfo* CharacterClassInfo = URageInMageAbilitySystemLibrary::GetCharacterClassInfo(AvatarActor))
		{
			if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(AvatarActor))
			{
				const ECharacterClass CharacterClass = CombatInterface->Execute_GetCharacterClass(AvatarActor);
				const FCharacterClassDefaultInfo DefaultInfo = CharacterClassInfo->GetCharacterClassDefaultInfo(CharacterClass);

				// Store the style in MageWidgetController for late-joining widgets
				RageWidgetController->HandleBGXPStyleChanged(DefaultInfo.ProgressBarColor, DefaultInfo.BackGroundMaterialInstance);

				// Broadcast to the base MageWidgetController delegate so all child controllers can receive it
				RageWidgetController->OnSetBGXPStyle.Broadcast(DefaultInfo.ProgressBarColor, DefaultInfo.BackGroundMaterialInstance);
			}
		}
	}
	
	// Broadcast Ability Info
	FForEachAbilitySpec BroadCastDelegate;
	BroadCastDelegate.BindLambda([this, InRageInMageAbilitySystemComponent](const FGameplayAbilitySpec& AbilitySpec)
	{
		FRageInMageAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(InRageInMageAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = InRageInMageAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);
		Info.AbilityTypeTag = InRageInMageAbilitySystemComponent->GetAbilityTypeTagFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
	});
	InRageInMageAbilitySystemComponent->ForEachAbilitySpec(BroadCastDelegate);
}

void UOverlayWidgetController::OnXpChanged(int32 NewXP)
{
	if (RagePlayerState)
	{
		ULevelUpInfo* LevelUpInfo = RagePlayerState->LevelUpInfo;
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
			if (!GetWorld()->GetTimerManager().IsTimerActive(XPInterpTimerHandle))
			{
				GetWorld()->GetTimerManager().SetTimer(XPInterpTimerHandle, this, &UOverlayWidgetController::InterpXPPercent, 0.016f, true);
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
		GetWorld()->GetTimerManager().ClearTimer(XPInterpTimerHandle);
	}
	else
	{
		CurrentXPPercent = FMath::FInterpTo(CurrentXPPercent, TargetXPPercent, 0.016f, XPInterpSpeed);
		OnXPPercentChangedDelegate.Broadcast(CurrentXPPercent);
	}
}
