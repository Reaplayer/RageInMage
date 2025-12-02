// Copyright Krymson Reaplays


#include "AbilitySystem/Data/AbilityInfo.h"

#include "RageInMage/RageInMageLogChannels.h"

FRageInMageAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (const FRageInMageAbilityInfo Info : AbilityInfos)
	{
		if (Info.AbilityTag.MatchesTagExact(AbilityTag))
		{
			return Info;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogRageInMage, Error, TEXT("Can't find AbilityInfo for AbilityTag [%s], on AbilityInfo [%s]"),
			*AbilityTag.ToString(), *GetNameSafe(this));
	}
	return FRageInMageAbilityInfo();
}
