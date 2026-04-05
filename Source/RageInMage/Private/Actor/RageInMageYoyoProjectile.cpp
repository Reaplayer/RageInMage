// Copyright Reaplays

#include "Actor/RageInMageYoyoProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Actor/RageInMageZone.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


ARageInMageYoyoProjectile::ARageInMageYoyoProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_GameTraceChannel1);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ARageInMageYoyoProjectile::BeginPlay()
{
	Super::BeginPlay();

	Sphere->IgnoreActorWhenMoving(GetInstigator(), true);
	SetLifeSpan(LifeSpan);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ARageInMageYoyoProjectile::OnSphereOverlap);
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());

	// Apply initial visual scale
	SetActorScale3D(FVector(CurrentScaleMultiplier));
}

void ARageInMageYoyoProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHit) return;

	// Apply pull acceleration toward PullOrigin
	if (bPullActive)
	{
		const FVector ToPullOrigin = PullOrigin - GetActorLocation();
		const float DistToOrigin = ToPullOrigin.Size();

		// Check if we've returned close enough — deactivate pull
		if (DistToOrigin <= OriginReturnRadius)
		{
			bPullActive = false;
		}
		else
		{
			const FVector PullAcceleration = ToPullOrigin.GetSafeNormal() * PullStrength;
			CurrentVelocity += PullAcceleration * DeltaTime;
		}
	}

	// Move
	const FVector DeltaMove = CurrentVelocity * DeltaTime;
	AddActorWorldOffset(DeltaMove, true);

	// Orient along velocity
	if (!CurrentVelocity.IsNearlyZero())
	{
		SetActorRotation(CurrentVelocity.Rotation());
	}
}

void ARageInMageYoyoProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARageInMageYoyoProjectile, CurrentVelocity);
	DOREPLIFETIME(ARageInMageYoyoProjectile, PullOrigin);
	DOREPLIFETIME(ARageInMageYoyoProjectile, bPullActive);
	DOREPLIFETIME(ARageInMageYoyoProjectile, CurrentScaleMultiplier);
}

void ARageInMageYoyoProjectile::Destroyed()
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

void ARageInMageYoyoProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
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
		// Apply damage
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			if (DamageEffectSpecHandle.IsValid() && DamageEffectSpecHandle.Data.IsValid())
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
			}
		}

		// Knockback
		if (KnockbackStrength > 0.f)
		{
			if (ACharacter* TargetChar = Cast<ACharacter>(OtherActor))
			{
				FVector PushDir = (OtherActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
				PushDir.Z = FMath::Clamp(KnockbackUpwardForce, 0.f, 1.f);
				PushDir.Normalize();
				TargetChar->LaunchCharacter(PushDir * KnockbackStrength, true, true);
			}
		}

		// Spawn puddle zone at ground level
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
		SpawnPuddleZone(GroundLocation);

		// Broadcast impact event before destruction
		OnYoyoImpact.Broadcast();
		Destroy();
	}
}

void ARageInMageYoyoProjectile::SpawnPuddleZone(const FVector& Location)
{
	if (!PuddleZoneClass) return;

	FTransform ZoneTransform;
	ZoneTransform.SetLocation(Location);

	ARageInMageZone* Zone = GetWorld()->SpawnActorDeferred<ARageInMageZone>(
		PuddleZoneClass, ZoneTransform,
		GetOwner(),
		GetInstigator(),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (Zone)
	{
		Zone->DamageEffectSpecHandle = PuddleDamageEffectSpecHandle;
		Zone->ZoneRadius = PuddleZoneRadius;
		Zone->ZoneDuration = PuddleZoneDuration;
		Zone->DamageTickInterval = PuddleZoneTickInterval;
		if (PuddleZoneEffect) Zone->ZoneEffect = PuddleZoneEffect;
		if (PuddleZoneLoopSound) Zone->ZoneLoopSound = PuddleZoneLoopSound;
		Zone->FinishSpawning(ZoneTransform);
	}
}
