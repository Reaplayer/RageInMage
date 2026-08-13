// Copyright Reaplays

#include "Actor/RageInMageRockWall.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"


ARageInMageRockWall::ARageInMageRockWall()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WallCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("WallCollision"));
	SetRootComponent(WallCollision);

	// BLOCKING, not overlapping — this is the whole point of the spell. Everything is stopped:
	// pawns (both teams), projectiles, spell traces, line of sight.
	WallCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WallCollision->SetCollisionObjectType(ECC_WorldDynamic);
	WallCollision->SetCollisionResponseToAllChannels(ECR_Block);
	// ...except the camera. The top-down spring arm probes for geometry, so a blocking wall would
	// yank the camera in on itself every time the player stood behind their own spell.
	WallCollision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	WallCollision->SetBoxExtent(FVector(250.f, 40.f, 150.f)); // synced from the Half* props in BeginPlay

	// Ask the navigation system to treat the wall as a hole so AI paths AROUND it rather than
	// grinding into it. Only takes effect if the project's navmesh runtime generation is Dynamic
	// (or Dynamic Modifiers Only) — on Static generation the wall still physically blocks, the AI
	// just won't plan around it.
	WallCollision->SetCanEverAffectNavigation(true);
	WallCollision->bDynamicObstacle = true;

	WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallMesh"));
	WallMesh->SetupAttachment(WallCollision);
	// The box does the blocking; the mesh must never fight it (same gotcha as the Slingrock mesh).
	WallMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WallMesh->SetCollisionProfileName(TEXT("NoCollision"));
	WallMesh->SetCanEverAffectNavigation(false);
}

void ARageInMageRockWall::BeginPlay()
{
	Super::BeginPlay();

	// Sync collision to the dimension properties (which the ability may have set at spawn).
	WallCollision->SetBoxExtent(FVector(WallHalfWidth, WallHalfThickness, WallHalfHeight));

	SetLifeSpan(WallDuration);

	if (SpawnEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, SpawnEffect, GetActorLocation(), GetActorRotation());
	}
	if (SpawnSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SpawnSound, GetActorLocation());
	}
}

void ARageInMageRockWall::Destroyed()
{
	if (CrumbleEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, CrumbleEffect, GetActorLocation(), GetActorRotation());
	}

	Super::Destroyed();
}
