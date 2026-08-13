// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "RageInMageAbilityTask_DualAimMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDualAimUpdateDelegate, const FVector&, AimPosition, const FVector&, AimDirection, float, DeltaTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDualAimReleaseDelegate, const FVector&, FinalAimPosition, const FVector&, FinalAimDirection);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDualAimCancelDelegate);

/**
 * Ability task for spells that need BOTH a placed ground point AND a direction
 * (Jagged Terra Form's push direction, Rock Solid's wall facing).
 *
 * Sibling of URageInMageAbilityTask_AimMode, which is position-only. The lifecycle is
 * identical (hold to aim, release to confirm, cancel on ability cancel) — the only
 * difference is that every delegate also carries a direction.
 *
 * Input model (both handled by the PlayerController, see BeginDualAim):
 *   Gamepad — LEFT stick moves the point, RIGHT stick sets the direction. The pawn does
 *             NOT move while aiming; the left stick is repurposed for the duration.
 *   M&K     — PRESS places the point at the cursor, the mouse then sets the direction
 *             from that fixed point, RELEASE confirms.
 *
 * Blueprint usage:
 *   ActivateAbility -> CommitAbilityCost -> DualAimMode
 *     OnAimUpdate:     drive the preview indicator (position + rotation)
 *     OnInputReleased: cast using the final position + direction, then EndAbility
 *     OnCancelled:     clean up and EndAbility
 */
UCLASS()
class RAGEINMAGE_API URageInMageAbilityTask_DualAimMode : public UAbilityTask
{
	GENERATED_BODY()

public:
	/**
	 * Start dual-stick aim mode. Ticks OnAimUpdate every frame until input is released.
	 * @param MaxRange If > 0, clamps the placed point to this distance from the caster.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks",
		meta = (DisplayName = "DualAimMode", HidePin = "OwningAbility",
			DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static URageInMageAbilityTask_DualAimMode* CreateDualAimModeTask(
		UGameplayAbility* OwningAbility, float MaxRange = 0.f);

	/** Fires every tick with the current aim position, direction and delta time. */
	UPROPERTY(BlueprintAssignable)
	FDualAimUpdateDelegate OnAimUpdate;

	/** Fires when the player releases the input. Time to cast! */
	UPROPERTY(BlueprintAssignable)
	FDualAimReleaseDelegate OnInputReleased;

	/** Fires if the ability is cancelled during aim mode. */
	UPROPERTY(BlueprintAssignable)
	FDualAimCancelDelegate OnCancelled;

protected:
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	float MaxRange = 0.f;
	bool bInputReleased = false;

	/** Read the current placed point + direction off the PlayerController. */
	void GetAimState(FVector& OutPosition, FVector& OutDirection) const;

	/** Called when the ability's input is released via GAS. */
	void HandleInputRelease();
};
