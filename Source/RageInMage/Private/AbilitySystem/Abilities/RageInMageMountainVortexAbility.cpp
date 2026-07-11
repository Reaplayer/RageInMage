// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageMountainVortexAbility.h"

#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Actor/RageInMageProjectile.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

URageInMageMountainVortexAbility::URageInMageMountainVortexAbility()
{
	// Scan/duration timers run across the ability's active lifetime - one instance per actor keeps them bound
	// to a stable 'this'.
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URageInMageMountainVortexAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(ScanTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(DurationTimerHandle);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URageInMageMountainVortexAbility::BeginMountainVortex()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar || !GetWorld())
	{
		K2_EndAbility();
		return;
	}

	// Reflecting is a gameplay change to replicated projectiles - only the server scans/reflects.
	if (Avatar->HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(
			ScanTimerHandle, this, &URageInMageMountainVortexAbility::ScanAndReflect, ScanInterval, true);
	}

	// End the vortex after its duration (runs on all roles so the ability's active window matches everywhere).
	const float Duration = FMath::Max(VortexDuration.GetValueAtLevel(GetAbilityLevel()), 0.1f);
	GetWorld()->GetTimerManager().SetTimer(
		DurationTimerHandle, this, &URageInMageMountainVortexAbility::FinishVortex, Duration, false);
}

void URageInMageMountainVortexAbility::ScanAndReflect()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	const float Radius = VortexRadius.GetValueAtLevel(GetAbilityLevel());
	const float RadiusSq = Radius * Radius;
	const FVector Origin = Avatar->GetActorLocation();

	// GetAllActorsOfClass is bulletproof for detection (projectile collision profiles vary); the projectile
	// count in play is tiny and the vortex is short-lived, so the per-scan cost is negligible.
	TArray<AActor*> Projectiles;
	UGameplayStatics::GetAllActorsOfClass(this, ARageInMageProjectile::StaticClass(), Projectiles);

	for (AActor* Actor : Projectiles)
	{
		ARageInMageProjectile* Projectile = Cast<ARageInMageProjectile>(Actor);
		if (!Projectile) continue;

		if (FVector::DistSquared(Projectile->GetActorLocation(), Origin) > RadiusSq) continue;

		AActor* Shooter = Projectile->GetInstigator();
		// No shooter, our own/ally projectile, or already reflected (instigator is now the caster) - skip.
		if (!Shooter || URageInMageAbilitySystemLibrary::IsFriendly(Avatar, Shooter)) continue;

		Projectile->ReflectFrom(Avatar);
	}

#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		DrawDebugSphere(GetWorld(), Origin, Radius, 24, FColor::Cyan, false, ScanInterval * 1.1f, 0, 1.5f);
	}
#endif
}

void URageInMageMountainVortexAbility::FinishVortex()
{
	K2_EndAbility();
}
