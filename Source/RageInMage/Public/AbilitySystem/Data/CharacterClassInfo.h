// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "ScalableFloat.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UConditionInfo;
class UGameplayAbility;
class UGameplayEffect;

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	Elementalist,
	Ranger,
	Warrior,
	Summoner,
	Summon,
	Tank,
	Assassin,
	MiniBoss,
	Boss,
	FireMage,
	WaterMage,
	AirMage,
	EarthMage,
	LightningMage,
	NatureMage,
	SoundMage,
	ShadowMage,
	Necromancer,
	HolyMage,
	PoisonMage,
	LifeMage
};

USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;

	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Class Defaults")
	FSlateColor ProgressBarColor = FSlateColor::UseForeground();

	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TObjectPtr<UMaterialInstance> BackGroundMaterialInstance = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	FScalableFloat XPReward = FScalableFloat();
};

/**
 * 
 */
UCLASS()
class RAGEINMAGE_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInformation;
	
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> ItemSpecificAttributes;
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> ResistanceAttributes;

	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
		TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Condition Defaults")
	TObjectPtr<UConditionInfo> ConditionInfo;

	UFUNCTION(BlueprintPure, Category = "CharacterClassInfo")
	FCharacterClassDefaultInfo GetCharacterClassDefaultInfo(ECharacterClass CharacterClass);
};
