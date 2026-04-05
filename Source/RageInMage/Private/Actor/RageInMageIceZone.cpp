// Copyright Reaplays

#include "Actor/RageInMageIceZone.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"


void ARageInMageIceZone::DamageTickEnemiesInside()
{
	// Base class handles damage ticking
	Super::DamageTickEnemiesInside();

	if (!HasAuthority() || !SlowEffectClass || !FreezeEffectClass) return;

	// Clean up stale refs (base already does this, but be safe)
	ActorsInside.RemoveAll([](const TObjectPtr<AActor>& Actor) { return !IsValid(Actor); });

	TArray<TObjectPtr<AActor>> ActorsToProcess = ActorsInside;

	for (AActor* Actor : ActorsToProcess)
	{
		if (!IsValid(Actor)) continue;
		if (URageInMageAbilitySystemLibrary::IsBothEnemy(GetInstigator(), Actor)) continue;

		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
		if (!TargetASC) continue;

		// Apply slow stack
		FGameplayEffectContextHandle EffectCtx = TargetASC->MakeEffectContext();
		EffectCtx.AddInstigator(GetInstigator(), GetInstigator());
		const FGameplayEffectSpecHandle SlowSpec = TargetASC->MakeOutgoingSpec(SlowEffectClass, 1.f, EffectCtx);
		TargetASC->ApplyGameplayEffectSpecToSelf(*SlowSpec.Data.Get());

		// Check if slow stacks reached freeze threshold
		if (SlowStackTag.IsValid())
		{
			const int32 StackCount = TargetASC->GetTagCount(SlowStackTag);
			if (StackCount >= FreezeStackThreshold)
			{
				// Apply freeze
				const FGameplayEffectSpecHandle FreezeSpec = TargetASC->MakeOutgoingSpec(FreezeEffectClass, 1.f, EffectCtx);
				TargetASC->ApplyGameplayEffectSpecToSelf(*FreezeSpec.Data.Get());
			}
		}
	}
}
