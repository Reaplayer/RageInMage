// Copyright Reaplays

#include "Actor/RageInMageSpikeField.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"


ARageInMageSpikeField::ARageInMageSpikeField()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SpikeCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("SpikeCollision"));
	SetRootComponent(SpikeCollision);

	// Starts inert; EnableBlocking() turns it solid after the eruption push (see the header).
	SpikeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpikeCollision->SetCollisionObjectType(ECC_WorldDynamic);
	SpikeCollision->SetCollisionResponseToAllChannels(ECR_Block);
	// The top-down spring arm probes for geometry — leave the camera alone or it yanks in whenever
	// the player stands behind their own spikes.
	SpikeCollision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	SpikeCollision->SetBoxExtent(FVector(350.f, 350.f, 100.f)); // synced from the props in BeginPlay

	// Let AI path around the spikes. Only bites if the project's navmesh runtime generation is
	// Dynamic (or Dynamic Modifiers Only); on Static generation they still block physically.
	SpikeCollision->SetCanEverAffectNavigation(true);
	SpikeCollision->bDynamicObstacle = true;

	SpikeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpikeMesh"));
	SpikeMesh->SetupAttachment(SpikeCollision);
	// The box does the blocking; the mesh must never fight it (same gotcha as the Slingrock mesh).
	SpikeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpikeMesh->SetCollisionProfileName(TEXT("NoCollision"));
	SpikeMesh->SetCanEverAffectNavigation(false);
}

void ARageInMageSpikeField::BeginPlay()
{
	Super::BeginPlay();

	SpikeCollision->SetBoxExtent(FVector(FieldHalfExtent, FieldHalfExtent, FieldHalfHeight));

	SetLifeSpan(FieldDuration);

	if (BlockingActivationDelay > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			BlockingTimerHandle, this, &ARageInMageSpikeField::EnableBlocking,
			BlockingActivationDelay, false);
	}
	else
	{
		EnableBlocking();
	}

	if (EruptEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, EruptEffect, GetActorLocation(), GetActorRotation());
	}
	if (EruptSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EruptSound, GetActorLocation());
	}
}

void ARageInMageSpikeField::EnableBlocking()
{
	SpikeCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}
