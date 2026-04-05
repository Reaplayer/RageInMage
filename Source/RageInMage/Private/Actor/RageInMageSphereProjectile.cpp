// Copyright Reaplays


#include "Actor/RageInMageSphereProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Actor/RageInMageFireWall.h"
#include "Actor/RageInMageZone.h"
#include "Components/AudioComponent.h"
#include "Components/DecalComponent.h"
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

	// Pass projectile speed to any Niagara components that have a ProjectileSpeed user param
	const float Speed = ProjectileMovement ? ProjectileMovement->Velocity.Size() : 0.f;
	if (Speed > 0.f)
	{
		TArray<UNiagaraComponent*> NiagaraComponents;
		GetComponents<UNiagaraComponent>(NiagaraComponents);
		for (UNiagaraComponent* NiagaraComp : NiagaraComponents)
		{
			NiagaraComp->SetVariableFloat(FName("ProjectileSpeed"), Speed);
		}
	}
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
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, AoEExplosionEffect, ImpactLocation,
			FRotator::ZeroRotator, FVector(1.f), /*bAutoDestroy=*/true,
			/*bAutoActivate=*/false);
		if (NiagaraComp)
		{
			NiagaraComp->SetVariableFloat(FName("ExplosionRadius"), AoERadius);
			NiagaraComp->Activate();
		}
	}
}

void ARageInMageSphereProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Ignore fire walls — the wall's own overlap logic decides whether to destroy this projectile
	if (Cast<ARageInMageFireWall>(OtherActor))
	{
		return;
	}
	if (!DamageEffectSpecHandle.IsValid() || OtherActor == GetInstigator())
	{
		return;
	}
	if (URageInMageAbilitySystemLibrary::IsBothEnemy(GetInstigator(), OtherActor))
	{
		return;
	}

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
		// Trace down to find the ground below impact (used by zone + decal)
		FVector GroundLocation = GetActorLocation();
		{
			FHitResult GroundHit;
			const FVector TraceStart = GroundLocation;
			const FVector TraceEnd = GroundLocation - FVector(0.f, 0.f, 1000.f);
			if (GetWorld()->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_Visibility))
			{
				GroundLocation = GroundHit.ImpactPoint;
			}
		}

		// Spawn zone at ground level
		if (ZoneClass)
		{
			FTransform ZoneTransform;
			ZoneTransform.SetLocation(GroundLocation);

			ARageInMageZone* Zone = GetWorld()->SpawnActorDeferred<ARageInMageZone>(
				ZoneClass, ZoneTransform,
				GetOwner(),
				GetInstigator(),
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

			if (Zone)
			{
				Zone->DamageEffectSpecHandle = ZoneDamageEffectSpecHandle;
				Zone->ZoneRadius = ZoneRadius;
				Zone->ZoneDuration = ZoneDuration;
				Zone->DamageTickInterval = ZoneTickInterval;
				if (ZoneEffect) Zone->ZoneEffect = ZoneEffect;
				if (ZoneLoopSound) Zone->ZoneLoopSound = ZoneLoopSound;
				Zone->FinishSpawning(ZoneTransform);
			}
		}

		// Spawn impact scorch decal at ground level
		if (ImpactDecalMaterial)
		{
			UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(
				GetWorld(), ImpactDecalMaterial, ImpactDecalSize,
				GroundLocation, FRotator(-90.f, FMath::RandRange(0.f, 360.f), 0.f));
			if (Decal)
			{
				Decal->SetFadeScreenSize(0.f);
				Decal->SetFadeOut(ImpactDecalFadeDelay, ImpactDecalFadeDuration);
			}
		}

		Destroy();
	}
	else
	{
		bHit = true;
	}
}

