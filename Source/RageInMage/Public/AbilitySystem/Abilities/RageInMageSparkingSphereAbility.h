// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "RageInMageSparkingSphereAbility.generated.h"

class ARageInMageSparkingSphereZone;
class UNiagaraSystem;

/**
 * Sparking Sphere (Lightning Secondary) - launches a lightning zone that travels in the aim
 * direction at a fixed speed, gliding through (not exploding on) enemies. Anyone caught inside
 * takes the standard lightning damage+Charge tick for as long as they're inside, until the
 * sphere's lifetime runs out.
 */
UCLASS()
class RAGEINMAGE_API URageInMageSparkingSphereAbility : public URageInMageDamageGameplayAbility
{
	GENERATED_BODY()

public:
	/** Launch the sphere from the avatar's location toward TargetLocation. Called from Blueprint. */
	UFUNCTION(BlueprintCallable, Category = "SparkingSphere")
	void BeginSparkingSphere(const FVector& TargetLocation);

protected:
	/** Toggle debug visualization of the sphere's spawn point and travel direction. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SparkingSphere|Debug")
	bool bShowDebug = false;

	/** Zone class to spawn (must derive from ARageInMageSparkingSphereZone). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SparkingSphere")
	TSubclassOf<ARageInMageSparkingSphereZone> ZoneClass;

	/** Radius of the sphere's damage/slow volume. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SparkingSphere")
	FScalableFloat ZoneRadius;

	/** How long the sphere lasts before despawning. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SparkingSphere")
	FScalableFloat ZoneDuration;

	/** How often the sphere ticks damage/slow to whoever's inside. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SparkingSphere")
	float DamageTickInterval = 0.5f;

	/** Travel speed of the sphere (units/sec). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SparkingSphere")
	float SphereSpeed = 400.f;

	/** Niagara effect for the sphere itself. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SparkingSphere|VFX")
	TObjectPtr<UNiagaraSystem> ZoneEffect;

	/** Looping sound while the sphere travels. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SparkingSphere|SFX")
	TObjectPtr<USoundBase> ZoneLoopSound;

private:
	FGameplayEffectSpecHandle MakeSparkingSphereDamageSpec() const;
};
