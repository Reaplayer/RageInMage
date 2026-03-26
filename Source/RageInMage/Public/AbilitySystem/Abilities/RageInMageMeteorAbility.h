// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "RageInMageMeteorAbility.generated.h"

class ARageInMageZone;
class UNiagaraSystem;

/**
 * Meteor rain ability - summons multiple meteors that rain from the sky in a target area.
 * Each meteor deals AoE damage on impact and leaves a fire zone on the ground.
 */
UCLASS()
class RAGEINMAGE_API URageInMageMeteorAbility : public URageInMageDamageGameplayAbility
{
	GENERATED_BODY()

public:
	/** Begin summoning meteors at the target location. Called from Blueprint. */
	UFUNCTION(BlueprintCallable, Category = "Meteor")
	void BeginMeteorRain(const FVector& TargetCenter);

protected:
	/** Toggle debug visualization of all spell radii and trajectories. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor|Debug")
	bool bShowDebug = false;
	/** Number of meteors to spawn. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	FScalableFloat MeteorCount;

	/** Radius of the overall rain area. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	FScalableFloat RainAreaRadius;

	/** Radius of each individual meteor explosion. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	FScalableFloat MeteorExplosionRadius;

	/** Time between each meteor impact (stagger). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	float MeteorStaggerDelay = 0.4f;

	/** How long each fire zone lasts after a meteor impact. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	float FireZoneDuration = 4.f;

	/** Damage tick interval for the fire zones left by meteors. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	float FireZoneTickInterval = 1.f;

	/** Zone class to spawn on meteor impact (FireZone, PoisonZone, etc.). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	TSubclassOf<ARageInMageZone> ZoneClass;

	/** Separate damage GE for fire zone DoT (optional; if null, uses DamageEffectClass). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor|FireZone")
	TSubclassOf<UGameplayEffect> FireZoneDamageEffectClass;

	/** Separate damage tags for fire zone DoT (optional; if empty, uses main DamageTypeTags). */
	UPROPERTY(EditDefaultsOnly, Category = "Meteor|FireZone")
	TMap<FGameplayTag, FScalableFloat> FireZoneDamageTypeTags;

	/** How high above the impact point the meteor VFX spawns before falling down. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor|VFX")
	float MeteorSpawnHeight = 2000.f;

	/** Travel speed of the meteor (units/sec). Combined with distance to determine travel time. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor|VFX")
	float MeteorSpeed = 4000.f;

	/** Effect to play at each meteor impact. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor|VFX")
	TObjectPtr<UNiagaraSystem> MeteorImpactEffect;

	/** Sound to play on each meteor impact. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor|SFX")
	TObjectPtr<USoundBase> MeteorImpactSound;

private:
	void SpawnNextMeteor();
	void SpawnMeteorAtLocation(const FVector& ImpactLocation);
	void OnMeteorImpactComplete();
	FGameplayEffectSpecHandle MakeDamageSpec() const;
	FGameplayEffectSpecHandle MakeFireZoneDamageSpec() const;

	/** Pick a random enemy from TargetableEnemies whose location is within the rain area.
	 *  Removes the chosen enemy (and any out-of-range enemies encountered) from the array.
	 *  Returns nullptr if no valid target remains. */
	AActor* PickAndConsumeRandomTarget();

	FVector MeteorRainCenter;
	int32 MeteorsRemaining = 0;
	int32 MeteorsPendingImpact = 0;
	FTimerHandle MeteorStaggerTimerHandle;

	/** Enemies found at cast time that haven't been targeted yet. One meteor per enemy max. */
	UPROPERTY()
	TArray<TObjectPtr<AActor>> TargetableEnemies;
};
