// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageLightningDamageAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"

void URageInMageLightningDamageAbility::CauseChainDamage(AActor* InitialTarget)
{
	FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
	if (!DamageEffectSpecHandle.IsValid()) return;

	for (const TTuple<FGameplayTag, FScalableFloat>& Pair : DamageTypeTags)
	{
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
			DamageEffectSpecHandle, Pair.Key, Pair.Value.GetValueAtLevel(GetAbilityLevel()));
	}

	URageInMageAbilitySystemLibrary::ApplyChainLightningDamage(
		GetAvatarActorFromActorInfo(), InitialTarget, DamageEffectSpecHandle,
		ChainJumpRadius, ChainDamageFalloffPerJump, ChainMaxJumps, ChainImpactEffect);
}
