// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Character/MageCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "MageCharacter.generated.h"

/**
 * 
 */
UCLASS()
class RAGEINMAGE_API AMageCharacter : public AMageCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()
public:
	AMageCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/* Combat Interface */
	virtual int32 GetCharacterLevel_Implementation() override;
	/* End Combat Interface */

	/* Player Interface */
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetXP_Implementation() const override;
	virtual int32 FindLevelForXP_Implementation(int32 InXP) const override;
	virtual int32 GetAttributePointsReward_Implementation(int32 InLevel) const override;
	virtual int32 GetSpellPointsReward_Implementation(int32 InLevel) const override;
	virtual void AddToAttributePoints_Implementation(int32 InAttributePoints) const override;
	virtual void AddToSpellPoints_Implementation(int32 InSpellPoints) const override;
	virtual void AddToPlayerLevel_Implementation(int32 InLevel) const override;
	
	/* End Player Interface */

protected:
	

private:
	virtual void InitPlayerAbilityActorInfo() override;
};
