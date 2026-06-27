// Copyright Reaplays

#include "Actor/RageInMageYoyoProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Actor/RageInMageZone.h"
#include "Character/RageInMageCharacterBase.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "RageInMage/RageInMageLogChannels.h"


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

	UE_LOG(LogRageInMage, Warning, TEXT("[YOYO:SPAWN] Pos=%s  Vel=%s  Speed=%.0f  Scale=%.2f  SphereRadius=%.0f  Pull=%s  PullOrigin=%s"),
		*GetActorLocation().ToCompactString(),
		*CurrentVelocity.ToCompactString(),
		CurrentVelocity.Size(),
		CurrentScaleMultiplier,
		Sphere->GetScaledSphereRadius(),
		bPullActive ? TEXT("ON") : TEXT("OFF"),
		*PullOrigin.ToCompactString());
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

		// Track whether the yo-yo has flown out past the return radius at least once
		if (!bHasLeftOrigin)
		{
			if (DistToOrigin > OriginReturnRadius)
			{
				bHasLeftOrigin = true;
			}
		}
		else if (DistToOrigin <= OriginReturnRadius)
		{
			// Only deactivate pull after the yo-yo has left and returned
			bPullActive = false;
		}

		// Always apply pull acceleration while active
		const FVector PullAcceleration = ToPullOrigin.GetSafeNormal() * PullStrength;
		CurrentVelocity += PullAcceleration * DeltaTime;
	}

	// Clamp to horizontal plane — yoyo should never drift vertically
	CurrentVelocity.Z = 0.f;

	// Move
	const FVector DeltaMove = CurrentVelocity * DeltaTime;
	AddActorWorldOffset(DeltaMove, true);

	// Orient along velocity
	if (!CurrentVelocity.IsNearlyZero())
	{
		SetActorRotation(CurrentVelocity.Rotation());
	}

	// DEBUG: Draw sphere at projectile location so we can see it
	DrawDebugSphere(GetWorld(), GetActorLocation(), Sphere->GetScaledSphereRadius() * CurrentScaleMultiplier,
		12, FColor::Cyan, false, -1.f, 0, 2.f);
	// Also draw a line showing velocity direction
	DrawDebugLine(GetWorld(), GetActorLocation(),
		GetActorLocation() + CurrentVelocity.GetSafeNormal() * 150.f,
		bPullActive ? FColor::Blue : FColor::Green, false, -1.f, 0, 2.f);
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
	UE_LOG(LogRageInMage, Warning, TEXT("[YOYO:DESTROYED] bHit=%d  TotalHits=%d  Pos=%s  (lifespan expiry: %s)"),
		bHit ? 1 : 0, EnemiesHitCount, *GetActorLocation().ToCompactString(),
		!bHit ? TEXT("YES") : TEXT("NO"));

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

	EnemiesHitCount++;

	UE_LOG(LogRageInMage, Warning, TEXT("[YOYO:HIT] #%d  Target=%s  Pos=%s  Speed=%.0f  Scale=%.2f"),
		EnemiesHitCount,
		*OtherActor->GetName(),
		*GetActorLocation().ToCompactString(),
		CurrentVelocity.Size(),
		CurrentScaleMultiplier);

	// Log all SetByCaller magnitudes from the damage spec
	if (DamageEffectSpecHandle.IsValid() && DamageEffectSpecHandle.Data.IsValid())
	{
		const FGameplayEffectSpec& Spec = *DamageEffectSpecHandle.Data.Get();
		for (const auto& SetByCallerPair : Spec.SetByCallerTagMagnitudes)
		{
			UE_LOG(LogRageInMage, Warning, TEXT("[YOYO:HIT]   DamageTag=%s  Value=%.2f"),
				*SetByCallerPair.Key.ToString(), SetByCallerPair.Value);
		}
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
		// Apply damage
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			if (DamageEffectSpecHandle.IsValid() && DamageEffectSpecHandle.Data.IsValid())
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());

				// Log target's post-hit state
				UE_LOG(LogRageInMage, Warning, TEXT("[YOYO:HIT]   Applied GE to %s  (ASC valid)"), *OtherActor->GetName());
			}
		}
		else
		{
			UE_LOG(LogRageInMage, Warning, TEXT("[YOYO:HIT]   %s has NO ASC — damage skipped"), *OtherActor->GetName());
		}

		// Knockback
		if (KnockbackStrength > 0.f)
		{
			if (ARageInMageCharacterBase* TargetChar = Cast<ARageInMageCharacterBase>(OtherActor))
			{
				FVector PushDir = (OtherActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
				PushDir.Z = FMath::Clamp(KnockbackUpwardForce, 0.f, 1.f);
				PushDir.Normalize();
				TargetChar->ApplyKnockbackImpulse(PushDir * KnockbackStrength, true, true);
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

		UE_LOG(LogRageInMage, Warning, TEXT("[YOYO:DESTROY] Total enemies hit: %d  Final pos: %s"),
			EnemiesHitCount, *GetActorLocation().ToCompactString());

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
