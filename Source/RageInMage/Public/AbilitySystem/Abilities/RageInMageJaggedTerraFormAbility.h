// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "RageInMageJaggedTerraFormAbility.generated.h"

class ARageInMageSpikeField;

/**
 * Jagged Terra Form — the Earth mage's Secondary spell. Punch the ground and a field of stone
 * spikes erupts at the placed point: everyone caught in it takes PIERCING damage and is shoved in
 * a chosen direction, and the spikes stay behind as an impassable barrier.
 *
 * Aimed with the DUAL-STICK aim task (URageInMageAbilityTask_DualAimMode): the placed point is
 * where the field erupts, the direction is which way the caught enemies get pushed.
 *
 * Unlike the parent's ApplyKnockback (which always pushes away from an origin) this pushes
 * everyone the SAME way — that is the spell's whole trick, herding a pack in one direction.
 *
 * Blueprint usage:
 *   ActivateAbility -> CommitAbilityCost -> DualAimMode
 *     OnAimUpdate:     DrawDebugSpikeFieldPreview(AimPosition, AimDirection)
 *     OnInputReleased: (play the punch montage, then) EruptSpikes(FinalAimPosition, FinalAimDirection) -> EndAbility
 *     OnCancelled:     EndAbility
 */
UCLASS()
class RAGEINMAGE_API URageInMageJaggedTerraFormAbility : public URageInMageDamageGameplayAbility
{
	GENERATED_BODY()

public:
	/**
	 * Erupt the spikes: damage + push every non-friendly in range, then leave the barrier behind.
	 * Server-authoritative (no-ops on clients).
	 * @param Location      Ground point the field erupts at.
	 * @param PushDirection Horizontal direction everyone caught is shoved.
	 */
	UFUNCTION(BlueprintCallable, Category = "JaggedTerraForm")
	ARageInMageSpikeField* EruptSpikes(const FVector& Location, const FVector& PushDirection);

	/** Knock a target in an explicit direction (rather than away from an origin like the parent). */
	UFUNCTION(BlueprintCallable, Category = "JaggedTerraForm|Knockback")
	void ApplyDirectionalKnockback(AActor* Target, const FVector& Direction) const;

	/** Draw the field footprint + push arrow. Call every OnAimUpdate tick. */
	UFUNCTION(BlueprintCallable, Category = "JaggedTerraForm|Debug")
	void DrawDebugSpikeFieldPreview(const FVector& Location, const FVector& Direction);

	/** The Immovable Mass stance scalar this cast would use (1.0 at stage 0). */
	UFUNCTION(BlueprintPure, Category = "JaggedTerraForm")
	float GetFieldAreaScalar() const;

	/** Field radius after stance scaling — what the cast will actually use. */
	UFUNCTION(BlueprintPure, Category = "JaggedTerraForm")
	float GetScaledFieldRadius() const;

protected:
	/** The spike field class to spawn. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "JaggedTerraForm")
	TSubclassOf<ARageInMageSpikeField> SpikeFieldClass;

	/** Radius of the eruption's damage/push query, before stance scaling. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "JaggedTerraForm")
	float FieldRadius = 350.f;

	/** How long the spike barrier stays up. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "JaggedTerraForm")
	FScalableFloat FieldDuration;

	// ── Immovable Mass stance scaling (AREA only — design: +15/30/50%) ──

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "JaggedTerraForm|ImmovableMass")
	float StanceBonusStage1 = 15.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "JaggedTerraForm|ImmovableMass")
	float StanceBonusStage2 = 30.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "JaggedTerraForm|ImmovableMass")
	float StanceBonusStage3 = 50.f;
};
