// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Actor/RageInMageZone.h"
#include "RageInMageSparkingSphereZone.generated.h"

class UProjectileMovementComponent;

/**
 * Travelling lightning zone - flies in a straight line via ProjectileMovementComponent instead
 * of staying put like the base zone. Glides through enemies (no impact/explosion), applying the
 * base zone's damage tick to anyone inside, until ZoneDuration expires.
 * Used for Sparking Sphere (Lightning Secondary).
 */
UCLASS()
class RAGEINMAGE_API ARageInMageSparkingSphereZone : public ARageInMageZone
{
	GENERATED_BODY()

public:
	ARageInMageSparkingSphereZone();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	/** Draws a debug sphere at the zone's current location/radius every frame as it travels. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SparkingSphere|Debug")
	bool bShowDebug = false;

protected:
	virtual void Tick(float DeltaTime) override;

	/**
	 * SparkingSphere drives its own damage in Tick() with a per-target cooldown, so it opts out
	 * of the base zone's fixed-interval timer and immediate on-entry damage.
	 */
	virtual bool UsesBuiltinTickDamage() const override { return false; }

private:
	/** Last time (world seconds) each in-range actor was damaged. Weak so dead actors fall out safely. */
	TMap<TWeakObjectPtr<AActor>, float> LastDamageTimeByActor;
};
