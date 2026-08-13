// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Actor/RageInMageZone.h"
#include "RageInMageGemJailZone.generated.h"

class UGameplayEffect;

/**
 * Gem Jail (Earth CrowdControl) - an area of gems that deals NO damage. PetrifyDelay seconds after the
 * gems form, every enemy still standing inside is encased and petrified; they have to break free.
 *
 * Damage is opted out entirely via UsesBuiltinTickDamage() -> false, so neither the base's on-entry hit
 * nor its periodic damage timer ever runs. Target tracking is inherited: ARageInMageZone maintains
 * ActorsInside from overlap events AND seeds it at BeginPlay with whoever is already inside the
 * footprint, so the delayed sweep is an accurate "who is still in the gems" snapshot.
 *
 * The jail itself is a GameplayEffect (PetrifyEffect) whose duration comes from a SetByCaller keyed to
 * Condition.Petrified. Granting Condition.Petrified alone is sufficient: it locks movement via
 * ARageInMageCharacterBase::CrowdControlTagChanged and blocks actions via URageInMageGameplayAbility's
 * ActivationBlockedTags, both of which cover the whole incapacitation set. Do NOT also grant
 * Condition.Stunned - keeping the conditions distinct is what lets a stun, a petrify and a freeze be
 * chained onto the same target while each still gets its own independent immunity window.
 */
UCLASS()
class RAGEINMAGE_API ARageInMageGemJailZone : public ARageInMageZone
{
	GENERATED_BODY()

public:
	ARageInMageGemJailZone();

	/** Seconds after the gems form before anyone still inside is jailed. Enemies can escape before this. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GemJail", meta = (ExposeOnSpawn = true))
	float PetrifyDelay = 2.f;

	/** How long the petrify lasts, fed to PetrifyEffect via SetByCaller(Condition.Petrified). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GemJail", meta = (ExposeOnSpawn = true))
	float PetrifyDuration = 4.f;

	/** GE granting Condition.Petrified (and Condition.Stunned, for the movement/action lock).
	 *  Its duration must read SetByCaller keyed to Condition.Petrified. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GemJail")
	TSubclassOf<UGameplayEffect> PetrifyEffect;

protected:
	virtual void BeginPlay() override;

	/** Gem Jail deals no damage at all - it only jails. */
	virtual bool UsesBuiltinTickDamage() const override { return false; }

private:
	/** Fires once, PetrifyDelay after spawn: petrifies every enemy still inside. */
	void JailEnemiesInside();

	FTimerHandle PetrifyTimerHandle;
};
