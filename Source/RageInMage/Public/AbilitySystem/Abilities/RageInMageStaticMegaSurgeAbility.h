// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "RageInMageStaticMegaSurgeAbility.generated.h"

class UNiagaraSystem;

/**
 * Static Megasurge - Lightning Defensive ability. Applies a self-buff (Resistance_Damage modifier +
 * Status.Reflecting tag) for Duration. While active, every hit that lands zaps the attacker back for
 * a flat (non-scaling) Lightning hit via the inherited CauseDamage - no lifesteal, no chaining onward.
 * Mirrors URageInMageShieldAbility's lifetime pattern: the ability stays alive for the buff's full
 * duration (bound to the AttributeSet's OnDamageReflected delegate) instead of ending immediately,
 * then cleans up and ends itself once the buff expires.
 */
UCLASS()
class RAGEINMAGE_API URageInMageStaticMegaSurgeAbility : public URageInMageDamageGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "StaticMegaSurge")
	void ApplyStaticMegaSurge();

protected:
	/** Self-buff GE: should grant Status.Reflecting (TargetTags GEComponent, set up by hand - see the
	 *  project's documented GEComponent tooling gotcha) and an Additive Resistance_Damage modifier. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StaticMegaSurge")
	TSubclassOf<UGameplayEffect> BuffEffectClass;

	/** How long the damage-reduction + reflect buff lasts. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StaticMegaSurge")
	FScalableFloat Duration;

	/** Niagara effect spawned on the attacker when they get zapped back. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StaticMegaSurge|VFX")
	TObjectPtr<UNiagaraSystem> ReflectEffect;

	/** Called each time the buff reflects a hit back at an attacker. For BP VFX/SFX feedback. */
	UFUNCTION(BlueprintImplementableEvent, Category = "StaticMegaSurge")
	void OnReflectProc(AActor* Attacker);

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	void HandleDamageReflected(AActor* Attacker);
	void HandleBuffExpired();
	void RemoveBuff();

	FActiveGameplayEffectHandle ActiveBuffEffectHandle;
	FDelegateHandle ReflectDelegateHandle;
	FTimerHandle BuffExpirationTimerHandle;
};
