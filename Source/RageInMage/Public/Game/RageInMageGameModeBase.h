// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RageInMageGameModeBase.generated.h"

class UCharacterClassInfo;
/**
 * 
 */
UCLASS()
class RAGEINMAGE_API ARageInMageGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	bool bIsTeamGame = false;

protected:

private:
};
