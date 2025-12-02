// Copyright Krymson Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AbilityInfo.generated.h"

USTRUCT(BlueprintType)
struct FRageInMageAbilityInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityTag = FGameplayTag();
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	FSlateColor ProgressBarColor = FSlateColor::UseForeground();

	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TObjectPtr<UMaterialInterface> BackGroundMaterialInstance = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AbilityDefinition = FText();
};
/**
 * @struct FRageInMageAbilityInfo
 * @brief Represents a data structure for holding information about an ability in the RageInMage game.
 *
 * This struct is used to encapsulate and organize data related to abilities, which can include
 * properties such as name, description, cooldown, cost, effects, and other metadata needed
 * for gameplay. It is designed to be used in conjunction with the UAbilityInfo class for
 * asset management and reflection in Unreal Engine.
 *
 * @note This struct provides the data foundation for ability-related data but does not
 * implement gameplay logic.
 */
UCLASS()
class RAGEINMAGE_API UAbilityInfo : public UDataAsset
{
	GENERATED_BODY()
	
	public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Info")
	TArray<FRageInMageAbilityInfo> AbilityInfos;
	
	
	FRageInMageAbilityInfo FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound = false) const;
};
