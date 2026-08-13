// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageRockSolidAbility.h"

#include "DrawDebugHelpers.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Actor/RageInMageRockWall.h"


float URageInMageRockSolidAbility::GetWallSizeScalar() const
{
	return URageInMageAbilitySystemLibrary::GetImmovableMassStageScalar(
		GetAvatarActorFromActorInfo(), StanceBonusStage1, StanceBonusStage2, StanceBonusStage3);
}

ARageInMageRockWall* URageInMageRockSolidAbility::SpawnRockWall(const FVector& Location, const FVector& Direction)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority()) return nullptr;
	if (!RockWallClass) return nullptr;

	// Direction is the way the wall FACES, so the slab spans perpendicular to it — same +90 yaw
	// convention as URageInMageWallAbility so both walls read the same way when aiming.
	FVector Facing = Direction.GetSafeNormal2D();
	if (Facing.IsNearlyZero())
	{
		Facing = AvatarActor->GetActorForwardVector().GetSafeNormal2D();
	}
	FRotator SpawnRotation = Facing.Rotation();
	SpawnRotation.Yaw += 90.f;

	// Bigger stance = bigger wall. Uniform scale takes the collision box and the mesh together
	// (the same trick Slingrock uses for its rock), so nothing has to be scaled twice.
	const float SizeScalar = GetWallSizeScalar();

	const ARageInMageRockWall* WallCDO = RockWallClass.GetDefaultObject();
	const float HalfHeight = WallCDO ? WallCDO->WallHalfHeight : 150.f;

	// Lift the spawn by the SCALED half-height so the wall's base sits on the ground.
	FVector SpawnLocation = Location;
	SpawnLocation.Z += HalfHeight * SizeScalar;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);
	SpawnTransform.SetRotation(SpawnRotation.Quaternion());
	SpawnTransform.SetScale3D(FVector(SizeScalar));

	ARageInMageRockWall* Wall = GetWorld()->SpawnActorDeferred<ARageInMageRockWall>(
		RockWallClass, SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(AvatarActor),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (Wall)
	{
		// Only override the actor's own default when the ability actually specifies a duration —
		// a 0 here would reach SetLifeSpan(0), which means "lives forever", not "expires instantly".
		const float Duration = WallDuration.GetValueAtLevel(GetAbilityLevel());
		if (Duration > 0.f)
		{
			Wall->WallDuration = Duration;
		}
		Wall->FinishSpawning(SpawnTransform);
	}

	return Wall;
}

void URageInMageRockSolidAbility::DrawDebugRockWallPreview(const FVector& Location, const FVector& Direction)
{
#if ENABLE_DRAW_DEBUG
	if (!GetWorld()) return;

	const ARageInMageRockWall* WallCDO = RockWallClass ? RockWallClass.GetDefaultObject() : nullptr;
	const float HalfWidth = WallCDO ? WallCDO->WallHalfWidth : 250.f;
	const float HalfThickness = WallCDO ? WallCDO->WallHalfThickness : 40.f;
	const float HalfHeight = WallCDO ? WallCDO->WallHalfHeight : 150.f;

	const float SizeScalar = GetWallSizeScalar();

	FVector Facing = Direction.GetSafeNormal2D();
	if (Facing.IsNearlyZero())
	{
		const AActor* AvatarActor = GetAvatarActorFromActorInfo();
		Facing = AvatarActor ? AvatarActor->GetActorForwardVector().GetSafeNormal2D() : FVector::ForwardVector;
	}
	FRotator PreviewRotation = Facing.Rotation();
	PreviewRotation.Yaw += 90.f; // matches SpawnRockWall

	// Same upward offset as the spawn, so what you see is where it lands.
	FVector BoxCenter = Location;
	BoxCenter.Z += HalfHeight * SizeScalar;

	const FVector BoxExtent = FVector(HalfWidth, HalfThickness, HalfHeight) * SizeScalar;
	DrawDebugBox(GetWorld(), BoxCenter, BoxExtent, PreviewRotation.Quaternion(),
		FColor::Emerald, false, 0.05f, 0, 3.f);

	// A stub arrow along the facing normal — without it the box alone is ambiguous about which
	// way the right stick is pointing the wall.
	DrawDebugDirectionalArrow(GetWorld(), BoxCenter, BoxCenter + Facing * 150.f,
		60.f, FColor::Emerald, false, 0.05f, 0, 3.f);
#endif
}
