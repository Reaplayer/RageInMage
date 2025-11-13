// Copyright Reaplays


#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

void URageInMageDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
	if (DamageEffectSpecHandle.IsValid())
	{
		for (TTuple<FGameplayTag, FScalableFloat> Pair : DamageTypeTags)
		{
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
				DamageEffectSpecHandle, Pair.Key, Pair.Value.GetValueAtLevel(GetAbilityLevel()));
		}
		GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
			*DamageEffectSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
	}
}

FTaggedMontage URageInMageDamageGameplayAbility::GetRandomTaggedMontageFromArray(
	const TArray<FTaggedMontage>& TaggedMontageArray) const
{
	if (TaggedMontageArray.Num() > 0)
	{
		return TaggedMontageArray[FMath::RandRange(0, TaggedMontageArray.Num() - 1)];
	}
	return FTaggedMontage();
}
