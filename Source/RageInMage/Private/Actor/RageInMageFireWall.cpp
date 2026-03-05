// Copyright Reaplays

#include "Actor/RageInMageFireWall.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Actor/RageInMageSphereProjectile.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"


ARageInMageFireWall::ARageInMageFireWall()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WallCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("WallCollision"));
	SetRootComponent(WallCollision);
	WallCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WallCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	WallCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	WallCollision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	// Also overlap projectiles (GameTraceChannel1)
	WallCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	WallCollision->SetBoxExtent(FVector(200.f, 20.f, 150.f));
}

void ARageInMageFireWall::SetDamageSpec(const FGameplayEffectSpecHandle& InSpecHandle)
{
	DamageEffectSpecHandle = InSpecHandle;
}

void ARageInMageFireWall::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(WallDuration);

	WallCollision->OnComponentBeginOverlap.AddDynamic(this, &ARageInMageFireWall::OnWallOverlap);
	WallCollision->OnComponentEndOverlap.AddDynamic(this, &ARageInMageFireWall::OnWallEndOverlap);

	// Start periodic damage tick
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(
			DamageTickTimerHandle, this, &ARageInMageFireWall::DamageTickEnemiesInside,
			DamageTickInterval, true, DamageTickInterval);
	}

	// Spawn VFX
	if (WallEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			WallEffect, GetRootComponent(), NAME_None,
			FVector::ZeroVector, FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset, true);
	}
	if (WallLoopSound)
	{
		LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(WallLoopSound, GetRootComponent());
	}
}

void ARageInMageFireWall::Destroyed()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(DamageTickTimerHandle);
	}
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
	}
	Super::Destroyed();
}

void ARageInMageFireWall::OnWallOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	// Destroy incoming projectiles if enabled
	if (bDestroyProjectiles)
	{
		if (ARageInMageSphereProjectile* Projectile = Cast<ARageInMageSphereProjectile>(OtherActor))
		{
			// Only destroy projectiles from enemies (not our own)
			if (!URageInMageAbilitySystemLibrary::IsFriendly(GetInstigator(), Projectile->GetInstigator()))
			{
				Projectile->Destroy();
			}
			return;
		}
	}

	// Track enemies entering the wall
	if (!HasAuthority()) return;
	if (OtherActor == GetInstigator()) return;
	if (URageInMageAbilitySystemLibrary::IsBothEnemy(GetInstigator(), OtherActor)) return;

	ActorsInside.AddUnique(OtherActor);

	// Apply damage immediately on entry
	if (DamageEffectSpecHandle.IsValid() && DamageEffectSpecHandle.Data.IsValid())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
	}
}

void ARageInMageFireWall::OnWallEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ActorsInside.Remove(OtherActor);
}

void ARageInMageFireWall::DamageTickEnemiesInside()
{
	if (!DamageEffectSpecHandle.IsValid() || !DamageEffectSpecHandle.Data.IsValid()) return;

	// Clean up stale references
	ActorsInside.RemoveAll([](const TObjectPtr<AActor>& Actor) { return !IsValid(Actor); });

	for (AActor* Actor : ActorsInside)
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
	}
}
