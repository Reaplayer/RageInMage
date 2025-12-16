// Copyright Krymson Reaplays


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(int32 XP) const
{
	// Total XP at Level N = 150 × N × (N + 1)
	int32 Level = 1;
	bool bSearching = true;
	while (bSearching)
	{
		if (LevelUpInfos.Num() - 1 <= Level) return Level;
		
		if  (XP >= LevelUpInfos[Level].LevelUpRequirement)
		{
			++Level;
		}
		else
		{
			bSearching = false;
		}
	}
	return Level;
}
