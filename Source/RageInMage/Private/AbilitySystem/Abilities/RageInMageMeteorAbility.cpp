// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageMeteorAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Actor/RageInMageZone.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


void URageInMageMeteorAbility::BeginMeteorRain(const FVector& TargetCenter)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority()) return;

	MeteorRainCenter = TargetCenter;
	MeteorsRemaining = FMath::RoundToInt(MeteorCount.GetValueAtLevel(GetAbilityLevel()));
	MeteorsPendingImpact = 0;

	// Gather live enemies within the rain area for targeted meteor strikes
	const float AreaRadius = RainAreaRadius.GetValueAtLevel(GetAbilityLevel());
	TArray<AActor*> LiveActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(AvatarActor);
	URageInMageAbilitySystemLibrary::GetLivePlayersWithinRadius(
		AvatarActor, AreaRadius, LiveActors, ActorsToIgnore, MeteorRainCenter);

	// Filter to only enemies of the caster
	TargetableEnemies.Reset();
	for (AActor* Actor : LiveActors)
	{
		if (!URageInMageAbilitySystemLibrary::IsFriendly(AvatarActor, Actor))
		{
			TargetableEnemies.Add(Actor);
		}
	}

#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		// Blue sphere: Rain area radius centered on aim position
		const float RainDuration = FMath::Max((MeteorsRemaining - 1) * MeteorStaggerDelay, 0.1f);
		DrawDebugSphere(GetWorld(), MeteorRainCenter, AreaRadius, 24, FColor::Blue, false, RainDuration, 0, 2.f);
	}
#endif

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

	FVector ImpactLocation;

	// Try to target an enemy first (each enemy targeted at most once)
	AActor* Target = PickAndConsumeRandomTarget();
	if (Target)
	{
		ImpactLocation = Target->GetActorLocation();
	}
	else
	{
		// No valid enemy targets remain — fall back to random point in the rain area
		const float AreaRadius = RainAreaRadius.GetValueAtLevel(GetAbilityLevel());
		const FVector2D RandomPoint = FMath::RandPointInCircle(AreaRadius);
		ImpactLocation = MeteorRainCenter + FVector(RandomPoint.X, RandomPoint.Y, 0.f);
	}

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
	MeteorsPendingImpact++;
}

void URageInMageMeteorAbility::SpawnMeteorAtLocation(const FVector& ImpactLocation)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();

#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		const float ExpRadius = MeteorExplosionRadius.GetValueAtLevel(GetAbilityLevel());

		// Orange sphere: Meteor explosion radius at impact point (1 second)
		DrawDebugSphere(GetWorld(), ImpactLocation, ExpRadius, 16, FColor::Orange, false, 1.f, 0, 1.5f);

		// Green sphere: Fire zone radius at impact point (lasts FireZoneDuration)
		// Drawn slightly elevated (+5) so it doesn't Z-fight with the orange sphere
		DrawDebugSphere(GetWorld(), ImpactLocation + FVector(0.f, 0.f, 5.f), ExpRadius, 16, FColor::Green, false, FireZoneDuration, 0, 1.5f);

		// Yellow line: meteor trajectory from spawn point (above caster) to impact
		const FVector DebugSpawnPos = AvatarActor->GetActorLocation() + FVector(0.f, 0.f, MeteorSpawnHeight);
		DrawDebugLine(GetWorld(), DebugSpawnPos, ImpactLocation, FColor::Yellow, false, 1.f, 0, 1.f);

		// Red point: exact impact location
		DrawDebugPoint(GetWorld(), ImpactLocation, 12.f, FColor::Red, false, 2.f);
	}
