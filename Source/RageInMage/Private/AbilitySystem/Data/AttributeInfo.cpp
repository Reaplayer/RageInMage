// Copyright Reaplays


#include "AbilitySystem/Data/AttributeInfo.h"

FRageInMageAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag,
	bool bLogNotFound) const
{
	for (const FRageInMageAttributeInfo& Info : AttributeInformation)
	{
		if (Info.AttributeTag.MatchesTagExact(AttributeTag))
		{
			return Info;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to find info for AttributeTag [%s] on AttributeInfo [%s]."),
			*AttributeTag.ToString(), *GetNameSafe(this))
	}
	return FRageInMageAttributeInfo();
}
