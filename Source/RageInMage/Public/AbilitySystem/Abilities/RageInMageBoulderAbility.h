// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "RageInMageBoulderAbility.generated.h"

class ARageInMageBoulder;

/**
 * The Boulder — the Earth mage's Ultimate. Summons a boulder, plants the caster on top of it, and
 * hands the movement stick over to steering. It rolls until it hits something or times out.
 *
 * The ability owns the RIDE, not the boulder: bailing out (or the boulder ending) finishes the
 * ability, but the boulder itself keeps rolling under its own power after a dismount — so the
 * ability ends on the boulder's destruction, not on the dismount.
 *
 * Stance CUTS grow-time (design: -15/30/50%), so the stance bonuses here are NEGATIVE and the
 * scalar multiplies GrowthDuration — a higher stance means it reaches full size sooner.
 *
 * Blueprint usage:
 *   ActivateAbility -> CommitAbility -> SummonBoulder
 *   (bind an input to DismountBoulder to bail early; the ability ends itself when the boulder dies)
 */
UCLASS()
class RAGEINMAGE_API URageInMageBoulderAbility : public URageInMageDamageGameplayAbility
{
	GENERATED_BODY()

public:
	/** Spawn the boulder in front of the caster, mount them, and start steering. Authority only. */
	UFUNCTION(BlueprintCallable, Category = "Boulder")
	ARageInMageBoulder* SummonBoulder();

	/** Bail out early. The boulder keeps rolling; the ability ends when it finally stops. */
	UFUNCTION(BlueprintCallable, Category = "Boulder")
	void DismountBoulder();

	UFUNCTION(BlueprintPure, Category = "Boulder")
	ARageInMageBoulder* GetActiveBoulder() const { return ActiveBoulder; }

	/** Grow-time scalar from the caster's stance (below 1 — a higher stance grows faster). */
	UFUNCTION(BlueprintPure, Category = "Boulder")
	float GetGrowthTimeScalar() const;

protected:
	/** The boulder class to spawn. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boulder")
	TSubclassOf<ARageInMageBoulder> BoulderClass;

	/** How far in front of the caster the boulder appears. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boulder")
	float SpawnForwardOffset = 250.f;

	/** Seconds of rolling to reach full size, before the stance scalar is applied. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boulder")
	float GrowthDuration = 6.f;

	/** How long the boulder lasts before crumbling, rider or not. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boulder")
	float MaxLifetime = 12.f;

	// ── Immovable Mass stance scaling (grow-time REDUCTION — design: -15/30/50%) ──

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boulder|ImmovableMass")
	float StanceBonusStage1 = -15.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boulder|ImmovableMass")
	float StanceBonusStage2 = -30.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boulder|ImmovableMass")
	float StanceBonusStage3 = -50.f;

private:
	/** Ends the ability once the boulder is gone (which may be long after a dismount). */
	UFUNCTION()
	void OnBoulderDestroyed(AActor* DestroyedActor);

	UPROPERTY()
	TObjectPtr<ARageInMageBoulder> ActiveBoulder;
};
