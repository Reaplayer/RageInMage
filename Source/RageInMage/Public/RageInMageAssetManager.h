// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "RageInMageAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class RAGEINMAGE_API URageInMageAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static URageInMageAssetManager& Get();

protected:
	virtual void StartInitialLoading() override;
};
