// Copyright Reaplays

#include "AbilitySystem/Data/InputDisplayInfo.h"

const FInputTagDisplayInfo* UInputDisplayInfo::FindInfoForTag(const FGameplayTag& InputTag) const
{
	for (const FInputTagDisplayInfo& Info : InputDisplayData)
	{
		if (Info.InputTag.MatchesTagExact(InputTag))
		{
			return &Info;
		}
	}
	return nullptr;
}

FText UInputDisplayInfo::GetDisplayNameForTag(const FGameplayTag& InputTag) const
{
	if (const FInputTagDisplayInfo* Info = FindInfoForTag(InputTag))
	{
		return Info->DisplayName;
	}
	return FText::FromString(InputTag.ToString());
}
