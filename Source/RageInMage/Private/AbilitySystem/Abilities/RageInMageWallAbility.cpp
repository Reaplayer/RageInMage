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
	SpawnRotation.Yaw += 90.f; // Wall spans perpendicular to the cast direction

	// Read the wall's half-height from the CDO so we can offset the spawn upward
	const ARageInMageFireWall* WallCDO = FireWallClass.GetDefaultObject();
	const float HalfHeight = WallCDO ? WallCDO->WallHalfHeight : 150.f;

	// Offset spawn location up by HalfHeight so the box bottom sits on the ground
	FVector SpawnLocation = TargetLocation;
	SpawnLocation.Z += HalfHeight;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);
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

	const ARageInMageFireWall* WallCDO = FireWallClass ? FireWallClass.GetDefaultObject() : nullptr;
	const float HalfWidth = WallCDO ? WallCDO->WallHalfWidth : 200.f;
	const float HalfHeight = WallCDO ? WallCDO->WallHalfHeight : 150.f;

	const FVector BoxExtent(HalfWidth, 20.f, HalfHeight);
	// Offset upward so the box bottom sits on the ground (matches SpawnWallAtLocation)
	FVector BoxCenter = Location;
	BoxCenter.Z += HalfHeight;
	FRotator PreviewRotation = Rotation;
	PreviewRotation.Yaw += 90.f; // Same perpendicular offset as SpawnWallAtLocation
	DrawDebugBox(GetWorld(), BoxCenter, BoxExtent, PreviewRotation.Quaternion(), FColor::Orange, false, 0.05f, 0, 2.f);
#endif
}
