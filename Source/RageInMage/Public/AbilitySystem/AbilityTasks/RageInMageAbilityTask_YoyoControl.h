// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "RageInMageAbilityTask_YoyoControl.generated.h"

class ARageInMageYoyoProjectile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FYoyoRecastAvailableDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FYoyoRecastUnavailableDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FYoyoRecastTriggeredDelegate, const FVector&, AimPosition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FYoyoProjectileLostDelegate);

/**
 * Ability task that monitors a yo-yo projectile and detects recast input.
 *
 * Monitors:
 * - Distance from projectile to CASTER → broadcasts OnRecastAvailable / OnRecastUnavailable
 * - Player input press (re-press after initial activation) → broadcasts OnRecastTriggered(AimPos)
 * - Projectile destruction → broadcasts OnProjectileLost
 *
 * Input detection: The ability activation already consumed the initial press, so InputPressed
 * starts as FALSE. We wait for it to become TRUE (a new press = recast intent).
 */
UCLASS()
class RAGEINMAGE_API URageInMageAbilityTask_YoyoControl : public UAbilityTask
{
	GENERATED_BODY()

public:
	/**
	 * Create the yo-yo control task.
	 * @param Projectile The yo-yo projectile to monitor.
	 * @param RecastRadius Distance from CASTER at which recast becomes available.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks",
		meta = (DisplayName = "YoyoControl", HidePin = "OwningAbility",
			DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static URageInMageAbilityTask_YoyoControl* CreateYoyoControlTask(
		UGameplayAbility* OwningAbility,
		ARageInMageYoyoProjectile* Projectile,
		float RecastRadius = 300.f);

	/** Projectile entered recast range of the caster. */
	UPROPERTY(BlueprintAssignable)
	FYoyoRecastAvailableDelegate OnRecastAvailable;

	/** Projectile left recast range of the caster. */
	UPROPERTY(BlueprintAssignable)
	FYoyoRecastUnavailableDelegate OnRecastUnavailable;

	/** Player pressed input again while projectile is in recast range. */
	UPROPERTY(BlueprintAssignable)
	FYoyoRecastTriggeredDelegate OnRecastTriggered;

	/** Projectile was destroyed (hit something). */
	UPROPERTY(BlueprintAssignable)
	FYoyoProjectileLostDelegate OnProjectileLost;

protected:
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	UPROPERTY()
	TWeakObjectPtr<ARageInMageYoyoProjectile> TrackedProjectile;

	float RecastRadius = 300.f;
	bool bInRecastRange = false;
	bool bWaitingForInputRelease = true; // Must see InputPressed=false first before detecting a new press

	FVector GetAimPosition() const;
};
