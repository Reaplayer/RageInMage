// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageGameplayAbility.h"
#include "ScalableFloat.h"
#include "RageInMageRockSolidAbility.generated.h"

class ARageInMageRockWall;

/**
 * Rock Solid — the Earth mage's Defensive spell. Summons an indestructible blocking wall at a
 * placed point, facing a chosen direction, for a few seconds.
 *
 * Deals NO damage, so this derives from the plain gameplay ability base rather than the damage one.
 *
 * Aimed with the DUAL-STICK aim task (URageInMageAbilityTask_DualAimMode): the placed point is
 * where the wall goes, the direction is the way it FACES (the wall spans perpendicular to it,
 * same convention as URageInMageWallAbility).
 *
 * Blueprint usage:
 *   ActivateAbility -> CommitAbilityCost -> DualAimMode
 *     OnAimUpdate:     DrawDebugRockWallPreview(AimPosition, AimDirection)
 *     OnInputReleased: SpawnRockWall(FinalAimPosition, FinalAimDirection) -> EndAbility
 *     OnCancelled:     EndAbility
 */
UCLASS()
class RAGEINMAGE_API URageInMageRockSolidAbility : public URageInMageGameplayAbility
{
	GENERATED_BODY()

public:
	/**
	 * Spawn the wall at Location, facing Direction. Server-authoritative (no-ops on clients).
	 * @param Location  Ground point the wall stands on (the spawn is offset up by its half-height).
	 * @param Direction Horizontal direction the wall FACES; it spans perpendicular to this.
	 */
	UFUNCTION(BlueprintCallable, Category = "RockSolid")
	ARageInMageRockWall* SpawnRockWall(const FVector& Location, const FVector& Direction);

	/** Draw a debug box matching where the wall would land. Call every OnAimUpdate tick. */
	UFUNCTION(BlueprintCallable, Category = "RockSolid|Debug")
	void DrawDebugRockWallPreview(const FVector& Location, const FVector& Direction);

	/** The Immovable Mass stance scalar this cast would use (1.0 at stage 0). Preview uses it too. */
	UFUNCTION(BlueprintPure, Category = "RockSolid")
	float GetWallSizeScalar() const;

protected:
	/** The rock wall class to spawn. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RockSolid")
	TSubclassOf<ARageInMageRockWall> RockWallClass;

	/** How long the wall stands. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RockSolid")
	FScalableFloat WallDuration;

	// ── Immovable Mass stance scaling (wall SIZE only — design: +10/20/30%) ──

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RockSolid|ImmovableMass")
	float StanceBonusStage1 = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RockSolid|ImmovableMass")
	float StanceBonusStage2 = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RockSolid|ImmovableMass")
	float StanceBonusStage3 = 30.f;
};
