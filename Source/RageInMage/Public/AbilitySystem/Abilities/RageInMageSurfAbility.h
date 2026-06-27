// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "RageInMageSurfAbility.generated.h"

class ARageInMageZone;
class ARageInMageWaveBox;
class UNiagaraSystem;

/**
 * Movement surf ability — propels the caster forward on a wave/slide.
 * Damages and pushes back enemies hit along the path.
 * Optionally leaves trail zone segments behind.
 * Used for Riding Tide (Water Movement), and reusable for Miasma Slide (Poison), etc.
 *
 * Blueprint workflow:
 * 1. ActivateAbility -> CommitAbility
 * 2. BeginSurf(Direction)
 * 3. OnSurfBegin (BP event): spawn wave VFX attached to caster
 * 4. OnSurfTick (BP event): update wave VFX each frame
 * 5. OnSurfEnd (BP event): destroy wave VFX, EndAbility
 */
UCLASS()
class RAGEINMAGE_API URageInMageSurfAbility : public URageInMageDamageGameplayAbility
{
	GENERATED_BODY()

public:
	/** Start surfing in the given world direction. Called from Blueprint. */
	UFUNCTION(BlueprintCallable, Category = "Surf")
	void BeginSurf(const FVector& Direction);

	/** Stop surfing early. Called from Blueprint or when duration expires. */
	UFUNCTION(BlueprintCallable, Category = "Surf")
	void EndSurf();

	/** Is the caster currently surfing? */
	UFUNCTION(BlueprintPure, Category = "Surf")
	bool IsSurfing() const { return bIsSurfing; }

	/** Called when surf begins. Override in BP to spawn wave VFX. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Surf")
	void OnSurfBegin(const FVector& StartLocation, const FVector& InSurfDirection);

	/** Called every tick during surf. Override in BP to update wave VFX position/rotation. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Surf")
	void OnSurfTick(float DeltaTime, const FVector& CurrentLocation, const FVector& CurrentDirection);

	/** Called when surf ends (duration or manual stop). Override in BP to destroy wave VFX. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Surf")
	void OnSurfEnd();

protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;

	/** Speed of the surf in units/sec. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf")
	FScalableFloat SurfSpeed;

	/** How long the surf lasts in seconds. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf")
	FScalableFloat SurfDuration;

	/** If true, the player can steer the surf direction with movement input (WASD/thumbstick). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf")
	bool bCanSteer = false;

	/** How quickly the surf direction blends toward movement input. Higher = snappier turns. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf", meta = (EditCondition = "bCanSteer", ClampMin = "0.0"))
	float SurfSteerRate = 5.f;

	/** Collision sweep radius for hitting enemies during surf. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf|Damage")
	float SurfCollisionRadius = 150.f;

	/** Interval between damage ticks on the same enemy (seconds). 0 = hit once only. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf|Damage")
	float SurfDamageTickInterval = 0.f;

	/** Pushback strength applied to enemies hit. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf|Damage")
	FScalableFloat PushbackStrength;

	/** GE applied to enemies hit (e.g. slow, wet). Optional. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf|Effects")
	TSubclassOf<UGameplayEffect> OnHitEffectClass;

	// ── Trail ──

	/** Zone class for trail segments left behind. If null, no trail. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf|Trail")
	TSubclassOf<ARageInMageZone> TrailZoneClass;

	/** Distance between trail segment spawns (units). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf|Trail")
	float TrailSpawnInterval = 200.f;

	/** Radius of each trail zone. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf|Trail")
	float TrailZoneRadius = 100.f;

	/** Duration of each trail zone. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf|Trail")
	float TrailZoneDuration = 3.f;

	/** Tick interval for trail zone damage (0 = no ticking damage). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf|Trail")
	float TrailZoneTickInterval = 1.f;

	/** Separate damage GE for trail zones. If null, uses DamageEffectClass. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf|Trail")
	TSubclassOf<UGameplayEffect> TrailDamageEffectClass;

	/** Separate damage tags for trail. If empty, uses main DamageTypeTags. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf|Trail")
	TMap<FGameplayTag, FScalableFloat> TrailDamageTypeTags;

	/** Niagara effect for trail zones (set on zone, overrides the zone's default). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf|Trail|VFX")
	TObjectPtr<UNiagaraSystem> TrailZoneEffect;

	/** Looping sound for trail zones. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf|Trail|SFX")
	TObjectPtr<USoundBase> TrailZoneLoopSound;

	// ── Wave Box ──

	/** Wave box actor class. If set, spawns an invisible box under the caster that rises/falls. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf|WaveBox")
	TSubclassOf<ARageInMageWaveBox> WaveBoxClass;

	/** Maximum height the wave lifts the caster (units). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf|WaveBox")
	float MaxWaveHeight = 200.f;

	/** Seconds to rise from ground to max height. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf|WaveBox")
	float WaveRiseDuration = 0.5f;

	/** Seconds to descend from max height back to ground (at end of surf). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf|WaveBox")
	float WaveDescendDuration = 0.8f;

	/** Half-extents of the wave box collision (X=forward, Y=side, Z=height). Thin and wide. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf|WaveBox")
	FVector WaveBoxHalfExtent = FVector(100.f, 30.f, 10.f);

	/** If true, taking damage during surf interrupts the ability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf|WaveBox")
	bool bInterruptOnHit = false;

	// ── VFX/SFX ──

	/** Looping sound while surfing. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf|SFX")
	TObjectPtr<USoundBase> SurfLoopSound;

private:
	void SurfTick();
	void SurfDamageCheck();
	void SpawnTrailSegment(const FVector& Location);
	FGameplayEffectSpecHandle MakeSurfDamageSpec() const;
	FGameplayEffectSpecHandle MakeSurfOnHitSpec() const;
	FGameplayEffectSpecHandle MakeTrailDamageSpec() const;

	/** Calculate the current wave height based on elapsed time (rise/cruise/descend phases). */
	float CalculateCurrentWaveHeight() const;

	/** Called when the caster receives a hit reaction tag during surf. */
	void OnCasterHitDuringSurf(const FGameplayTag Tag, int32 NewCount);

	FVector SurfDirection = FVector::ZeroVector;
	bool bIsSurfing = false;
	float SurfTimeRemaining = 0.f;
	FVector LastTrailSpawnLocation = FVector::ZeroVector;

	FTimerHandle SurfTickTimerHandle;

	/** Track last hit time per enemy for tick interval enforcement. */
	UPROPERTY()
	TMap<TObjectPtr<AActor>, double> DamagedEnemyTimestamps;

	UPROPERTY()
	TObjectPtr<UAudioComponent> SurfSoundComponent;

	// ── Wave Box state ──

	UPROPERTY()
	TObjectPtr<ARageInMageWaveBox> ActiveWaveBox;

	/** Total duration when surf started (for elapsed time calculation). */
	float SurfTotalDuration = 0.f;

	/** Ground-level Z when surf started (baseline for height offset). */
	float SurfStartZ = 0.f;

	/** Delegate handle for hit reaction tag event (used for bInterruptOnHit). */
	FDelegateHandle HitReactionDelegateHandle;
};
