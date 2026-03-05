// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageLeapAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"


void URageInMageLeapAbility::ApplyLandingExplosion()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority()) return;

	const FVector ExplosionCenter = AvatarActor->GetActorLocation();
	const float Radius = ExplosionRadius.GetValueAtLevel(GetAbilityLevel());
	const float Pushback = PushbackStrength.GetValueAtLevel(GetAbilityLevel());

	// VFX and SFX
	if (ExplosionEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(AvatarActor, ExplosionEffect, ExplosionCenter);
	}
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(AvatarActor, ExplosionSound, ExplosionCenter);
	}

	// Find all enemies in range
	TArray<AActor*> OverlappingActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(AvatarActor);
	URageInMageAbilitySystemLibrary::GetLivePlayersWithinRadius(
		AvatarActor, Radius, OverlappingActors, ActorsToIgnore, ExplosionCenter);

	// Create damage spec
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

	for (AActor* Target : OverlappingActors)
	{
		if (URageInMageAbilitySystemLibrary::IsBothEnemy(AvatarActor, Target)) continue;

		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}

		// Apply pushback — direction away from explosion center, scaled by distance
		if (ACharacter* TargetCharacter = Cast<ACharacter>(Target))
		{
			FVector PushDirection = (Target->GetActorLocation() - ExplosionCenter).GetSafeNormal();
			PushDirection.Z = 0.3f; // Slight upward push
			PushDirection.Normalize();
			TargetCharacter->LaunchCharacter(PushDirection * Pushback, true, true);
		}
	}
}
