// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageMeteorAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Actor/RageInMageFireZone.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


void URageInMageMeteorAbility::BeginMeteorRain(const FVector& TargetCenter)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority()) return;

	MeteorRainCenter = TargetCenter;
	MeteorsRemaining = FMath::RoundToInt(MeteorCount.GetValueAtLevel(GetAbilityLevel()));

	// Spawn the first meteor immediately, then stagger the rest
	SpawnNextMeteor();
	if (MeteorsRemaining > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(
			MeteorStaggerTimerHandle, this, &URageInMageMeteorAbility::SpawnNextMeteor,
			MeteorStaggerDelay, true);
	}
}

void URageInMageMeteorAbility::SpawnNextMeteor()
{
	if (MeteorsRemaining <= 0)
	{
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(MeteorStaggerTimerHandle);
		}
		return;
	}

	// Pick a random location within the rain area
	const float AreaRadius = RainAreaRadius.GetValueAtLevel(GetAbilityLevel());
	const FVector2D RandomPoint = FMath::RandPointInCircle(AreaRadius);
	FVector ImpactLocation = MeteorRainCenter + FVector(RandomPoint.X, RandomPoint.Y, 0.f);

	// Trace down to find ground level
	FHitResult GroundHit;
	const FVector TraceStart = ImpactLocation + FVector(0.f, 0.f, 2000.f);
	const FVector TraceEnd = ImpactLocation - FVector(0.f, 0.f, 2000.f);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetAvatarActorFromActorInfo());

	if (GetWorld()->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		ImpactLocation = GroundHit.ImpactPoint;
	}

	SpawnMeteorAtLocation(ImpactLocation);
	MeteorsRemaining--;
}

void URageInMageMeteorAbility::SpawnMeteorAtLocation(const FVector& ImpactLocation)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();

	// VFX and SFX
	if (MeteorImpactEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(AvatarActor, MeteorImpactEffect, ImpactLocation);
	}
	if (MeteorImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(AvatarActor, MeteorImpactSound, ImpactLocation);
	}

	// Apply AoE damage at impact
	const float ExplosionRadius = MeteorExplosionRadius.GetValueAtLevel(GetAbilityLevel());
	FGameplayEffectSpecHandle SpecHandle = MakeDamageSpec();

	TArray<AActor*> OverlappingActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(AvatarActor);
	URageInMageAbilitySystemLibrary::GetLivePlayersWithinRadius(
		AvatarActor, ExplosionRadius, OverlappingActors, ActorsToIgnore, ImpactLocation);

	for (AActor* Target : OverlappingActors)
	{
		if (URageInMageAbilitySystemLibrary::IsBothEnemy(AvatarActor, Target)) continue;

		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
		ApplyKnockback(Target, ImpactLocation);
	}

	// Spawn fire zone at impact location
	if (FireZoneClass)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(ImpactLocation);

		ARageInMageFireZone* FireZone = GetWorld()->SpawnActorDeferred<ARageInMageFireZone>(
			FireZoneClass, SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(AvatarActor),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		if (FireZone)
		{
			// Make a separate spec for the fire zone ticks (could be reduced damage)
			FireZone->DamageEffectSpecHandle = MakeDamageSpec();
			FireZone->ZoneRadius = ExplosionRadius;
			FireZone->ZoneDuration = FireZoneDuration;
			FireZone->DamageTickInterval = FireZoneTickInterval;
			FireZone->FinishSpawning(SpawnTransform);
		}
	}
}

FGameplayEffectSpecHandle URageInMageMeteorAbility::MakeDamageSpec() const
{
	const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddInstigator(GetOwningActorFromActorInfo(), GetAvatarActorFromActorInfo());

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
	for (const auto& Pair : DamageTypeTags)
	{
		const float ScaledValue = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledValue);
	}
	return SpecHandle;
}
