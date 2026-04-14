// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Actor/RageInMageProjectile.h"
#include "RageInMageSphereProjectile.generated.h"


class USphereComponent;

/**
 * Sphere-collision projectile. Used for most standard spell projectiles.
 */
UCLASS()
class RAGEINMAGE_API ARageInMageSphereProjectile : public ARageInMageProjectile
{
	GENERATED_BODY()

public:
	ARageInMageSphereProjectile();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Sphere;

protected:
	virtual UPrimitiveComponent* GetCollisionComponent() const override;
};
