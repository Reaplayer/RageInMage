// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "RageInMageMeteorAbility.generated.h"

class ARageInMageFireZone;
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

	/** Fire zone class to spawn on meteor impact. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	TSubclassOf<ARageInMageFireZone> FireZoneClass;

	/** Separate damage GE for fire zone DoT (optional; if null, uses DamageEffectClass). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor|FireZone")
	TSubclassOf<UGameplayEffect> FireZoneDamageEffectClass;

	/** Separate damage tags for fire zone DoT (optional; if empty, uses main DamageTypeTags). */
	UPROPERTY(EditDefaultsOnly, Category = "Meteor|FireZone")
	TMap<FGameplayTag, FScalableFloat> FireZoneDamageTypeTags;

	/** Effect to play at each meteor impact. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor|VFX")
	TObjectPtr<UNiagaraSystem> MeteorImpactEffect;

	/** Sound to play on each meteor impact. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor|SFX")
	TObjectPtr<USoundBase> MeteorImpactSound;

private:
	void SpawnNextMeteor();
	void SpawnMeteorAtLocation(const FVector& ImpactLocation);
	FGameplayEffectSpecHandle MakeDamageSpec() const;

	FVector MeteorRainCenter;
	int32 MeteorsRemaining = 0;
	FTimerHandle MeteorStaggerTimerHandle;
};
