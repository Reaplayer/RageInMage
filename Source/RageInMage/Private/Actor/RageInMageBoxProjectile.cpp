// Copyright Reaplays

#include "Actor/RageInMageBoxProjectile.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ARageInMageBoxProjectile::ARageInMageBoxProjectile()
{
	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	Box->SetBoxExtent(FVector(16.f, 16.f, 32.f));
	Box->SetCollisionObjectType(ECC_GameTraceChannel1);
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Box->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SetRootComponent(Box);
	ProjectileMovement->SetUpdatedComponent(Box);
}

UPrimitiveComponent* ARageInMageBoxProjectile::GetCollisionComponent() const
{
	return Box;
}
