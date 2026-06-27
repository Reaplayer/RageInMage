// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageGameplayAbility.h"
#include "RageInMageFlashAndAweAbility.generated.h"

class UNiagaraSystem;
class USoundBase;

/**
 * Flash And Awe (Lightning CrowdControl) — teleports the caster to a target point ("Flash"), then
 * stuns every enemy within StunRadius of the landing point ("...and Awe"). Deals no damage and
 * bypasses the Charge/OverCharged system entirely via
 * URageInMageAbilitySystemLibrary::ApplyGuaranteedStun, which also strips any existing
 * Condition.StunImmune so this guaranteed stun always lands.
 */
UCLASS()
class RAGEINMAGE_API URageInMageFlashAndAweAbility : public URageInMageGameplayAbility
{
	GENERATED_BODY()

public:
	/** Teleports to TargetLocation (clamped to MaxSpellRange, validated via TeleportTo so the caster
	 *  never lands inside solid geometry), then stuns every enemy within StunRadius of wherever the
	 *  caster actually ends up. Called from Blueprint (e.g. on input released). */
	UFUNCTION(BlueprintCallable, Category = "FlashAndAwe")
	void ApplyFlashAndAwe(const FVector& TargetLocation);

protected:
	/** How long the guaranteed stun lasts. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FlashAndAwe")
	FScalableFloat StunDuration;

	/** Grace period of StunImmunity granted on top of StunDuration (see ApplyGuaranteedStun) — kept
	 *  as its own tunable, separate from StunDuration, so a future item/Tenacity stat can reduce just
	 *  this without touching how long the stun itself lasts. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FlashAndAwe")
	float StunImmunityGraceSeconds = 4.f;

	/** Radius around the landing point that gets stunned. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FlashAndAwe")
	FScalableFloat StunRadius;

	/** Niagara effect spawned at the departure point. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FlashAndAwe|VFX")
	TObjectPtr<UNiagaraSystem> FlashDepartEffect;

	/** Niagara effect spawned at the landing point — expects a "StunRadius" float user parameter. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FlashAndAwe|VFX")
	TObjectPtr<UNiagaraSystem> FlashLandEffect;

	/** Sound played on landing. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FlashAndAwe|SFX")
	TObjectPtr<USoundBase> FlashSound;
};
