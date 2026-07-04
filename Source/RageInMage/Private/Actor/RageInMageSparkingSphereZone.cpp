// Copyright Reaplays

#include "Actor/RageInMageSparkingSphereZone.h"

#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/ProjectileMovementComponent.h"


ARageInMageSparkingSphereZone::ARageInMageSparkingSphereZone()
{
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 400.f;
	ProjectileMovement->MaxSpeed = 400.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->bRotationFollowsVelocity = false;
}

void ARageInMageSparkingSphereZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), ZoneRadius, 16, FColor::Cyan, false, -1.f, 0, 2.f);
	}
#endif

	if (!HasAuthority()) return;

	const float Now = GetWorld()->GetTimeSeconds();

	// Drop cooldown entries for actors that no longer exist so the map can't grow unbounded.
	for (auto It = LastDamageTimeByActor.CreateIterator(); It; ++It)
	{
		if (!It.Key().IsValid())
		{
			It.RemoveCurrent();
		}
	}

	// Every frame: check who is in range right now, and damage each actor only if its own
	// per-target cooldown (DamageTickInterval) has elapsed since we last hit it. A newly
	// in-range actor has no prior timestamp, so it takes an immediate first hit.
	TArray<AActor*> Overlapping;
	ZoneCollision->GetOverlappingActors(Overlapping, APawn::StaticClass());

	for (AActor* Actor : Overlapping)
	{
		if (!IsValid(Actor) || Actor == GetInstigator()) continue;
		if (URageInMageAbilitySystemLibrary::IsBothEnemy(GetInstigator(), Actor)) continue;

		const float* LastTime = LastDamageTimeByActor.Find(Actor);
		if (!LastTime || (Now - *LastTime) >= DamageTickInterval)
		{
			ApplyDamageSpecToActor(Actor);
			LastDamageTimeByActor.Add(Actor, Now);
		}
	}
}
