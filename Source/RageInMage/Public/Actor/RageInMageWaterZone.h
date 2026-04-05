// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Actor/RageInMageZone.h"
#include "RageInMageWaterZone.generated.h"

/**
 * Water-element zone. Inherits all AoE behaviour from ARageInMageZone.
 * Set water-specific Niagara effects, sounds, etc. on the Blueprint defaults.
 * Used for Riding Tide trail puddles, Swirling Yo-yo impact puddles, etc.
 */
UCLASS()
class RAGEINMAGE_API ARageInMageWaterZone : public ARageInMageZone
{
	GENERATED_BODY()
};
