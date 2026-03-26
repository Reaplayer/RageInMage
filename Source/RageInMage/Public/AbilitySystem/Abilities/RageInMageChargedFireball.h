// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageChargedSpell.h"
#include "RageInMageChargedFireball.generated.h"

/**
 * Fire-element charged lobbed spell (Blazing Flame - Fire CC).
 * Inherits all charge/arc/projectile/zone behaviour from URageInMageChargedSpell.
 * Set fire-specific VFX, sounds, zone class, etc. on the Blueprint defaults.
 */
UCLASS()
class RAGEINMAGE_API URageInMageChargedFireball : public URageInMageChargedSpell
{
	GENERATED_BODY()

public:
	/** Backwards-compatible wrapper. New BPs should call ReleaseChargedSpell instead. */
	UFUNCTION(BlueprintCallable, Category = "ChargedSpell", meta = (DeprecatedFunction, DeprecationMessage = "Use ReleaseChargedSpell instead"))
	void ReleaseChargedFireball(const FVector& TargetLocation, const FGameplayTag& SocketTag)
	{
		ReleaseChargedSpell(TargetLocation, SocketTag);
	}
};
