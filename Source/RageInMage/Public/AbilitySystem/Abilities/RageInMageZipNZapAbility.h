// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageLightningDamageAbility.h"
#include "RageInMageZipNZapAbility.generated.h"

/**
 * Zip 'N' Zap - Lightning Movement ability. Instant forward dash ("Zip") that zaps ("Zap") every
 * enemy along its path with a normal chain-lightning hit. Has MaxCharges independently-recharging
 * uses instead of a single cooldown (first ability-charge system in this project) - InstancedPerActor
 * so charge state survives between activations.
 *
 * The dash works by temporarily raising the character's BaseWalkSpeed to CharacterSpeed (reverted on
 * end) instead of overriding MaxWalkSpeed directly, so it goes through the same
 * BaseWalkSpeed * MovementSpeed multiplier every other speed change does - a character with a higher
 * MovementSpeed coefficient dashes faster automatically. DashDistance is scaled by that same
 * MovementSpeed coefficient, so a faster character also dashes further.
 */
UCLASS()
class RAGEINMAGE_API URageInMageZipNZapAbility : public URageInMageLightningDamageAbility
{
	GENERATED_BODY()

public:
	URageInMageZipNZapAbility();

	/** Consumes a charge, sweeps the dash path for a one-time zap, and starts the dash. */
	UFUNCTION(BlueprintCallable, Category = "ZipNZap")
	void BeginZipNZap();

	UFUNCTION(BlueprintPure, Category = "ZipNZap")
	int32 GetChargesRemaining() const { return CurrentCharges; }

	UFUNCTION(BlueprintPure, Category = "ZipNZap")
	int32 GetMaxCharges() const { return MaxCharges; }

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	/** Number of dash charges available at once. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ZipNZap")
	int32 MaxCharges = 2;

	/** Seconds for a single spent charge to recharge. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ZipNZap")
	float RechargeTime = 4.f;

	/** BaseWalkSpeed set for the duration of the dash (reverted on end). Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ZipNZap")
	FScalableFloat CharacterSpeed;

	/** Base dash distance before the MovementSpeed attribute multiplier is applied. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ZipNZap")
	FScalableFloat DashDistance;

	/** Width of the sweep used to find enemies along the dash path. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ZipNZap")
	float DashWidth = 150.f;

	/** Hard safety cap so a blocked dash (e.g. against a wall) can't get stuck forever and never free its charge. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ZipNZap")
	float MaxDashSafetyDuration = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ZipNZap|Debug")
	bool bShowDebug = false;

private:
	void ZipTick();
	void EndZip();
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
};
