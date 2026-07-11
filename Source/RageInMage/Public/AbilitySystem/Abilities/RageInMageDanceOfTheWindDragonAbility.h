// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "RageInMageDanceOfTheWindDragonAbility.generated.h"

/**
 * Dance of the Wind Dragon - Air Ultimate. On cast the mage becomes untargetable for Duration seconds and a
 * damage zone forms around the cast location. Over that window the mage lands NumAttacks evenly-spaced zone
 * pulses (damage + knockback to every enemy inside ZoneRadius), while cosmetic "afterimage" actors spawn every
 * AfterimageInterval seconds to sell the flurry.
 *
 * Untargetability is achieved by temporarily removing the caster's "Player" actor tag (enemy AI finds players
 * by that tag), restored when the ability ends. The afterimages are purely visual - all damage is done in C++
 * by the zone pulses. Damage/knockback are authority-gated.
 */
UCLASS()
class RAGEINMAGE_API URageInMageDanceOfTheWindDragonAbility : public URageInMageDamageGameplayAbility
{
	GENERATED_BODY()

public:
	URageInMageDanceOfTheWindDragonAbility();

	/** Starts the ultimate: untargetable + zone pulses + afterimages for Duration seconds. Call from the GA graph after CommitAbility. */
	UFUNCTION(BlueprintCallable, Category = "DanceOfTheWindDragon")
	void BeginDanceOfTheWindDragon();

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	/** Total duration of the dance (untargetable window). Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DanceOfTheWindDragon")
	FScalableFloat Duration;

	/** Number of zone-damage pulses spread evenly across Duration. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DanceOfTheWindDragon")
	int32 NumAttacks = 7;

	/** Radius of the damage zone around the cast location. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DanceOfTheWindDragon")
	FScalableFloat ZoneRadius;

	/** Seconds between cosmetic afterimage spawns. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DanceOfTheWindDragon")
	float AfterimageInterval = 0.5f;

	/** Cosmetic afterimage actor spawned at the caster every AfterimageInterval. Deals no damage; make it a
	 *  non-replicating visual BP. If null, no afterimages spawn. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DanceOfTheWindDragon")
	TSubclassOf<AActor> AfterimageActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DanceOfTheWindDragon|Debug")
	bool bShowDebug = false;

private:
	void DoZoneAttack();
	void SpawnAfterimage();
	void FinishDance();

	FTimerHandle AttackTimerHandle;
	FTimerHandle AfterimageTimerHandle;
	FTimerHandle DurationTimerHandle;

	int32 AttacksRemaining = 0;
	FVector ZoneCenter = FVector::ZeroVector;
	bool bRemovedPlayerTag = false;
};
