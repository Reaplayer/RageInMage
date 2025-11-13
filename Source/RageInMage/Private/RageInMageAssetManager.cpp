// Copyright Reaplays


#include "RageInMageAssetManager.h"
#include "RageInMageGameplayTag.h"
#include "AbilitySystemGlobals.h"

URageInMageAssetManager& URageInMageAssetManager::Get()
{
	check(GEngine);
	
	URageInMageAssetManager* RageInMageAssetManager = Cast<URageInMageAssetManager>(GEngine->AssetManager);
	return *RageInMageAssetManager;
}

void URageInMageAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FRageInMageGameplayTag::InitializeNativeGameplayTags();

	// This is required to use Target Data!
	UAbilitySystemGlobals::Get().InitGlobalData();
}
