// Copyright Reaplays


#include "Actor/RageInMageProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Actor/RageInMageDecal.h"
#include "Character/RageInMageCharacterBase.h"
#include "Actor/RageInMageFireWall.h"
#include "Actor/RageInMageWaveBox.h"
#include "Actor/RageInMageZone.h"
#include "Components/AudioComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

namespace
{
	void ApplyKnockbackToActor(AActor* Target, const FVector& Origin, float Strength, float UpwardForce)
	{
		if (Strength <= 0.f || !Target) return;
		if (ARageInMageCharacterBase* TargetChar = Cast<ARageInMageCharacterBase>(Target))
		{
			FVector PushDir = (Target->GetActorLocation() - Origin).GetSafeNormal();
			PushDir.Z = FMath::Clamp(UpwardForce, 0.f, 1.f);
			PushDir.Normalize();
			TargetChar->ApplyKnockbackImpulse(PushDir * Strength, true, true);
		}
	}
}


ARageInMageProjectile::ARageInMageProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void ARageInMageProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSpan);

	// Bind overlap on whatever collision shape the child provides
	if (UPrimitiveComponent* Collision = GetCollisionComponent())
	{
		Collision->IgnoreActorWhenMoving(GetInstigator(), true);
		Collision->OnComponentBeginOverlap.AddDynamic(this, &ARageInMageProjectile::OnProjectileOverlap);
	}

	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());

	// Pass projectile params to any Niagara components
	const float Speed = ProjectileMovement ? ProjectileMovement->Velocity.Size() : 0.f;
	{
		TArray<UNiagaraComponent*> NiagaraComponents;
		GetComponents<UNiagaraComponent>(NiagaraComponents);
		for (UNiagaraComponent* NiagaraComp : NiagaraComponents)
		{
			if (Speed > 0.f)
			{
				NiagaraComp->SetVariableFloat(FName("ProjectileSpeed"), Speed);
			}
			if (LifeSpan > 0.f)
			{
				NiagaraComp->SetVariableFloat(FName("ProjectileLifetime"), LifeSpan);
			}
		}
	}
}

void ARageInMageProjectile::Destroyed()
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

void ARageInMageProjectile::ApplyAoEDamage(const FVector& ImpactLocation)
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

void ARageInMageProjectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Ignore fire walls and wave boxes — they handle their own overlap logic
	if (Cast<ARageInMageFireWall>(OtherActor))
	{
		return;
	}
	if (Cast<ARageInMageWaveBox>(OtherActor))
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
			if (ImpactDecalClass)
			{
				// Animated decal actor (scale-in + fade)
				ARageInMageDecal::SpawnDecal(
					this, ImpactDecalClass, GroundLocation,
					ImpactDecalMaterial, ImpactDecalSize,
					ImpactDecalScaleInDuration, ImpactDecalFadeDelay, ImpactDecalFadeDuration);
			}
			else
			{
				// Fallback: raw decal component (no scale-in)
				UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(
					GetWorld(), ImpactDecalMaterial, ImpactDecalSize,
					GroundLocation, FRotator(-90.f, FMath::RandRange(0.f, 360.f), 0.f));
				if (Decal)
				{
					Decal->SetFadeScreenSize(0.f);
					Decal->SetFadeOut(ImpactDecalFadeDelay, ImpactDecalFadeDuration);
				}
			}
		}

		Destroy();
	}
	else
	{
		bHit = true;
	}
}
