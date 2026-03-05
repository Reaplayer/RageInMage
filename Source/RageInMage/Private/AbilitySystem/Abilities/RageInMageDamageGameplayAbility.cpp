// Copyright Reaplays


#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"

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

void URageInMageDamageGameplayAbility::ApplyKnockback(AActor* Target, const FVector& Origin) const
{
	const float Strength = KnockbackStrength.GetValueAtLevel(GetAbilityLevel());
	if (Strength <= 0.f || !Target) return;

	ACharacter* TargetCharacter = Cast<ACharacter>(Target);
	if (!TargetCharacter) return;

	FVector PushDirection = (Target->GetActorLocation() - Origin).GetSafeNormal();
	PushDirection.Z = FMath::Clamp(KnockbackUpwardForce, 0.f, 1.f);
	PushDirection.Normalize();
	TargetCharacter->LaunchCharacter(PushDirection * Strength, true, true);
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
