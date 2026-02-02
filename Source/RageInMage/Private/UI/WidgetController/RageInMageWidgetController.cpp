// Copyright Reaplays


#include "UI/WidgetController/RageInMageWidgetController.h"
#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAttributeSet.h"

void URageInMageWidgetController::SetWidgetControllerParams(const FRageInMageWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
	if (URageInMageAbilitySystemComponent* InRageASC = CastChecked<URageInMageAbilitySystemComponent>(AbilitySystemComponent))
	{
		RageASC = InRageASC;
	}
	if (URageInMageAttributeSet* InRageAS = CastChecked<URageInMageAttributeSet>(AttributeSet))
	{
		RageAS = InRageAS;
	}
	if (ARageInMagePlayerState* InMagePlayerState = CastChecked<ARageInMagePlayerState>(PlayerState))
	{
		RagePlayerState = InMagePlayerState;
	}
}

void URageInMageWidgetController::BroadcastInitialValues()
{
	
}

void URageInMageWidgetController::BindCallbacksToDependencies()
{
	SetRageWidgetController(this);
}

void URageInMageWidgetController::BroadcastStoredBGXPStyle()
{
	if (bHasStoredStyle)
	{
		OnSetBGXPStyle.Broadcast(StoredProgressBarColor, StoredBackgroundMaterial);
	}
}

void URageInMageWidgetController::HandleBGXPStyleChanged(const FSlateColor& ProgressBarColor,
	UMaterialInstance* BackgroundMaterial)
{
	StoredProgressBarColor = ProgressBarColor;
	StoredBackgroundMaterial = BackgroundMaterial;
	bHasStoredStyle = true;
}
