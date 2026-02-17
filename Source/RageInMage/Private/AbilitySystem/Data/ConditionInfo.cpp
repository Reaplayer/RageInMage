// Copyright Krymson Reaplays


#include "AbilitySystem/Data/ConditionInfo.h"

#include "RageInMage/RageInMageLogChannels.h"

FRageInMageConditionInfo UConditionInfo::FindConditionInfoForTag(const FGameplayTag& ConditionTag, bool bLogNotFound) const
{
	for (const FRageInMageConditionInfo& Info : ConditionInfos)
	{
		if (Info.ConditionTag.MatchesTagExact(ConditionTag))
		{
			return Info;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogRageInMage, Error, TEXT("Can't find ConditionInfo for ConditionTag [%s], on ConditionInfo [%s]"),
			*ConditionTag.ToString(), *GetNameSafe(this));
	}
	return FRageInMageConditionInfo();
}

TArray<FRageInMageConditionInfo> UConditionInfo::GetConditionsByCategory(EConditionCategory Category) const
{
	TArray<FRageInMageConditionInfo> Result;
	for (const FRageInMageConditionInfo& Info : ConditionInfos)
	{
		if (Info.Category == Category)
		{
			Result.Add(Info);
		}
	}
	return Result;
}

const FRageInMageConditionInfo* UConditionInfo::FindConditionForMechanicsThreshold(
	const FGameplayTag& MechanicsTag, float CurrentValue) const
{
	for (const FRageInMageConditionInfo& Info : ConditionInfos)
	{
		if (Info.MechanicsAttributeTag.MatchesTagExact(MechanicsTag))
		{
			if (Info.MechanicsThreshold >= 0.f && CurrentValue >= Info.MechanicsThreshold)
			{
				return &Info;
			}
			if (Info.MechanicsThreshold < 0.f && CurrentValue <= Info.MechanicsThreshold)
			{
				return &Info;
			}
		}
	}
	return nullptr;
}
