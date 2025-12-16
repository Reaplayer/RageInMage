// Copyright Krymson Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

USTRUCT(BlueprintType)
struct FRageInMageLevelUpInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Up Info")
	int32 LevelUpRequirement = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Up Info")
	int32 AttributePointsReward = 2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Up Info")
	int32 SpellPointsReward = 1;
};

/**
 * 
 */
UCLASS()
class RAGEINMAGE_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Up Info")
	TArray<FRageInMageLevelUpInfo> LevelUpInfos;
	
	int32 FindLevelForXP(int32 XP) const;
};
