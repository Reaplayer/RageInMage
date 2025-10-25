// Copyright Reaplays


#include "UI/Widget/MageUserWidget.h"

void UMageUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
