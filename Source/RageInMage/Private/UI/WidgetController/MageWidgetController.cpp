// Copyright Reaplays


#include "UI/WidgetController/MageWidgetController.h"

void UMageWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UMageWidgetController::BroadcastInitalValues()
{
	
}

void UMageWidgetController::BindCallbacksToDependencies()
{
}
