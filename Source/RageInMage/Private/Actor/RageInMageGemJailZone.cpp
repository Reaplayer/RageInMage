// Copyright Reaplays

#include "Actor/RageInMageGemJailZone.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Engine/World.h"
#include "RageInMageGameplayTag.h"
#include "TimerManager.h"

ARageInMageGemJailZone::ARageInMageGemJailZone()
{
	// No damage timer and no on-entry hit - UsesBuiltinTickDamage() returns false, so the base skips both.
}

void ARageInMageGemJailZone::BeginPlay()
{
	Super::BeginPlay();

	// Server owns the jailing; the zone itself replicates for VFX.
	if (!HasAuthority()) return;

	GetWorld()->GetTimerManager().SetTimer(
		PetrifyTimerHandle, this, &ARageInMageGemJailZone::JailEnemiesInside, PetrifyDelay, false);
}

void ARageInMageGemJailZone::JailEnemiesInside()
{
	if (!HasAuthority() || !PetrifyEffect) return;

	// ActorsInside is kept live by the base's overlap events (and seeded at spawn), so by now it is
	// exactly "who is still standing in the gems" - anyone who walked out has already been removed.
	ActorsInside.RemoveAll([](const TObjectPtr<AActor>& Actor) { return !IsValid(Actor); });

	for (const TObjectPtr<AActor>& Actor : ActorsInside)
	{
		if (!Actor || Actor == GetInstigator()) continue;
		if (URageInMageAbilitySystemLibrary::IsFriendly(GetInstigator(), Actor)) continue;

		UAbilitySystemComponent* TargetASC =
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
		if (!TargetASC) continue;

		// Same application shape as ARageInMageIceZone's slow/freeze: build the spec on the target's
		// ASC with the caster recorded as instigator, then self-apply.
		FGameplayEffectContextHandle EffectCtx = TargetASC->MakeEffectContext();
		EffectCtx.AddInstigator(GetInstigator(), GetInstigator());

		const FGameplayEffectSpecHandle PetrifySpec = TargetASC->MakeOutgoingSpec(PetrifyEffect, 1.f, EffectCtx);
		if (!PetrifySpec.IsValid()) continue;

		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
			PetrifySpec, FRageInMageGameplayTag::Get().Condition_Petrified, PetrifyDuration);
		TargetASC->ApplyGameplayEffectSpecToSelf(*PetrifySpec.Data.Get());
	}
}
