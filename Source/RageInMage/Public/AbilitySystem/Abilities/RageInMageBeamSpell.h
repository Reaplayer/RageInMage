// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "RageInMageBeamSpell.generated.h"

/**
 * Channeled beam ability - fires a continuous beam that ticks damage on targets in the beam path.
 * When the caster's target is Ignited, the beam pierces through to hit enemies behind.
 */
UCLASS()
class RAGEINMAGE_API URageInMageBeamSpell : public URageInMageDamageGameplayAbility
{
	GENERATED_BODY()

public:
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

	/** How often the beam ticks damage (seconds). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Beam")
	float BeamTickInterval = 0.2f;

	/** The width/radius of the beam trace. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Beam")
	float BeamRadius = 30.f;

	/** Maximum range of the beam. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Beam")
	float BeamRange = 1500.f;

	/** Tag that indicates the target is Ignited (beam pierces when this tag is on any hit target). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Beam")
	FGameplayTag IgnitedTag;

private:
	void BeamTickDamage();

	FTimerHandle BeamTickTimerHandle;
	FVector CurrentTargetLocation;
	FGameplayTag CurrentSocketTag;
	bool bIsChanneling = false;
};
