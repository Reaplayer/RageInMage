// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Actor/RageInMageProjectile.h"
#include "RageInMageBoxProjectile.generated.h"

class UBoxComponent;

/**
 * Box-collision projectile. Used for elongated projectile shapes (ice shards, etc.)
 * where a box better fits the visual.
 */
UCLASS()
class RAGEINMAGE_API ARageInMageBoxProjectile : public ARageInMageProjectile
{
	GENERATED_BODY()

public:
	ARageInMageBoxProjectile();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> Box;

protected:
	virtual UPrimitiveComponent* GetCollisionComponent() const override;
};
