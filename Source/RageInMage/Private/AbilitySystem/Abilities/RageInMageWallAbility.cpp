// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageWallAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/RageInMageFireWall.h"


void URageInMageWallAbility::SpawnWall()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority()) return;
	if (!FireWallClass) return;

	// Spawn wall in front of caster, facing the same direction
	const FVector CasterLocation = AvatarActor->GetActorLocation();
	const FRotator CasterRotation = AvatarActor->GetActorRotation();
	const FVector ForwardVector = CasterRotation.Vector();
	const FVector SpawnLocation = CasterLocation + ForwardVector * SpawnDistanceInFront;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);
	SpawnTransform.SetRotation(CasterRotation.Quaternion());

	ARageInMageFireWall* Wall = GetWorld()->SpawnActorDeferred<ARageInMageFireWall>(
		FireWallClass, SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(AvatarActor),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (Wall)
	{
		// Create damage spec
		const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddInstigator(GetOwningActorFromActorInfo(), AvatarActor);
		FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
		for (auto& Pair : DamageTypeTags)
		{
			const float ScaledValue = Pair.Value.GetValueAtLevel(GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledValue);
		}

		Wall->DamageEffectSpecHandle = SpecHandle;
		Wall->WallDuration = WallDuration.GetValueAtLevel(GetAbilityLevel());
		Wall->DamageTickInterval = WallDamageTickInterval;
		Wall->bDestroyProjectiles = bWallDestroysProjectiles;
		Wall->FinishSpawning(SpawnTransform);
	}
}
