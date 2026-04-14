// Copyright Reaplays

#include "Actor/RageInMageFireWall.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Actor/RageInMageProjectile.h"
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
	WallCollision->SetBoxExtent(FVector(200.f, 20.f, 150.f)); // default; synced from WallHalfWidth/WallHalfHeight in BeginPlay
}

void ARageInMageFireWall::SetDamageSpec(const FGameplayEffectSpecHandle& InSpecHandle)
{
	DamageEffectSpecHandle = InSpecHandle;
}

void ARageInMageFireWall::BeginPlay()
{
	Super::BeginPlay();

	// Sync collision box to match dimension properties
	WallCollision->SetBoxExtent(FVector(WallHalfWidth, 20.f, WallHalfHeight));

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

	// Spawn VFX with parameter passthrough (spawn inactive, set params, then activate)
	if (WallEffect)
	{
		// Offset VFX down by HalfHeight so it sits at ground level
		// (the actor origin is at box center, HalfHeight above the ground)
		WallNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			WallEffect, GetRootComponent(), NAME_None,
			FVector(0.f, 0.f, -WallHalfHeight), FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset, /*bAutoActivate=*/false);
		if (WallNiagaraComponent)
		{
			WallNiagaraComponent->SetVariableFloat(FName("WallWidth"), WallHalfWidth * 2.f);
			WallNiagaraComponent->SetVariableFloat(FName("WallHeight"), WallHalfHeight * 2.f);
			WallNiagaraComponent->SetVariableFloat(FName("WallDuration"), WallDuration);
			WallNiagaraComponent->Activate();
		}
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
		if (ARageInMageProjectile* Projectile = Cast<ARageInMageProjectile>(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("WALL OVERLAP: Wall Instigator=[%s], Projectile Instigator=[%s], OtherActor=[%s]"),
				GetInstigator() ? *GetInstigator()->GetName() : TEXT("NULL"),
				Projectile->GetInstigator() ? *Projectile->GetInstigator()->GetName() : TEXT("NULL"),
				*OtherActor->GetName());

			const bool bIsFriendlyProjectile = URageInMageAbilitySystemLibrary::IsFriendly(GetInstigator(), Projectile->GetInstigator());

			UE_LOG(LogTemp, Warning, TEXT("WALL OVERLAP: IsFriendly=%s, bDestroyFriendlyProjectiles=%s -> %s"),
				bIsFriendlyProjectile ? TEXT("TRUE") : TEXT("FALSE"),
				bDestroyFriendlyProjectiles ? TEXT("TRUE") : TEXT("FALSE"),
				(bIsFriendlyProjectile && !bDestroyFriendlyProjectiles) ? TEXT("PASS THROUGH") : TEXT("DESTROYING"));

			if (bIsFriendlyProjectile && !bDestroyFriendlyProjectiles)
			{
				// Friendly projectile passes through
				return;
			}
			Projectile->Destroy();
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

	// Clean up stale refs
	ActorsInside.RemoveAll([](const TObjectPtr<AActor>& Actor) { return !IsValid(Actor); });

	// Copy the array — applying damage can kill actors, triggering OnZoneEndOverlap
	// which modifies ActorsInside during iteration
	TArray<TObjectPtr<AActor>> ActorsToProcess = ActorsInside;

	for (AActor* Actor : ActorsToProcess)
	{
		if (!IsValid(Actor)) continue;
		if (URageInMageAbilitySystemLibrary::IsBothEnemy(GetInstigator(), Actor)) continue;

		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
	}
}
