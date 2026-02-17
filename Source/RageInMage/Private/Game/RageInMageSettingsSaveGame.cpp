// Copyright Reaplays

#include "Game/RageInMageSettingsSaveGame.h"
#include "Kismet/GameplayStatics.h"

const FString URageInMageSettingsSaveGame::SlotName = TEXT("RageInMageSettings");

void URageInMageSettingsSaveGame::SaveSettings()
{
	UGameplayStatics::SaveGameToSlot(this, SlotName, 0);
}

URageInMageSettingsSaveGame* URageInMageSettingsSaveGame::LoadOrCreateSettings()
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		return Cast<URageInMageSettingsSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	}

	URageInMageSettingsSaveGame* NewSettings = Cast<URageInMageSettingsSaveGame>(
		UGameplayStatics::CreateSaveGameObject(URageInMageSettingsSaveGame::StaticClass()));
	NewSettings->SaveSettings();
	return NewSettings;
}
