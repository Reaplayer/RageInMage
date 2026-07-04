// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageStaticMegaSurgeAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "NiagaraFunctionLibrary.h"

void URageInMageStaticMegaSurgeAbility::ApplyStaticMegaSurge()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!AvatarActor || !ASC) return;

	if (BuffEffectClass)
	{
		const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(BuffEffectClass, GetAbilityLevel());
		ActiveBuffEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}

	// Bind to the AttributeSet's OnDamageReflected delegate (mirrors URageInMageShieldAbility's
	// OnShieldAbsorbedDamage binding pattern exactly).
	if (const URageInMageAttributeSet* AS = Cast<URageInMageAttributeSet>(ASC->GetAttributeSet(URageInMageAttributeSet::StaticClass())))
	{
		// const_cast needed — delegate is mutable on the AttributeSet, but GetAttributeSet returns const
		URageInMageAttributeSet* MutableAS = const_cast<URageInMageAttributeSet*>(AS);
		ReflectDelegateHandle = MutableAS->OnDamageReflected.AddUObject(
			this, &URageInMageStaticMegaSurgeAbility::HandleDamageReflected);
	}

	if (AvatarActor->GetWorld())
	{
		AvatarActor->GetWorld()->GetTimerManager().SetTimer(
			BuffExpirationTimerHandle,
			FTimerDelegate::CreateUObject(this, &URageInMageStaticMegaSurgeAbility::HandleBuffExpired),
			Duration.GetValueAtLevel(GetAbilityLevel()), false);
	}
}

void URageInMageStaticMegaSurgeAbility::HandleDamageReflected(AActor* Attacker)
{
	if (!Attacker) return;

	// Flat, non-scaling zap — same DamageEffectClass/DamageTypeTags every other Lightning damage
	// ability uses, just routed at the attacker instead of chaining onward.
	CauseDamage(Attacker);

	if (ReflectEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			Attacker, ReflectEffect, Attacker->GetActorLocation());
	}

	OnReflectProc(Attacker);
}

void URageInMageStaticMegaSurgeAbility::HandleBuffExpired()
{
	RemoveBuff();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URageInMageStaticMegaSurgeAbility::RemoveBuff()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	if (ActiveBuffEffectHandle.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(ActiveBuffEffectHandle);
		ActiveBuffEffectHandle.Invalidate();
	}

	if (ReflectDelegateHandle.IsValid())
	{
		if (const URageInMageAttributeSet* AS = Cast<URageInMageAttributeSet>(ASC->GetAttributeSet(URageInMageAttributeSet::StaticClass())))
		{
			URageInMageAttributeSet* MutableAS = const_cast<URageInMageAttributeSet*>(AS);
			MutableAS->OnDamageReflected.Remove(ReflectDelegateHandle);
		}
		ReflectDelegateHandle.Reset();
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(BuffExpirationTimerHandle);
	}
}

void URageInMageStaticMegaSurgeAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	RemoveBuff();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
