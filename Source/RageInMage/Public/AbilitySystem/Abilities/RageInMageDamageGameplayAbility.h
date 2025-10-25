// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageGameplayAbility.h"
#include "RageInMageDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class RAGEINMAGE_API URageInMageDamageGameplayAbility : public URageInMageGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypeTags;
};
