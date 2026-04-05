// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "RageInMageSurfAbility.generated.h"

class ARageInMageZone;
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

	/** Collision sweep radius for hitting enemies during surf. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf|Damage")
	float SurfCollisionRadius = 150.f;

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

	// ── VFX/SFX ──

	/** Looping sound while surfing. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surf|SFX")
	TObjectPtr<USoundBase> SurfLoopSound;

private:
	void SurfTick();
	void SurfDamageCheck();
	void SpawnTrailSegment(const FVector& Location);
	FGameplayEffectSpecHandle MakeSurfDamageSpec() const;
	FGameplayEffectSpecHandle MakeTrailDamageSpec() const;

	FVector SurfDirection = FVector::ZeroVector;
	bool bIsSurfing = false;
	float SurfTimeRemaining = 0.f;
	FVector LastTrailSpawnLocation = FVector::ZeroVector;

	FTimerHandle SurfTickTimerHandle;

	/** Track enemies already damaged this surf to prevent multi-hit on the same enemy. */
	UPROPERTY()
	TSet<TObjectPtr<AActor>> DamagedEnemies;

	UPROPERTY()
	TObjectPtr<UAudioComponent> SurfSoundComponent;
};
