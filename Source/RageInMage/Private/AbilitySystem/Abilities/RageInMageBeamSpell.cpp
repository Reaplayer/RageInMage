// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageBeamSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "RageInMageGameplayTag.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"


void URageInMageBeamSpell::StartBeamChannel(const FVector& TargetLocation, const FGameplayTag& SocketTag)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;

	CurrentTargetLocation = TargetLocation;
	CurrentSocketTag = SocketTag;
	bIsChanneling = true;

	// Tick damage immediately, then on interval
	BeamTickDamage();
	GetWorld()->GetTimerManager().SetTimer(
		BeamTickTimerHandle, this, &URageInMageBeamSpell::BeamTickDamage, BeamTickInterval, true);
}

void URageInMageBeamSpell::StopBeamChannel()
{
	bIsChanneling = false;
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(BeamTickTimerHandle);
	}
}

void URageInMageBeamSpell::UpdateBeamTarget(const FVector& NewTargetLocation)
{
	CurrentTargetLocation = NewTargetLocation;
}

void URageInMageBeamSpell::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	StopBeamChannel();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URageInMageBeamSpell::BeamTickDamage()
{
	if (!bIsChanneling) return;

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority()) return;

	// Get beam start location from combat socket
	const FVector BeamStart = ICombatInterface::Execute_GetCombatSocketLocation(AvatarActor, CurrentSocketTag);
	FVector BeamDirection = (CurrentTargetLocation - BeamStart).GetSafeNormal();
	const FVector BeamEnd = BeamStart + BeamDirection * BeamRange;

	// Create the damage spec
	const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddInstigator(GetOwningActorFromActorInfo(), AvatarActor);
	const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
	for (auto& Pair : DamageTypeTags)
	{
		const float ScaledValue = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledValue);
	}

	// Sphere trace along the beam
	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);
	QueryParams.AddIgnoredActor(GetOwningActorFromActorInfo());

	GetWorld()->SweepMultiByChannel(
		HitResults,
		BeamStart,
		BeamEnd,
		FQuat::Identity,
		ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(BeamRadius),
		QueryParams);

	bool bAnyTargetIgnited = false;
	TSet<AActor*> AlreadyHit;

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || AlreadyHit.Contains(HitActor)) continue;
		if (URageInMageAbilitySystemLibrary::IsBothEnemy(AvatarActor, HitActor)) continue;

		AlreadyHit.Add(HitActor);

		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor))
		{
			// Check if this target is Ignited (has the Ignited tag)
			if (IgnitedTag.IsValid() && TargetASC->HasMatchingGameplayTag(IgnitedTag))
			{
				bAnyTargetIgnited = true;
			}

			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}

		// If no target is Ignited, stop at the first hit (no piercing)
		if (!bAnyTargetIgnited)
		{
			break;
		}
	}
}
