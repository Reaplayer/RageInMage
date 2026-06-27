// Copyright Reaplays

#include "Actor/RageInMageSparkingSphereZone.h"

#include "DrawDebugHelpers.h"
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
}
