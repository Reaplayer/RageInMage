// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageShieldAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"


void URageInMageShieldAbility::ActivateShield()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!AvatarActor || !ASC) return;

	// Apply the shield GE (grants Status.Shielded tag)
	if (ShieldEffectClass)
	{
		const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(ShieldEffectClass, GetAbilityLevel());
		ActiveShieldEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}

	// Bind to the AttributeSet's OnShieldAbsorbedDamage delegate
	if (const URageInMageAttributeSet* AS = Cast<URageInMageAttributeSet>(ASC->GetAttributeSet(URageInMageAttributeSet::StaticClass())))
	{
		// const_cast needed — delegate is mutable on the AttributeSet, but GetAttributeSet returns const
		URageInMageAttributeSet* MutableAS = const_cast<URageInMageAttributeSet*>(AS);
		ShieldAbsorbDelegateHandle = MutableAS->OnShieldAbsorbedDamage.AddUObject(
			this, &URageInMageShieldAbility::HandleShieldAbsorbed);
	}

	// Start expiration timer
	if (AvatarActor->GetWorld())
	{
		AvatarActor->GetWorld()->GetTimerManager().SetTimer(
			ShieldExpirationTimerHandle,
			FTimerDelegate::CreateUObject(this, &URageInMageShieldAbility::HandleShieldExpired),
			ShieldDuration, false);
	}

	// VFX / SFX
	if (ShieldFormSound)
	{
		UGameplayStatics::PlaySoundAtLocation(AvatarActor, ShieldFormSound, AvatarActor->GetActorLocation());
	}

	OnShieldActivated();
}

void URageInMageShieldAbility::HandleShieldAbsorbed(float AbsorbedAmount)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return;

	// Clear the expiration timer — the shield has been consumed
	if (AvatarActor->GetWorld())
	{
		AvatarActor->GetWorld()->GetTimerManager().ClearTimer(ShieldExpirationTimerHandle);
	}

	RemoveShieldEffect();
	OnShieldAbsorbed(AbsorbedAmount);

	// Convert absorbed damage to healing
	const float HealAmount = AbsorbedAmount * HealConversionRate;
	if (HealAmount > 0.f)
	{
		ApplyAbsorbHeal(HealAmount);
	}

	// Pop sound
	if (ShieldPopSound)
	{
		UGameplayStatics::PlaySoundAtLocation(AvatarActor, ShieldPopSound, AvatarActor->GetActorLocation());
	}

	// Heal VFX
	if (HealBurstEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			AvatarActor, HealBurstEffect, AvatarActor->GetActorLocation());
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URageInMageShieldAbility::HandleShieldExpired()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();

	RemoveShieldEffect();
	OnShieldExpired();

	// Pop sound (quieter expiration — same sound, BP can differentiate via OnShieldExpired)
	if (ShieldPopSound && AvatarActor)
	{
		UGameplayStatics::PlaySoundAtLocation(AvatarActor, ShieldPopSound, AvatarActor->GetActorLocation());
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URageInMageShieldAbility::RemoveShieldEffect()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	// Remove the shield GE
	if (ActiveShieldEffectHandle.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(ActiveShieldEffectHandle);
		ActiveShieldEffectHandle.Invalidate();
	}

	// Unbind from the AttributeSet delegate
	if (ShieldAbsorbDelegateHandle.IsValid())
	{
		if (const URageInMageAttributeSet* AS = Cast<URageInMageAttributeSet>(ASC->GetAttributeSet(URageInMageAttributeSet::StaticClass())))
		{
			URageInMageAttributeSet* MutableAS = const_cast<URageInMageAttributeSet*>(AS);
			MutableAS->OnShieldAbsorbedDamage.Remove(ShieldAbsorbDelegateHandle);
		}
		ShieldAbsorbDelegateHandle.Reset();
	}
}

void URageInMageShieldAbility::ApplyAbsorbHeal(float AbsorbedAmount)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!AvatarActor || !ASC || !HealEffectClass || !HealMagnitudeTag.IsValid()) return;

	// Create heal spec with SetByCaller magnitude
	const FGameplayEffectSpecHandle HealSpec = MakeOutgoingGameplayEffectSpec(HealEffectClass, GetAbilityLevel());
	if (!HealSpec.IsValid()) return;

	HealSpec.Data->SetSetByCallerMagnitude(HealMagnitudeTag, AbsorbedAmount);

	// Heal self
	ASC->ApplyGameplayEffectSpecToSelf(*HealSpec.Data.Get());

	// Heal nearby friendlies
	TArray<AActor*> NearbyActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(AvatarActor);

	URageInMageAbilitySystemLibrary::GetLivePlayersWithinRadius(
		AvatarActor, HealRadius, NearbyActors, ActorsToIgnore, AvatarActor->GetActorLocation());

	for (AActor* NearbyActor : NearbyActors)
	{
		if (!URageInMageAbilitySystemLibrary::IsFriendly(AvatarActor, NearbyActor)) continue;

		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(NearbyActor))
		{
			// Create a fresh spec for each target so GAS handles it cleanly
			const FGameplayEffectSpecHandle FriendlyHealSpec = MakeOutgoingGameplayEffectSpec(HealEffectClass, GetAbilityLevel());
			if (FriendlyHealSpec.IsValid())
			{
				FriendlyHealSpec.Data->SetSetByCallerMagnitude(HealMagnitudeTag, AbsorbedAmount);
				ASC->ApplyGameplayEffectSpecToTarget(*FriendlyHealSpec.Data.Get(), TargetASC);
			}
		}
	}
}
