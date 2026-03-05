// Copyright Reaplays

#include "Actor/RageInMageFireZone.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"


ARageInMageFireZone::ARageInMageFireZone()
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

void ARageInMageFireZone::BeginPlay()
{
	Super::BeginPlay();

	// Update collision radius to match the configured zone radius
	ZoneCollision->SetSphereRadius(ZoneRadius);

	SetLifeSpan(ZoneDuration);

	ZoneCollision->OnComponentBeginOverlap.AddDynamic(this, &ARageInMageFireZone::OnZoneOverlap);
	ZoneCollision->OnComponentEndOverlap.AddDynamic(this, &ARageInMageFireZone::OnZoneEndOverlap);

	if (HasAuthority())
	{
		// Tick damage immediately, then periodically
		DamageTickEnemiesInside();
		GetWorld()->GetTimerManager().SetTimer(
			DamageTickTimerHandle, this, &ARageInMageFireZone::DamageTickEnemiesInside,
			DamageTickInterval, true);
	}

	// VFX
	if (ZoneEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ZoneEffect, GetActorLocation());
	}
	if (ZoneLoopSound)
	{
		LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(ZoneLoopSound, GetRootComponent());
	}
}

void ARageInMageFireZone::Destroyed()
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

void ARageInMageFireZone::OnZoneOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || !OtherActor) return;
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

void ARageInMageFireZone::OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ActorsInside.Remove(OtherActor);
}

void ARageInMageFireZone::DamageTickEnemiesInside()
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
