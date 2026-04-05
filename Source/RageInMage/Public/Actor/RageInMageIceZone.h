// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Actor/RageInMageZone.h"
#include "RageInMageIceZone.generated.h"

class UGameplayEffect;
/**
 * Ice Storm zone - extends base zone with progressive slow and freeze.
 * In addition to damage ticks, applies a stacking slow effect.
 * When an enemy's slow stack count reaches the threshold, applies freeze.
 * The icicle VFX is handled by the Niagara system (random icicles falling within ZoneRadius).
 * Used for Tears Do Fall (Water Ultimate).
 */
UCLASS()
class RAGEINMAGE_API ARageInMageIceZone : public ARageInMageZone
{
	GENERATED_BODY()

public:
	/** GE that applies/stacks slow on enemies each tick. Should be a stacking Duration GE. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IceZone")
	TSubclassOf<UGameplayEffect> SlowEffectClass;

	/** GE that applies freeze when stack threshold is reached. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IceZone")
	TSubclassOf<UGameplayEffect> FreezeEffectClass;

	/** Tag granted by the SlowEffectClass. Used to count stacks on the target. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IceZone")
	FGameplayTag SlowStackTag;

	/** Number of slow stacks needed to trigger freeze. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IceZone")
	int32 FreezeStackThreshold = 3;

protected:
	virtual void DamageTickEnemiesInside() override;
};
