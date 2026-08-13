// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "RageInMageChargingBullAbility.generated.h"

/**
 * Charging Bull - Earth Movement ability. The mage encases an arm and side in rock and charges straight
 * ahead, plowing through any enemy in the path for damage + forward knockback. Structurally mirrors the
 * project's proven dash template (URageInMageCleanWindDashAbility): the charge temporarily raises
 * BaseWalkSpeed to CharacterSpeed (reverted on end) so it flows through the same BaseWalkSpeed *
 * MovementSpeed multiplier as every other speed change, phases through other pawns for its duration, and
 * ends itself once it reaches DashDistance (or a safety timeout). Unlike Clean Wind Dash it is
 * cooldown-gated (no charges) and continuously sweeps as it travels so it hits each enemy it reaches once.
 *
 * Immovable Mass: BOTH the damage and the knockback scale with the caster's current stance stage
 * (+StanceBonusStage1/2/3 percent), evaluated once at charge start.
 */
UCLASS()
class RAGEINMAGE_API URageInMageChargingBullAbility : public URageInMageDamageGameplayAbility
{
	GENERATED_BODY()

public:
	URageInMageChargingBullAbility();

	/** Starts the rock-encased forward charge. Called from the GA graph after CommitAbility. */
	UFUNCTION(BlueprintCallable, Category = "ChargingBull")
	void BeginChargingBull();

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	/** BaseWalkSpeed set for the duration of the charge (reverted on end). Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargingBull")
	FScalableFloat CharacterSpeed;

	/** Base charge distance before the MovementSpeed attribute multiplier is applied. Scales with level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargingBull")
	FScalableFloat DashDistance;

	/** Width of the sweep used to find enemies the charge plows through. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargingBull")
	float DashWidth = 150.f;

	/** Hard safety cap so a blocked charge (e.g. against a wall) can't stay active forever. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargingBull")
	float MaxDashSafetyDuration = 2.f;

	/** Per-Immovable-Mass-stage bonus to BOTH damage and knockback, in percent (design: +10/20/30%). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargingBull|ImmovableMass")
	float StanceBonusStage1 = 10.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargingBull|ImmovableMass")
	float StanceBonusStage2 = 20.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargingBull|ImmovableMass")
	float StanceBonusStage3 = 30.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargingBull|Debug")
	bool bShowDebug = false;

private:
	void DashTick();
	void EndDash();

	/** CauseDamage with every damage-type magnitude multiplied by Multiplier (stance-scaled hit). */
	void CauseScaledDamage(AActor* TargetActor, float Multiplier);
	/** Knockback the target along the charge direction, scaled by the stance multiplier. */
	void ApplyForwardKnockback(AActor* TargetActor) const;

	bool bIsDashing = false;
	FTimerHandle DashTickTimerHandle;
	FVector DashDirection = FVector::ZeroVector;
	FVector DashStartLocation = FVector::ZeroVector;
	float ActualDashDistance = 0.f;
	float OriginalBaseWalkSpeed = 600.f;
	float DashElapsedSafety = 0.f;

	/** Damage/knockback multiplier from the caster's stance stage, captured at charge start. */
	float StanceScalar = 1.f;

	/** Enemies already struck this charge (continuous plow-through hits each enemy once). */
	TSet<TWeakObjectPtr<AActor>> HitActors;

	/** Capsule's Pawn-channel response captured at charge start, restored on end - the charge sets it to
	 *  Ignore so the mage phases through other pawns while charging (walls still block). */
	TEnumAsByte<ECollisionResponse> SavedPawnCollisionResponse = ECR_Block;
};
