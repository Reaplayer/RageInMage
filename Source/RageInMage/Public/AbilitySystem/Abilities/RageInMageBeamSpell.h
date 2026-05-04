// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "RageInMageBeamSpell.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class USoundBase;
class UAudioComponent;

/**
 * Channeled beam ability - fires a continuous beam that ticks damage on targets in the beam path.
 * When the caster's target is Ignited, the beam pierces through to hit enemies behind.
 */
UCLASS()
class RAGEINMAGE_API URageInMageBeamSpell : public URageInMageDamageGameplayAbility
{
	GENERATED_BODY()

public:
	URageInMageBeamSpell();

	/** Start channeling the beam from the caster toward the target location. Called from Blueprint. */
	UFUNCTION(BlueprintCallable, Category = "Beam")
	void StartBeamChannel(const FVector& TargetLocation, const FGameplayTag& SocketTag);

	/** Stop channeling the beam. Called from Blueprint when channel ends. */
	UFUNCTION(BlueprintCallable, Category = "Beam")
	void StopBeamChannel();

	/** Update the beam target location each frame while channeling. */
	UFUNCTION(BlueprintCallable, Category = "Beam")
	void UpdateBeamTarget(const FVector& NewTargetLocation);

protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// ── Damage ──

	/** How often the beam ticks damage (seconds). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Beam|Damage")
	float BeamTickInterval = 0.2f;

	/** The width/radius of the beam trace. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Beam|Damage")
	float BeamRadius = 30.f;

	/** Maximum range of the beam. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Beam|Damage")
	float BeamRange = 1500.f;

	/** How fast the beam particles travel from start to end (units/sec). Used to calculate particle lifetime. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Beam|Damage")
	float BeamSpeed = 1000.f;

	/** How long smoke emitters linger after the beam stops (seconds). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Beam|VFX")
	float SmokeLingerDuration = 2.f;

	/** Tag that indicates the target is Ignited (beam pierces when this tag is on any hit target). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Beam|Damage")
	FGameplayTag IgnitedTag;

	// ── VFX ──

	/** Niagara system to spawn for the beam visual. Must have BeamStart, BeamEnd (Position) and BeamKill, SmokeKill (Bool) user params. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Beam|VFX")
	TObjectPtr<UNiagaraSystem> BeamVFX;

	// ── SFX ──

	/** Looping sound to play while channeling. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Beam|SFX")
	TObjectPtr<USoundBase> BeamLoopSound;

private:
	void BeamTickDamage();
	void SpawnBeamVFX();
	void UpdateBeamVFX();
	void DestroyBeamVFX();

	FTimerHandle BeamTickTimerHandle;
	FVector CurrentTargetLocation;
	FGameplayTag CurrentSocketTag;
	bool bIsChanneling = false;

	/** Distance along beam where a non-ignited target blocks it. Only valid when bBeamBlocked is true. */
	float BlockedBeamDistance = 0.f;
	bool bBeamBlocked = false;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> BeamVFXComponent;

	UPROPERTY()
	TObjectPtr<UAudioComponent> BeamLoopSoundComponent;
};
