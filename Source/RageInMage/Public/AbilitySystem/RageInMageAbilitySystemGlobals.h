// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "RageInMageAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class RAGEINMAGE_API URageInMageAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
