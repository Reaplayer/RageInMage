// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "RageInMageBlueSkyWhirlwindAbility.generated.h"

/**
 * Blue Sky Whirlwind - Air Crowd Control ability. A two-part spinning kick combo:
 *   1. An immediate radial AOE "kick" around the caster - every non-friendly inside KickRadius takes a
 *      CauseDamage hit and is knocked back away from the caster.
 *   2. A medium forward dash (velocity-driven, phases through pawns) that carries the caster across a gap.
 *   3. A second identical radial kick at the landing spot, then the ability ends.
 *
 * Reuses URageInMageCleanWindDashAbility's proven dash mechanic (BaseWalkSpeed boost reverted on end,
 * MovementSpeed-scaled travel distance, ECC_Pawn pass-through) but WITHOUT the charge system - this is a
 * cooldown-gated CC spell, so it grants a single use per cooldown via its Cooldown GE. Damage + knockback
 * come from the URageInMageDamageGameplayAbility base (CauseDamage / ApplyKnockback), so knockback is tuned
 * on the shared KnockbackStrength / KnockbackUpwardForce properties.
 */
UCLASS()
class RAGEINMAGE_API URageInMageBlueSkyWhirlwindAbility : public URageInMageDamageGameplayAbility
{
	GENERATED_BODY()

public:
	URageInMageBlueSkyWhirlwindAbility();

	/** Kicks everything around the caster, dashes forward, then kicks again at the landing spot. Call from the GA graph after CommitAbility. */
	UFUNCTION(BlueprintCallable, Category = "BlueSkyWhirlwind")
	void BeginBlueSkyWhirlwind();

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	/** Radius of each radial kick's damage+knockback. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlueSkyWhirlwind")
	FScalableFloat KickRadius;

	/** BaseWalkSpeed set for the duration of the dash (reverted on end). Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlueSkyWhirlwind")
	FScalableFloat CharacterSpeed;

	/** Base dash distance before the MovementSpeed attribute multiplier is applied. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlueSkyWhirlwind")
	FScalableFloat DashDistance;

	/** Hard safety cap so a blocked dash (e.g. against a wall) can't hang the ability and skip the second kick. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlueSkyWhirlwind")
	float MaxDashSafetyDuration = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlueSkyWhirlwind|Debug")
	bool bShowDebug = false;

private:
	/** Radial AOE: damage + knockback (away from Origin) every non-friendly live actor within KickRadius. Authority only. */
	void DoKick(const FVector& Origin);

	void DashTick();
	void EndDash();

	bool bIsDashing = false;
	FTimerHandle DashTickTimerHandle;
	FVector DashDirection = FVector::ZeroVector;
	FVector DashStartLocation = FVector::ZeroVector;
	float ActualDashDistance = 0.f;
	float OriginalBaseWalkSpeed = 600.f;
	float DashElapsedSafety = 0.f;

	/** Capsule's Pawn-channel response captured at dash start, restored on end - dash sets it to Ignore so
	 *  the caster phases through other pawns while dashing (walls still block). */
	TEnumAsByte<ECollisionResponse> SavedPawnCollisionResponse = ECR_Block;
};
