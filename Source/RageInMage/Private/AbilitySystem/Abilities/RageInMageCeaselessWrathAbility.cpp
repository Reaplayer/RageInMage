// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageCeaselessWrathAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"

void URageInMageCeaselessWrathAbility::BeginCeaselessWrath(const FVector& TargetCenter)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority()) return;

	WrathCenter = TargetCenter;
	const float Duration = WrathDuration.GetValueAtLevel(GetAbilityLevel());
	const float Interval = FMath::Max(WaveInterval.GetValueAtLevel(GetAbilityLevel()), 0.1f);
	// First wave fires immediately at t=0, so total waves = floor(Duration/Interval) + 1.
	WavesRemaining = FMath::Max(1, FMath::FloorToInt(Duration / Interval) + 1);

#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		const float Radius = ZoneRadius.GetValueAtLevel(GetAbilityLevel());
		DrawDebugSphere(GetWorld(), WrathCenter, Radius, 24, FColor::Blue, false, Duration, 0, 2.f);
	}
#endif

	LaunchWave();

	if (WavesRemaining > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(
			WaveTimerHandle, this, &URageInMageCeaselessWrathAbility::LaunchWave, Interval, true);
	}
}

void URageInMageCeaselessWrathAbility::LaunchWave()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !GetWorld()) return;

	WavesRemaining--;
	const bool bIsFinalWave = WavesRemaining <= 0;
	if (bIsFinalWave)
	{
		GetWorld()->GetTimerManager().ClearTimer(WaveTimerHandle);
	}

	// Gather fresh living enemies in the zone this wave — re-tracks current positions every wave.
	const float Radius = ZoneRadius.GetValueAtLevel(GetAbilityLevel());
	TArray<AActor*> LiveActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(AvatarActor);
	URageInMageAbilitySystemLibrary::GetLivePlayersWithinRadius(
		AvatarActor, Radius, LiveActors, ActorsToIgnore, WrathCenter);

	TArray<AActor*> TargetableEnemies;
	for (AActor* Actor : LiveActors)
	{
		if (!URageInMageAbilitySystemLibrary::IsFriendly(AvatarActor, Actor))
		{
			TargetableEnemies.Add(Actor);
		}
	}

	// Pick up to StrikeCount impact points: tracked enemies first (no repeats this wave), then
	// random points in the zone once enemies run out — all strikes spawn simultaneously, no stagger.
	const int32 NumStrikes = FMath::Max(1, FMath::RoundToInt(StrikeCount.GetValueAtLevel(GetAbilityLevel())));
	TArray<FVector> ImpactLocations;
	ImpactLocations.Reserve(NumStrikes);

	for (int32 i = 0; i < NumStrikes; ++i)
	{
		FVector ImpactLocation;
		if (TargetableEnemies.Num() > 0)
		{
			const int32 RandomIndex = FMath::RandRange(0, TargetableEnemies.Num() - 1);
			ImpactLocation = TargetableEnemies[RandomIndex]->GetActorLocation();
			TargetableEnemies.RemoveAtSwap(RandomIndex);
		}
		else
		{
			const FVector2D RandomPoint = FMath::RandPointInCircle(Radius);
			ImpactLocation = WrathCenter + FVector(RandomPoint.X, RandomPoint.Y, 0.f);
		}

		// Trace down to ground level
		FHitResult GroundHit;
		const FVector TraceStart = ImpactLocation + FVector(0.f, 0.f, 2000.f);
		const FVector TraceEnd = ImpactLocation - FVector(0.f, 0.f, 2000.f);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(AvatarActor);
		if (GetWorld()->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			ImpactLocation = GroundHit.ImpactPoint;
		}

		ImpactLocations.Add(ImpactLocation);

		if (StrikeEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(AvatarActor, StrikeEffect, ImpactLocation, FRotator::ZeroRotator);
		}
		if (StrikeSound)
		{
			UGameplayStatics::PlaySoundAtLocation(AvatarActor, StrikeSound, ImpactLocation);
		}

#if ENABLE_DRAW_DEBUG
		if (bShowDebug)
		{
			DrawDebugSphere(GetWorld(), ImpactLocation, StrikeExplosionRadius.GetValueAtLevel(GetAbilityLevel()),
				16, FColor::Orange, false, StrikeImpactDelay + 0.5f, 0, 1.5f);
		}
#endif
	}

	// All strikes in this wave land together after one shared, fixed VFX-sync delay.
	FTimerHandle ImpactTimer;
	FTimerDelegate ImpactDelegate;
	ImpactDelegate.BindLambda([this, ImpactLocations, bIsFinalWave]()
	{
		AActor* Avatar = GetAvatarActorFromActorInfo();
		if (!Avatar || !GetWorld())
		{
			if (bIsFinalWave)
			{
				K2_EndAbility();
			}
			return;
		}

		const float ExplosionRadius = StrikeExplosionRadius.GetValueAtLevel(GetAbilityLevel());

		for (const FVector& ImpactLocation : ImpactLocations)
		{
			TArray<AActor*> OverlappingActors;
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(Avatar);
			URageInMageAbilitySystemLibrary::GetLivePlayersWithinRadius(
				Avatar, ExplosionRadius, OverlappingActors, ActorsToIgnore, ImpactLocation);

			for (AActor* Target : OverlappingActors)
			{
				if (URageInMageAbilitySystemLibrary::IsBothEnemy(Avatar, Target)) continue;

				CauseDamage(Target);
				ApplyKnockback(Target, ImpactLocation);
			}
		}

		if (bIsFinalWave)
		{
			K2_EndAbility();
		}
	});

	GetWorld()->GetTimerManager().SetTimer(ImpactTimer, ImpactDelegate, StrikeImpactDelay, false);
}