#endif

	// Calculate meteor trajectory: spawn above caster, fly toward impact point
	const FVector CasterLocation = AvatarActor->GetActorLocation();
	const FVector SpawnOffset = FVector(
		FMath::RandRange(-100.f, 100.f),
		FMath::RandRange(-100.f, 100.f),
		MeteorSpawnHeight);
	const FVector SpawnPos = CasterLocation + SpawnOffset;
	const FVector ToImpact = ImpactLocation - SpawnPos;
	const float Distance = ToImpact.Size();
	const float TravelTime = MeteorSpeed > 0.f ? Distance / MeteorSpeed : 0.05f;
	const FVector MeteorVelocity = MeteorSpeed > 0.f ? ToImpact.GetSafeNormal() * MeteorSpeed : FVector::ZeroVector;

	// VFX — spawn deactivated, set user params, then activate so the first frame uses correct values
	if (MeteorImpactEffect)
	{
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			AvatarActor, MeteorImpactEffect, ImpactLocation,
			FRotator::ZeroRotator, FVector(1.f), /*bAutoDestroy=*/true,
			/*bAutoActivate=*/false);
		if (NiagaraComp)
		{
			NiagaraComp->SetVariablePosition(FName("SpawnOffset"), SpawnPos);
			NiagaraComp->SetVariableVec3(FName("MeteorVelocity"), MeteorVelocity);
			NiagaraComp->SetVariableFloat(FName("TravelTime"), TravelTime);
			NiagaraComp->Activate();
		}
	}

	// Delay damage + fire zone until the meteor visually arrives
	FTimerHandle MeteorDamageTimer;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this, ImpactLocation]()
	{
		AActor* Avatar = GetAvatarActorFromActorInfo();
		if (!Avatar || !GetWorld())
		{
			OnMeteorImpactComplete();
			return;
		}

		if (MeteorImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(Avatar, MeteorImpactSound, ImpactLocation);
		}

		// Apply AoE damage at impact
		const float ExplosionRadius = MeteorExplosionRadius.GetValueAtLevel(GetAbilityLevel());
		FGameplayEffectSpecHandle SpecHandle = MakeDamageSpec();

		TArray<AActor*> OverlappingActors;
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(Avatar);
		URageInMageAbilitySystemLibrary::GetLivePlayersWithinRadius(
			Avatar, ExplosionRadius, OverlappingActors, ActorsToIgnore, ImpactLocation);

		for (AActor* Target : OverlappingActors)
		{
			if (URageInMageAbilitySystemLibrary::IsBothEnemy(Avatar, Target)) continue;

			if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
			ApplyKnockback(Target, ImpactLocation);
		}

		// Spawn zone at impact location
		if (ZoneClass)
		{
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(ImpactLocation);

			ARageInMageZone* Zone = GetWorld()->SpawnActorDeferred<ARageInMageZone>(
				ZoneClass, SpawnTransform,
				GetOwningActorFromActorInfo(),
				Cast<APawn>(Avatar),
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

			if (Zone)
			{
				Zone->DamageEffectSpecHandle = MakeFireZoneDamageSpec();
				Zone->ZoneRadius = ExplosionRadius;
				Zone->ZoneDuration = FireZoneDuration;
				Zone->DamageTickInterval = FireZoneTickInterval;
				Zone->FinishSpawning(SpawnTransform);
			}
		}

		OnMeteorImpactComplete();
	});

	GetWorld()->GetTimerManager().SetTimer(MeteorDamageTimer, TimerDelegate, FMath::Max(TravelTime, 0.05f), false);
}

void URageInMageMeteorAbility::OnMeteorImpactComplete()
{
	MeteorsPendingImpact--;
	if (MeteorsRemaining <= 0 && MeteorsPendingImpact <= 0)
	{
		K2_EndAbility();
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

AActor* URageInMageMeteorAbility::PickAndConsumeRandomTarget()
{
	const float AreaRadius = RainAreaRadius.GetValueAtLevel(GetAbilityLevel());
	const float AreaRadiusSq = AreaRadius * AreaRadius;

	while (TargetableEnemies.Num() > 0)
	{
		const int32 RandomIndex = FMath::RandRange(0, TargetableEnemies.Num() - 1);
		AActor* Candidate = TargetableEnemies[RandomIndex];

		// Remove from array regardless (each enemy only considered once)
		TargetableEnemies.RemoveAtSwap(RandomIndex);

		// Skip dead/destroyed actors
		if (!IsValid(Candidate)) continue;

		// Check if the enemy is still within the rain area (2D distance, ignore Z)
		const FVector ToCandidate = Candidate->GetActorLocation() - MeteorRainCenter;
		const float DistSq2D = ToCandidate.X * ToCandidate.X + ToCandidate.Y * ToCandidate.Y;
		if (DistSq2D <= AreaRadiusSq)
		{
			return Candidate;
		}
		// Out of range — discard and try next
	}

	return nullptr;
}

FGameplayEffectSpecHandle URageInMageMeteorAbility::MakeFireZoneDamageSpec() const
{
	const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddInstigator(GetOwningActorFromActorInfo(), GetAvatarActorFromActorInfo());

	// Use FireZoneDamageEffectClass if set, else fallback to DamageEffectClass
	TSubclassOf<UGameplayEffect> GEClass = FireZoneDamageEffectClass ? FireZoneDamageEffectClass : DamageEffectClass;
	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(GEClass, GetAbilityLevel(), EffectContextHandle);

	// Use FireZoneDamageTypeTags if set, else fallback to DamageTypeTags
	const TMap<FGameplayTag, FScalableFloat>& Tags = FireZoneDamageTypeTags.Num() > 0
		? FireZoneDamageTypeTags : DamageTypeTags;

	for (const auto& Pair : Tags)
	{
		const float ScaledValue = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledValue);
	}

	return SpecHandle;
}
