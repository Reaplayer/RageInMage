// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Character/MageCharacterBase.h"
#include "MageCharacter.generated.h"

/**
 * 
 */
UCLASS()
class RAGEINMAGE_API AMageCharacter : public AMageCharacterBase
{
	GENERATED_BODY()
public:
	AMageCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/* Combat Interface */
	virtual int32 GetPlayerLevel() override;
	/* End Combat Interface */

protected:


private:
	virtual void InitPlayerAbilityActorInfo() override;
};
