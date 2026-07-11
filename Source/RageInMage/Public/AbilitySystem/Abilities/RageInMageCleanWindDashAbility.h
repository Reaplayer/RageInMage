// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "RageInMageCleanWindDashAbility.generated.h"

/**
 * Clean Wind Dash - Air Movement ability. Instant medium-distance forward dash. Any enemy the dash
 * sweeps through is struck (a normal CauseDamage hit) and stunned for StunDuration. Has MaxCharges
 * independently-recharging uses instead of a single cooldown - InstancedPerActor so charge state
 * survives between activations.
 *
 * Structurally mirrors URageInMageZipNZapAbility (the project's proven dash+charge template): the dash
 * temporarily raises BaseWalkSpeed to CharacterSpeed (reverted on end) so it flows through the same
 * BaseWalkSpeed * MovementSpeed multiplier as every other speed change, and DashDistance is scaled by
 * that same MovementSpeed coefficient - a faster character dashes both quicker and further. The one
 * behavioural difference from ZipNZap is the on-hit payload: strike + guaranteed stun instead of a
 * chain-lightning zap, so this inherits URageInMageDamageGameplayAbility (single-target CauseDamage)
 * rather than the Lightning chain base.
 */
UCLASS()
class RAGEINMAGE_API URageInMageCleanWindDashAbility : public URageInMageDamageGameplayAbility
{
	GENERATED_BODY()

public:
	URageInMageCleanWindDashAbility();

	/** Consumes a charge, sweeps the dash path for a one-time strike+stun, and starts the dash. */
	UFUNCTION(BlueprintCallable, Category = "CleanWindDash")
	void BeginCleanWindDash();

	UFUNCTION(BlueprintPure, Category = "CleanWindDash")
	int32 GetChargesRemaining() const { return CurrentCharges; }

	UFUNCTION(BlueprintPure, Category = "CleanWindDash")
	int32 GetMaxCharges() const { return MaxCharges; }

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	/** Number of dash charges available at once. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CleanWindDash")
	int32 MaxCharges = 2;

	/** Seconds for a single spent charge to recharge. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CleanWindDash")
	float RechargeTime = 5.f;

	/** BaseWalkSpeed set for the duration of the dash (reverted on end). Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CleanWindDash")
	FScalableFloat CharacterSpeed;

	/** Base dash distance before the MovementSpeed attribute multiplier is applied. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CleanWindDash")
	FScalableFloat DashDistance;

	/** How long a struck enemy is stunned, in seconds. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CleanWindDash")
	float StunDuration = 1.f;

	/** Extra grace window (added to StunDuration) during which a struck enemy can't be re-stunned - stops perma-lock. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CleanWindDash")
	float StunGraceSeconds = 0.5f;

	/** Width of the sweep used to find enemies along the dash path. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CleanWindDash")
	float DashWidth = 150.f;

	/** Hard safety cap so a blocked dash (e.g. against a wall) can't get stuck forever and never free its charge. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CleanWindDash")
	float MaxDashSafetyDuration = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CleanWindDash|Debug")
	bool bShowDebug = false;

private:
	void DashTick();
	void EndDash();
	void ConsumeCharge();
	void OnChargeRecharged();

	int32 CurrentCharges = 2;
	FTimerHandle RechargeTimerHandle;

	bool bIsDashing = false;
	FTimerHandle DashTickTimerHandle;
	FVector DashDirection = FVector::ZeroVector;
	FVector DashStartLocation = FVector::ZeroVector;
	float ActualDashDistance = 0.f;
	float OriginalBaseWalkSpeed = 600.f;
	float DashElapsedSafety = 0.f;

	/** Capsule's Pawn-channel response captured at dash start, restored on end - the dash sets it to
	 *  Ignore so the character phases through other pawns while dashing (walls still block). */
	TEnumAsByte<ECollisionResponse> SavedPawnCollisionResponse = ECR_Block;
};
