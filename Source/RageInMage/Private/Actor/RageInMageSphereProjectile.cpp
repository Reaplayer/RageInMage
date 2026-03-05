// Copyright Reaplays


#include "Actor/RageInMageSphereProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Actor/RageInMageFireZone.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

namespace
{
	void ApplyKnockbackToActor(AActor* Target, const FVector& Origin, float Strength, float UpwardForce)
	{
		if (Strength <= 0.f || !Target) return;
		if (ACharacter* TargetChar = Cast<ACharacter>(Target))
		{
			FVector PushDir = (Target->GetActorLocation() - Origin).GetSafeNormal();
			PushDir.Z = FMath::Clamp(UpwardForce, 0.f, 1.f);
			PushDir.Normalize();
			TargetChar->LaunchCharacter(PushDir * Strength, true, true);
		}
	}
}


ARageInMageSphereProjectile::ARageInMageSphereProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_GameTraceChannel1);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void ARageInMageSphereProjectile::BeginPlay()
{
	Super::BeginPlay();
	Sphere->IgnoreActorWhenMoving(GetInstigator(),true);
	SetLifeSpan(LifeSpan);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ARageInMageSphereProjectile::OnSphereOverlap);
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
}

void ARageInMageSphereProjectile::Destroyed()
{
	if (!bHit && !HasAuthority())
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation(), FRotator::ZeroRotator);
		if (LoopingSoundComponent) LoopingSoundComponent->Stop();
		bHit = true;
	}
	Super::Destroyed();
}

void ARageInMageSphereProjectile::ApplyAoEDamage(const FVector& ImpactLocation)
{
	if (!DamageEffectSpecHandle.IsValid() || !DamageEffectSpecHandle.Data.IsValid()) return;

	TArray<AActor*> OverlappingActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetInstigator());

	URageInMageAbilitySystemLibrary::GetLivePlayersWithinRadius(
		this, AoERadius, OverlappingActors, ActorsToIgnore, ImpactLocation);

	for (AActor* Target : OverlappingActors)
	{
		if (URageInMageAbilitySystemLibrary::IsBothEnemy(GetInstigator(), Target)) continue;

		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
		ApplyKnockbackToActor(Target, ImpactLocation, KnockbackStrength, KnockbackUpwardForce);
	}

	if (AoEExplosionEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, AoEExplosionEffect, ImpactLocation, FRotator::ZeroRotator);
	}
}

void ARageInMageSphereProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!DamageEffectSpecHandle.IsValid() || OtherActor == GetInstigator()) return;
	if (URageInMageAbilitySystemLibrary::IsBothEnemy(GetInstigator(), OtherActor)) return;
	if (!bHit)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation(), FRotator::ZeroRotator);
		if (LoopingSoundComponent) LoopingSoundComponent->Stop();
		bHit = true;
	}

	if (HasAuthority())
	{
		if (AoERadius > 0.f)
		{
			ApplyAoEDamage(GetActorLocation());
		}
		else
		{
			if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
			{
				if (DamageEffectSpecHandle.IsValid() && DamageEffectSpecHandle.Data.IsValid())
				{
					TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
				}
			}
			ApplyKnockbackToActor(OtherActor, GetActorLocation(), KnockbackStrength, KnockbackUpwardForce);
		}
		// Spawn fire zone at impact location if configured
		if (FireZoneClass)
		{
			FTransform ZoneTransform;
			ZoneTransform.SetLocation(GetActorLocation());

			ARageInMageFireZone* FireZone = GetWorld()->SpawnActorDeferred<ARageInMageFireZone>(
				FireZoneClass, ZoneTransform,
				GetOwner(),
				GetInstigator(),
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

			if (FireZone)
			{
				FireZone->DamageEffectSpecHandle = FireZoneDamageEffectSpecHandle;
				FireZone->ZoneRadius = FireZoneRadius;
				FireZone->ZoneDuration = FireZoneDuration;
				FireZone->DamageTickInterval = FireZoneTickInterval;
				FireZone->FinishSpawning(ZoneTransform);
			}
		}

		Destroy();
	}
	else
	{
		bHit = true;
	}
}

