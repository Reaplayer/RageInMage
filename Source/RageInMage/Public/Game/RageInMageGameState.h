// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RageInMageGameState.generated.h"

class UCharacterClassInfo;

/**
 * 
 */
UCLASS()
class RAGEINMAGE_API ARageInMageGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;
};