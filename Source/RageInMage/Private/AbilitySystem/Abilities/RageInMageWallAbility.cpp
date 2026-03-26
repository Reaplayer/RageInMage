// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageWallAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Actor/RageInMageFireWall.h"


void URageInMageWallAbility::SpawnWallAtLocation(const FVector& TargetLocation)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority()) return;
	if (!FireWallClass) return;

	// Wall spawns at the target location, oriented perpendicular to a caster->target direction
	const FVector CasterLocation = AvatarActor->GetActorLocation();
	FVector Direction = TargetLocation - CasterLocation;
	Direction.Z = 0.f;
	FRotator SpawnRotation = Direction.GetSafeNormal().Rotation();

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(TargetLocation);
	SpawnTransform.SetRotation(SpawnRotation.Quaternion());

	ARageInMageFireWall* Wall = GetWorld()->SpawnActorDeferred<ARageInMageFireWall>(
		FireWallClass, SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(AvatarActor),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (Wall)
	{
		// Create a damage spec
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
		Wall->bDestroyFriendlyProjectiles = bWallDestroysFriendlyProjectiles;
		Wall->FinishSpawning(SpawnTransform);
	}
}

void URageInMageWallAbility::DrawDebugWallPreview(const FVector& Location, const FRotator& Rotation)
{
#if ENABLE_DRAW_DEBUG
	if (!GetWorld()) return;

	// Match the fire wall's box extent: (200, 20, 150) = 400 wide, 40 deep, 300 tall
	const FVector BoxExtent(200.f, 20.f, 150.f);
	FRotator PreviewRotation = Rotation;
	PreviewRotation.Yaw += 90.f; // Same perpendicular offset as SpawnWallAtLocation
	DrawDebugBox(GetWorld(), Location, BoxExtent, PreviewRotation.Quaternion(), FColor::Orange, false, 0.05f, 0, 2.f);
#endif
}
