// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "InputDisplayInfo.generated.h"

/**
 * Display metadata for a single input tag, used in the keybinding remapping UI.
 */
USTRUCT(BlueprintType)
struct FInputTagDisplayInfo
{
	GENERATED_BODY()

	/** The input tag (e.g., InputTag.LMB, InputTag.Controller.RT) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag InputTag;

	/** Human-readable name shown in the remapping UI (e.g., "Primary Spell") */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	/** Category grouping for the UI (e.g., "Spells", "Movement", "Actions") */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Category;

	/** Whether this binding can be remapped (some may be locked) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsRemappable = true;
};

/**
 * DataAsset providing display names and categories for all remappable InputTags.
 * Set in the editor and referenced by ControlsWidgetController.
 */
UCLASS()
class RAGEINMAGE_API UInputDisplayInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TArray<FInputTagDisplayInfo> InputDisplayData;

	/** Find display info for a given InputTag. Returns nullptr if not found. */
	const FInputTagDisplayInfo* FindInfoForTag(const FGameplayTag& InputTag) const;

	/** Get display name for a tag, or the tag name as fallback. */
	FText GetDisplayNameForTag(const FGameplayTag& InputTag) const;
};
