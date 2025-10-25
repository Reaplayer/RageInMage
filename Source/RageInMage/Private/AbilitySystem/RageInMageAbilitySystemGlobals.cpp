// Copyright Reaplays


#include "AbilitySystem/RageInMageAbilitySystemGlobals.h"

#include "RageInMageAbilitySystemTypes.h"

FGameplayEffectContext* URageInMageAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FRageInMageGameplayEffectContext();
}
