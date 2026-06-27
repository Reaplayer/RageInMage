// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "RageInMageWaveBox.generated.h"

class ACharacter;
class UBoxComponent;
class ARageInMageProjectile;

/**
 * Invisible wave box that rides under the caster during Riding Tide (and similar surf abilities).
 * - Absorbs enemy projectiles (destroys them harmlessly)
 * - Exception: lightning-type projectiles pass damage through to the riding character
 * - Friendly projectiles pass through unaffected
 * - Acts as a moving wall for enemies: anything it overlaps gets dragged along every tick and
 *   damaged/popped on its own interval — this is the actual hit-detection for the surf, not the
 *   ability's radius check (which only runs as a fallback when no wave box is configured)
 * - No mesh — purely a collision actor driven by the owning ability's tick
 */
UCLASS()
class RAGEINMAGE_API ARageInMageWaveBox : public AActor
{
	GENERATED_BODY()

public:
	ARageInMageWaveBox();

	virtual void Tick(float DeltaTime) override;

	/** Box collision for intercepting projectiles and enemies. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> BoxCollision;

	/** The character riding on this wave. Set by the spawning ability. */
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<ACharacter> RidingCharacter;

	/** Damage GE spec applied to enemies the box overlaps. Set by the spawning ability. */
	UPROPERTY(BlueprintReadWrite)
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

	/** Optional secondary effect (e.g. slow/wet) applied alongside damage. */
	UPROPERTY(BlueprintReadWrite)
	FGameplayEffectSpecHandle OnHitEffectSpecHandle;

	/** Continuous drag velocity (units/sec) applied along the box's current forward vector to
	 *  anything it overlaps, every tick — this is what carries enemies along with the wave. */
	UPROPERTY(BlueprintReadWrite)
	float DragSpeed = 0.f;

	/** Extra upward pop applied alongside each damage tick. */
	UPROPERTY(BlueprintReadWrite)
	float PushbackStrength = 0.f;

	/** Interval between damage ticks on the same enemy (seconds). 0 = hit once only. Drag is
	 *  always continuous regardless of this interval. */
	UPROPERTY(BlueprintReadWrite)
	float DamageTickInterval = 0.f;

	/** Update the box position to sit below the character at the given wave height. */
	void UpdatePosition(const FVector& CharacterLocation, float CurrentHeight, float CapsuleHalfHeight);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

private:
	/** Check if a projectile's damage spec contains electric/lightning damage. */
	bool IsLightningProjectile(ARageInMageProjectile* Projectile) const;

	/** Route the projectile's damage to the riding character's ASC. */
	void RouteDamageToRider(ARageInMageProjectile* Projectile);

	/** Drag + damage every enemy character currently overlapping the box. */
	void ProcessEnemyOverlaps();

	/** Track last hit time per enemy for DamageTickInterval enforcement. */
	UPROPERTY()
	TMap<TObjectPtr<AActor>, double> DamagedEnemyTimestamps;
};
