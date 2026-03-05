// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "RageInMageDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class RAGEINMAGE_API URageInMageDamageGameplayAbility : public URageInMageGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	/** Apply knockback to a target away from an origin point. Uses KnockbackStrength and KnockbackUpwardForce. */
	UFUNCTION(BlueprintCallable, Category = "Damage|Knockback")
	void ApplyKnockback(AActor* Target, const FVector& Origin) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypeTags;

	/** Knockback force applied to targets on hit. 0 = no knockback. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|Knockback")
	FScalableFloat KnockbackStrength;

	/** Upward component of knockback (0-1). 0.3 = slight pop-up, 1.0 = mostly vertical. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|Knockback")
	float KnockbackUpwardForce = 0.3f;

	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontageArray) const;
};