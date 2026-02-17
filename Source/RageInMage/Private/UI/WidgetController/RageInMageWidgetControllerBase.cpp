// Copyright Reaplays

#include "UI/WidgetController/RageInMageWidgetControllerBase.h"
#include "Player/RageInMagePlayerController.h"
#include "Player/RageInMagePlayerState.h"
#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

void URageInMageWidgetControllerBase::SetGASReferences(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	PlayerController = PC;
	PlayerState = PS;
	AbilitySystemComponent = ASC;
	AttributeSet = AS;

	// Cache typed references (safe Cast — CastChecked would crash instead of returning null)
	RagePC = Cast<ARageInMagePlayerController>(PlayerController);
	RageASC = Cast<URageInMageAbilitySystemComponent>(AbilitySystemComponent);
	RageAS = Cast<URageInMageAttributeSet>(AttributeSet);
	RagePS = Cast<ARageInMagePlayerState>(PlayerState);
}

void URageInMageWidgetControllerBase::BindCallbacksToDependencies()
{
	// Override in subclass
}

void URageInMageWidgetControllerBase::BroadcastInitialValues()
{
	// Override in subclass
}

void URageInMageWidgetControllerBase::BroadcastClassVisuals()
{
	if (AActor* AvatarActor = AbilitySystemComponent->GetAvatarActor())
	{
		if (UCharacterClassInfo* CharacterClassInfo = URageInMageAbilitySystemLibrary::GetCharacterClassInfo(AvatarActor))
		{
			if (const ICombatInterface* CombatInterface = Cast<ICombatInterface>(AvatarActor))
			{
				const ECharacterClass CharacterClass = CombatInterface->Execute_GetCharacterClass(AvatarActor);
				const FCharacterClassDefaultInfo DefaultInfo = CharacterClassInfo->GetCharacterClassDefaultInfo(CharacterClass);

				// Store the style for late-joining widgets
				BGXPStyleDelegate.Broadcast(DefaultInfo.ProgressBarColor, DefaultInfo.BackGroundMaterialInstance);
			}
		}
	}
}

void URageInMageWidgetControllerBase::BroadcastAbilityInfo()
{
	if (!RageASC->bStartupAbilitiesGiven) return;
	FForEachAbilitySpec BroadCastDelegate;
	BroadCastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		FRageInMageAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(GetRageASC()->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = GetRageASC()->GetInputTagFromSpec(AbilitySpec);
		Info.AbilityTypeTag = GetRageASC()->GetAbilityTypeTagFromSpec(AbilitySpec);
		Info.ProgressionTag = GetRageASC()->GetAbilityProgressionTagFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
	});
	RageASC->ForEachAbilitySpec(BroadCastDelegate);
}

ARageInMagePlayerController* URageInMageWidgetControllerBase::GetRagePC()
{
	if (!RagePC)
	{
		RagePC = Cast<ARageInMagePlayerController>(PlayerController);
	}
	return RagePC;
}

ARageInMagePlayerState* URageInMageWidgetControllerBase::GetRagePS()
{
	if (!RagePS)
	{
		RagePS = Cast<ARageInMagePlayerState>(PlayerState);
	}
	return RagePS;
}

URageInMageAbilitySystemComponent* URageInMageWidgetControllerBase::GetRageASC()
{
	if (!RageASC)
	{
		RageASC = Cast<URageInMageAbilitySystemComponent>(AbilitySystemComponent);
	}
	return RageASC;
}

URageInMageAttributeSet* URageInMageWidgetControllerBase::GetRageAS()
{
	if (!RageAS)
	{
		RageAS = Cast<URageInMageAttributeSet>(AttributeSet);
	}
	return RageAS;
}