// Copyright Reaplays


#include "Actor/RageInMageSphereProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


ARageInMageSphereProjectile::ARageInMageSphereProjectile()
{
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_GameTraceChannel1);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ProjectileMovement->SetUpdatedComponent(Sphere);
}

UPrimitiveComponent* ARageInMageSphereProjectile::GetCollisionComponent() const
{
	return Sphere;
}
