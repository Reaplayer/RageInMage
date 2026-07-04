// Copyright Reaplays

#include "Actor/RageInMageZone.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Components/AudioComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"


ARageInMageZone::ARageInMageZone()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	ZoneCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ZoneCollision"));
	SetRootComponent(ZoneCollision);
	ZoneCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ZoneCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	ZoneCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ZoneCollision->SetSphereRadius(ZoneRadius);
}

void ARageInMageZone::BeginPlay()
{
	Super::BeginPlay();

	// Update collision radius to match the configured zone radius
	ZoneCollision->SetSphereRadius(ZoneRadius);

#if ENABLE_DRAW_DEBUG
	DrawDebugSphere(GetWorld(), GetActorLocation(), ZoneRadius, 24, FColor::Cyan, false, ZoneDuration, 0, 2.f);
#endif

	SetLifeSpan(ZoneDuration);

	ZoneCollision->OnComponentBeginOverlap.AddDynamic(this, &ARageInMageZone::OnZoneOverlap);
	ZoneCollision->OnComponentEndOverlap.AddDynamic(this, &ARageInMageZone::OnZoneEndOverlap);

	if (HasAuthority())
	{
		// Overlap delegates only fire on state changes, so actors already standing inside
		// the zone's footprint at spawn (e.g. the radius resize above, or component
		// registration before this even ran) never generate a BeginOverlap event. Seed
		// ActorsInside with a snapshot of whoever's already overlapping right now.
		TArray<AActor*> InitialOverlaps;
		ZoneCollision->GetOverlappingActors(InitialOverlaps, APawn::StaticClass());
		for (AActor* Actor : InitialOverlaps)
		{
			if (!Actor || Actor == GetInstigator()) continue;
			if (URageInMageAbilitySystemLibrary::IsBothEnemy(GetInstigator(), Actor)) continue;
			ActorsInside.AddUnique(Actor);
		}

		// Tick damage immediately, then periodically — unless a subclass drives its own damage.
		if (UsesBuiltinTickDamage())
		{
			DamageTickEnemiesInside();
			GetWorld()->GetTimerManager().SetTimer(
				DamageTickTimerHandle, this, &ARageInMageZone::DamageTickEnemiesInside,
				DamageTickInterval, true);
		}
	}

	// VFX — spawn inactive, set user params, then activate
	if (ZoneEffect)
	{
		ZoneNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, ZoneEffect, GetActorLocation(),
			FRotator::ZeroRotator, FVector(1.f), /*bAutoDestroy=*/true,
			/*bAutoActivate=*/false);
		if (ZoneNiagaraComponent)
		{
			ZoneNiagaraComponent->SetVariableFloat(FName("ZoneRadius"), ZoneRadius);
			ZoneNiagaraComponent->SetVariableFloat(FName("ZoneDuration"), ZoneDuration);
			ZoneNiagaraComponent->Activate();
		}
	}
	if (ZoneLoopSound)
	{
		LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(ZoneLoopSound, GetRootComponent());
	}
}

void ARageInMageZone::Destroyed()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(DamageTickTimerHandle);
	}
	if (ZoneNiagaraComponent)
	{
		ZoneNiagaraComponent->Deactivate();
	}
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
	}
	Super::Destroyed();
}

void ARageInMageZone::OnZoneOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || !OtherActor) return;
	if (OtherActor == GetInstigator()) return;
	if (URageInMageAbilitySystemLibrary::IsBothEnemy(GetInstigator(), OtherActor)) return;

	ActorsInside.AddUnique(OtherActor);

	// Apply damage immediately on entry — unless a subclass drives its own damage.
	if (UsesBuiltinTickDamage())
	{
		ApplyDamageSpecToActor(OtherActor);
	}
}

void ARageInMageZone::OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ActorsInside.Remove(OtherActor);
}

void ARageInMageZone::DamageTickEnemiesInside()
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

		ApplyDamageSpecToActor(Actor);
	}
}

void ARageInMageZone::ApplyDamageSpecToActor(AActor* Actor) const
{
	if (!DamageEffectSpecHandle.IsValid() || !DamageEffectSpecHandle.Data.IsValid()) return;

	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
	{
		TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
	}
}
