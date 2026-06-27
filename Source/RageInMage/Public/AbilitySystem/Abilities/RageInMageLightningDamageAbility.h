// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "RageInMageLightningDamageAbility.generated.h"

class UNiagaraSystem;

/**
 * Shared base for Lightning-school damage abilities that want chain-lightning behavior.
 * Subclasses call CauseChainDamage(InitialTarget) instead of the inherited CauseDamage() to route
 * their hit through URageInMageAbilitySystemLibrary::ApplyChainLightningDamage, which jumps the hit
 * onward to nearby enemies as long as the target it just left is Charged. Every future Lightning
 * spell that wants chaining should inherit from this class and call CauseChainDamage() the same way.
 */
UCLASS()
class RAGEINMAGE_API URageInMageLightningDamageAbility : public URageInMageDamageGameplayAbility
{
	GENERATED_BODY()

protected:
	/** Builds this ability's damage spec (DamageEffectClass + DamageTypeTags, same as the inherited
	 *  CauseDamage) and routes it through the chain-lightning helper instead of a single apply. */
	UFUNCTION(BlueprintCallable, Category = "Damage|Lightning")
	void CauseChainDamage(AActor* InitialTarget);

	/** Max distance a chain jump will travel to find its next target. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|Lightning")
	float ChainJumpRadius = 350.f;

	/** Cumulative damage falloff applied per jump (0.1 = each jump deals 10% less than the last).
	 *  A jump leaving an OverCharged target always chains at full damage instead. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|Lightning")
	float ChainDamageFalloffPerJump = 0.1f;

	/** Max number of jumps a single chain can make. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|Lightning")
	int32 ChainMaxJumps = 10;

	/** Niagara system spawned for each successful jump, from the target just left to the next one.
	 *  Expects a "BeamEnd" Position user parameter (absolute world space), same convention as
	 *  RageInMageLightningFlashAbility's RodImpactEffect. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|Lightning")
	TObjectPtr<UNiagaraSystem> ChainImpactEffect;
};
