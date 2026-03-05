// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "RageInMageAbilityTask_AimMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAimUpdateDelegate, const FVector&, AimPosition, float, DeltaTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAimReleaseDelegate, const FVector&, FinalAimPosition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAimCancelDelegate);

/**
 * Ability task that implements the hold-to-aim pattern.
 * Ticks every frame while held, broadcasting the current aim world position.
 * When the player releases the input, broadcasts the final aim position for casting.
 *
 * Blueprint usage:
 *   ActivateAbility -> CommitAbility -> CreateAimModeTask
 *     OnAimUpdate: Update preview indicators each frame
 *     OnInputReleased: Fire the spell and EndAbility
 *     OnCancelled: Clean up and EndAbility(cancelled)
 */
UCLASS()
class RAGEINMAGE_API URageInMageAbilityTask_AimMode : public UAbilityTask
{
	GENERATED_BODY()

public:
	/**
	 * Start the aim mode task. Ticks OnAimUpdate every frame until input is released.
	 * @param MaxRange If > 0, clamps the aim position to this distance from the caster.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks",
		meta = (DisplayName = "AimMode", HidePin = "OwningAbility",
			DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static URageInMageAbilityTask_AimMode* CreateAimModeTask(
		UGameplayAbility* OwningAbility, float MaxRange = 0.f);

	/** Fires every tick with the current aim position and delta time. */
	UPROPERTY(BlueprintAssignable)
	FAimUpdateDelegate OnAimUpdate;

	/** Fires when the player releases the input. Time to cast! */
	UPROPERTY(BlueprintAssignable)
	FAimReleaseDelegate OnInputReleased;

	/** Fires if the ability is cancelled during aim mode. */
	UPROPERTY(BlueprintAssignable)
	FAimCancelDelegate OnCancelled;

protected:
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	float MaxRange = 0.f;
	bool bInputReleased = false;

	/** Get aim position, clamped to MaxRange if set. */
	FVector GetClampedAimPosition() const;

	/** Called when the ability's input is released via GAS. */
	void HandleInputRelease();
};
