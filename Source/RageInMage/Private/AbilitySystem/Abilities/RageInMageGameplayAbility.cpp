// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "Player/RageInMagePlayerController.h"
#include "RageInMageGameplayTag.h"

URageInMageGameplayAbility::URageInMageGameplayAbility()
{
	// All abilities are blocked while the caster is channeling another spell.
	// Channeling abilities grant Status.Channeling via ActivationOwnedTags.
	ActivationBlockedTags.AddTag(FRageInMageGameplayTag::Get().Status_Channeling);

	// Every incapacitating condition blocks actions, not just Stunned - these are the Condition.* tags
	// documented as "unable to Move or make any Action". Their movement half is handled by
	// ARageInMageCharacterBase::CrowdControlTagChanged; this is the action half.
	const FRageInMageGameplayTag& Tags = FRageInMageGameplayTag::Get();
	ActivationBlockedTags.AddTag(Tags.Condition_Stunned);
	ActivationBlockedTags.AddTag(Tags.Condition_Frozen);
	ActivationBlockedTags.AddTag(Tags.Condition_Petrified);
	ActivationBlockedTags.AddTag(Tags.Condition_Grappled);
	ActivationBlockedTags.AddTag(Tags.Condition_Constricted);
	ActivationBlockedTags.AddTag(Tags.Condition_Paralysed);
	ActivationBlockedTags.AddTag(Tags.Condition_Shocked);

	// Silenced blocks actions only - movement is unaffected.
	ActivationBlockedTags.AddTag(Tags.Condition_Silenced);
}

FVector URageInMageGameplayAbility::GetAimWorldPosition() const
{
	if (const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo())
	{
		if (const ARageInMagePlayerController* MagePC = Cast<ARageInMagePlayerController>(ActorInfo->PlayerController.Get()))
		{
			return MagePC->GetCurrentAimWorldPosition();
		}
	}

	// Fallback: project forward from avatar
	if (const AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		return Avatar->GetActorLocation() + Avatar->GetActorForwardVector() * 500.f;
	}
	return FVector::ZeroVector;
}

float URageInMageGameplayAbility::GetEffectiveCooldownReduction() const
{
	if (!bAffectedByCooldownReduction) return 0.f;

	const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return 0.f;

	bool bFound = false;
	const float Reduction = ASC->GetGameplayAttributeValue(URageInMageAttributeSet::GetCooldownReductionAttribute(), bFound);
	if (!bFound) return 0.f;

	return FMath::Clamp(Reduction, 0.f, FMath::Clamp(MaxCooldownReductionPercent, 0.f, 100.f));
}

void URageInMageGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (!CooldownGE) return;

	const float Reduction = GetEffectiveCooldownReduction();
	if (FMath::IsNearlyZero(Reduction))
	{
		Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo);
		return;
	}

	// Build the spec ourselves so the authored duration can be scaled before the effect is applied.
	// This keeps every GE_Cooldown_* asset untouched — whatever duration it computes is the "base" cooldown.
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(
		Handle, ActorInfo, ActivationInfo, CooldownGE->GetClass(), GetAbilityLevel(Handle, ActorInfo));

	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	if (!Spec)
	{
		Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo);
		return;
	}

	const float BaseDuration = Spec->GetDuration();
	// Only scale finite durations — instant/infinite cooldown effects have nothing meaningful to reduce.
	if (BaseDuration > 0.f)
	{
		const float ReducedDuration = FMath::Max(MinCooldownAfterReduction, BaseDuration * (1.f - Reduction / 100.f));
		// bLockDuration so the aggregated duration isn't recalculated back to the authored value.
		Spec->SetDuration(FMath::Min(ReducedDuration, BaseDuration), true);
	}

	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
}
