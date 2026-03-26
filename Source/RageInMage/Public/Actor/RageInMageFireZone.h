// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Actor/RageInMageZone.h"
#include "RageInMageFireZone.generated.h"

/**
 * Fire-element zone. Inherits all AoE behaviour from ARageInMageZone.
 * Set fire-specific Niagara effects, sounds, etc. on the Blueprint defaults.
 */
UCLASS()
class RAGEINMAGE_API ARageInMageFireZone : public ARageInMageZone
{
	GENERATED_BODY()
};
