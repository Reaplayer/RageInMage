// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "RageInMageConfig.generated.h"

USTRUCT(BlueprintType)
struct FMageInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	const class UInputAction* InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputModifierTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputModifierAdditionalTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AbilityTypeTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly)
	FText Name = FText();
};

/**
 * 
 */
UCLASS()
class RAGEINMAGE_API URageInMageConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = false) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FMageInputAction> AbilityInputActions;
};
