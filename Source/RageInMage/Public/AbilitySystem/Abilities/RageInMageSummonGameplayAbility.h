// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageGameplayAbility.h"
#include "RageInMageSummonGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class RAGEINMAGE_API URageInMageSummonGameplayAbility : public URageInMageGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(blueprintCallable)
	TArray<FVector> GetSummonLocations(int32 InSummonAmount);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Summon")
	int32 SummonAmount = 5;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Summon")
	float MinSpawnDistance = 75.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Summon")
	float MaxSpawnDistance = 250.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Summon")
	float SpawnSpreadAngle = 90.f;
};
