// Copyright Krymson Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "KeyIconData.generated.h"

/**
 * DataTable row mapping an FKey to its display icon texture.
 * Row Name should be the FKey name (e.g. "LeftMouseButton", "Gamepad_RightTrigger").
 */
USTRUCT(BlueprintType)
struct FKeyIconRow : public FTableRowBase
{
	GENERATED_BODY()

	/** The key this row represents (redundant with RowName but useful for lookups). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FKey Key;

	/** Icon texture for this key/button. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon;

	/** Short display label (e.g. "LMB", "RT", "Shift"). Used as fallback if no icon texture. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayLabel;
};
