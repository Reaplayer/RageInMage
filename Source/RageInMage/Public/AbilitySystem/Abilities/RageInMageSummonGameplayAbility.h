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
	TArray<FVector> GetSummonLocations();
	
	UPROPERTY(EditDefaultsOnly, Category = "Summon")
	int32 SummonCount = 1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Summon")
	TSubclassOf<AActor> SummonClasses;
	
	UPROPERTY(EditDefaultsOnly, Category = "Summon")
	float MinSpawnDistance = 75.f;
	UPROPERTY(EditDefaultsOnly, Category = "Summon")
	float MaxSpawnDistance = 250.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Summon")
	float SpawnSpreadAngle = 90.f;
};
