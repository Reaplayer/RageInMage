// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RageInMageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class RAGEINMAGE_API URageInMageGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category="Input")
	FGameplayTag StartupInputTag;
};
