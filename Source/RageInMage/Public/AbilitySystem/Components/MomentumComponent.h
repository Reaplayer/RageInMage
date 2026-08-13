// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "MomentumComponent.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;
class UGameplayAbility;
class ACharacter;

/**
 * Momentum — the Air mage's passive flow-state meter (the counterpart to the Earth mage's Immovable
 * Mass, and the inverse trigger: you build it by MOVING, not by standing still).
 *
 * Moving charges a 0-120 gauge; every movement-spell cast adds a burst; standing still (after a short
 * grace) drains it. The gauge maps to 4 escalating pure-upside stages (move speed, attack speed, and
 * later cooldown reduction) applied by swapping a stage GameplayEffect. Move-speed% also lengthens
 * dashes for free (dashes scale off the MovementSpeed attribute).
 *
 * Server-authoritative: only the authority ticks the gauge and swaps stage effects; the gauge is
 * mirrored to the replicated Momentum attribute for clients/UI. Created on every player but only
 * ActivateMomentum()'d for the Air mage.
 */
UCLASS(ClassGroup = (RageInMage), meta = (BlueprintSpawnableComponent))
class RAGEINMAGE_API UMomentumComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMomentumComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Caches the ASC/owner, binds the movement-cast hook, and enables the gauge tick. Call once the
	 *  ASC is initialized, and only for the Air mage (the caller gates on character class). */
	void ActivateMomentum();

	/** Adds (or removes, if negative) momentum and re-clamps. Public so movement abilities/other
	 *  systems can feed the gauge directly. */
	void AddMomentum(float Delta);

	/** Current stage 0-3 (for UI / other systems). */
	int32 GetStage() const { return CurrentStage; }

protected:
	virtual void BeginPlay() override;

	/* ================= Tunables (editable on the component in BP) ================= */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Momentum|Gauge")
	float MaxGauge = 120.f;

	/** Gauge value at/above which each stage begins (Still Air / Rising Wind / Gale Force / Tempest). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Momentum|Gauge")
	float Stage1Threshold = 35.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Momentum|Gauge")
	float Stage2Threshold = 70.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Momentum|Gauge")
	float Stage3Threshold = 105.f;

	/** Gauge gained per second while moving. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Momentum|Gauge")
	float MomentumPerSecondMoving = 10.f;

	/** Gauge gained per movement-type ability cast (Ability.Type.Movement). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Momentum|Gauge")
	float MomentumPerMovementCast = 10.f;

	/** Horizontal speed (cm/s) above which the mage counts as moving (charges). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Momentum|Gauge")
	float MovingSpeedThreshold = 20.f;

	/** Seconds of continuous stillness before the gauge starts draining. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Momentum|Gauge")
	float StillnessDecayDelay = 1.5f;

	/** Gauge drained per second once standing still past the delay. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Momentum|Gauge")
	float DecayPerSecond = 20.f;

	/** Per-stage buff GameplayEffects (move speed + attack speed [+ cooldown reduction later]). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Momentum|Stages")
	TSubclassOf<UGameplayEffect> Stage1Effect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Momentum|Stages")
	TSubclassOf<UGameplayEffect> Stage2Effect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Momentum|Stages")
	TSubclassOf<UGameplayEffect> Stage3Effect;

private:
	bool bActivated = false;
	int32 CurrentStage = 0;
	float TimeStill = 0.f;
	FVector LastLocation = FVector::ZeroVector;

	FActiveGameplayEffectHandle CurrentStageEffectHandle;

	TWeakObjectPtr<UAbilitySystemComponent> OwnerASC;
	TWeakObjectPtr<ACharacter> OwnerCharacter;

	void UpdateGauge(float DeltaTime);
	void SetGauge(float NewGauge);
	float GetGauge() const;
	int32 StageForGauge(float Gauge) const;
	void EvaluateStage();
	void ApplyStageEffect(int32 NewStage);

	/** Bound to the ASC's ability-activated delegate — grants a momentum burst on movement casts. */
	void OnAbilityActivated(UGameplayAbility* Ability);
};
