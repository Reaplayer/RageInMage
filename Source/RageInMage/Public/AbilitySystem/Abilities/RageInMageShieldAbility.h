// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageGameplayAbility.h"
#include "RageInMageShieldAbility.generated.h"

class UNiagaraSystem;

/**
 * Shield ability — absorbs one hit of damage, then converts the absorbed amount into
 * healing for the caster and nearby friendlies.
 * Used for Soothing Bubble (Water Defence), and reusable for any absorb-then-heal shield.
 *
 * Blueprint workflow:
 * 1. ActivateAbility -> CommitAbility
 * 2. ActivateShield()
 * 3. Shield absorbs damage (via AttributeSet delegate) OR duration expires
 * 4. If absorbed: ApplyAbsorbHeal() -> heals caster + nearby friendlies
 * 5. EndAbility
 */
UCLASS()
class RAGEINMAGE_API URageInMageShieldAbility : public URageInMageGameplayAbility
{
	GENERATED_BODY()

public:
	/** Activate the shield. Applies the shield GE and starts the expiration timer. */
	UFUNCTION(BlueprintCallable, Category = "Shield")
	void ActivateShield();

protected:
	// ── Shield ──

	/** GE that grants Status.Shielded tag. Should be Infinite duration. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shield")
	TSubclassOf<UGameplayEffect> ShieldEffectClass;

	/** Maximum time the shield lasts before expiring on its own. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shield")
	float ShieldDuration = 10.f;

	/** Maximum number of damage instances the shield absorbs before breaking. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shield")
	int32 MaxAbsorbCount = 1;

	// ── Heal ──

	/** Instant GE that heals. Uses SetByCaller for magnitude. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shield|Heal")
	TSubclassOf<UGameplayEffect> HealEffectClass;

	/** SetByCaller tag used to pass the heal magnitude into HealEffectClass. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shield|Heal")
	FGameplayTag HealMagnitudeTag;

	/** Radius around caster to search for friendly targets to heal. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shield|Heal")
	float HealRadius = 500.f;

	/** How much of the absorbed damage is converted to healing. 1.0 = 100%. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shield|Heal")
	float HealConversionRate = 1.f;

	// ── VFX / SFX ──

	/** VFX burst when shield pops and heals. Spawned at caster location. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shield|VFX")
	TObjectPtr<UNiagaraSystem> HealBurstEffect;

	/** Sound when shield first forms. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shield|SFX")
	TObjectPtr<USoundBase> ShieldFormSound;

	/** Sound when shield pops (absorbs damage or expires). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shield|SFX")
	TObjectPtr<USoundBase> ShieldPopSound;

	// ── BP Events ──

	/** Called when shield is activated. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shield")
	void OnShieldActivated();

	/** Called when shield expires without absorbing a hit. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shield")
	void OnShieldExpired();

	/** Called when shield absorbs damage. Passes total absorbed amount before conversion. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shield")
	void OnShieldAbsorbed(float AbsorbedDamage);

	/** Called each time the shield absorbs a hit (before it breaks). For per-hit VFX feedback. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shield")
	void OnShieldHitAbsorbed(int32 HitNumber, float DamageAbsorbed);

private:
	void HandleShieldAbsorbed(float AbsorbedAmount);
	void HandleShieldExpired();
	void RemoveShieldEffect();
	void ApplyAbsorbHeal(float AbsorbedAmount);

	UPROPERTY()
	FActiveGameplayEffectHandle ActiveShieldEffectHandle;

	FTimerHandle ShieldExpirationTimerHandle;
	FDelegateHandle ShieldAbsorbDelegateHandle;

	int32 CurrentAbsorbCount = 0;
	float TotalAbsorbedDamage = 0.f;
};
