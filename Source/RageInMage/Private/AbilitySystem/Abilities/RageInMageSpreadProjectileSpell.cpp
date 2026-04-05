// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageSpreadProjectileSpell.h"

#include "Interaction/CombatInterface.h"


void URageInMageSpreadProjectileSpell::SpawnSpreadProjectiles(
	const FVector& TargetLocation, const FGameplayTag& SocketTag,
	TSubclassOf<AActor> ProjectileClass, bool bCalculatePitch)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority()) return;

	const int32 Count = FMath::RoundToInt(ProjectileCount.GetValueAtLevel(GetAbilityLevel()));
	if (Count <= 0 || !ProjectileClass) return;

	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
		AvatarActor, SocketTag);

	// Base direction toward target (horizontal only)
	FVector BaseDirection = TargetLocation - SocketLocation;
	const float TargetDistance = FVector(BaseDirection.X, BaseDirection.Y, 0.f).Size();
	BaseDirection.Z = 0.f;
	BaseDirection = BaseDirection.GetSafeNormal();

	if (BaseDirection.IsNearlyZero())
	{
		BaseDirection = AvatarActor->GetActorForwardVector();
	}

	for (int32 i = 0; i < Count; ++i)
	{
		float AngleOffset = 0.f;
		if (Count > 1)
		{
			// Evenly distribute across the cone: from -ConeHalfAngle to +ConeHalfAngle
			AngleOffset = FMath::Lerp(-ConeHalfAngle, ConeHalfAngle,
				static_cast<float>(i) / static_cast<float>(Count - 1));
		}

		// Rotate direction around Z axis by the angle offset
		const FVector SpreadDirection = BaseDirection.RotateAngleAxis(AngleOffset, FVector::UpVector);
		FVector SpreadTarget = SocketLocation + SpreadDirection * TargetDistance;
		SpreadTarget.Z = TargetLocation.Z;

		// Use parent's SpawnProjectile — each gets its own damage spec, knockback, etc.
		SpawnProjectile(SpreadTarget, SocketTag, ProjectileClass, bCalculatePitch);
	}
}
