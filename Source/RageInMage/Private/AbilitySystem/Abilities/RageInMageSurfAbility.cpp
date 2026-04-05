// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageSurfAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Actor/RageInMageZone.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


void URageInMageSurfAbility::BeginSurf(const FVector& Direction)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority()) return;

	SurfDirection = Direction;
	SurfDirection.Z = 0.f;
	SurfDirection.Normalize();
	if (SurfDirection.IsNearlyZero())
	{
		SurfDirection = AvatarActor->GetActorForwardVector();
	}

	bIsSurfing = true;
	SurfTimeRemaining = SurfDuration.GetValueAtLevel(GetAbilityLevel());
	LastTrailSpawnLocation = AvatarActor->GetActorLocation();
	DamagedEnemies.Empty();

	// Start surf tick at ~60Hz
	constexpr float TickInterval = 1.f / 60.f;
	GetWorld()->GetTimerManager().SetTimer(
		SurfTickTimerHandle, this, &URageInMageSurfAbility::SurfTick,
		TickInterval, true);

	// Looping sound
	if (SurfLoopSound)
	{
		SurfSoundComponent = UGameplayStatics::SpawnSoundAttached(
			SurfLoopSound, AvatarActor->GetRootComponent());
	}

	OnSurfBegin(AvatarActor->GetActorLocation(), SurfDirection);
}

void URageInMageSurfAbility::EndSurf()
{
	if (!bIsSurfing) return;
	bIsSurfing = false;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(SurfTickTimerHandle);
	}

	// Stop looping sound
	if (SurfSoundComponent)
	{
		SurfSoundComponent->Stop();
		SurfSoundComponent = nullptr;
	}

	// Restore character movement
	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	}

	OnSurfEnd();
}

void URageInMageSurfAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	EndSurf();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URageInMageSurfAbility::SurfTick()
{
	if (!bIsSurfing) return;

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		EndSurf();
		return;
	}

	constexpr float DeltaTime = 1.f / 60.f;

	// Drive movement by setting velocity directly (keeps Walking mode for ground snapping)
	if (ACharacter* Character = Cast<ACharacter>(AvatarActor))
	{
		const float Speed = SurfSpeed.GetValueAtLevel(GetAbilityLevel());
		Character->GetCharacterMovement()->Velocity = SurfDirection * Speed;
	}

	// Server-only: damage check and trail spawning
	if (AvatarActor->HasAuthority())
	{
		SurfDamageCheck();

		// Trail spawning based on distance
		if (TrailZoneClass)
		{
			const float DistTravelled = FVector::Dist(AvatarActor->GetActorLocation(), LastTrailSpawnLocation);
			if (DistTravelled >= TrailSpawnInterval)
			{
				SpawnTrailSegment(AvatarActor->GetActorLocation());
				LastTrailSpawnLocation = AvatarActor->GetActorLocation();
			}
		}
	}

	// Broadcast to BP for VFX update
	OnSurfTick(DeltaTime, AvatarActor->GetActorLocation(), SurfDirection);

	SurfTimeRemaining -= DeltaTime;
	if (SurfTimeRemaining <= 0.f)
	{
		EndSurf();
	}
}

void URageInMageSurfAbility::SurfDamageCheck()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return;

	const FVector Origin = AvatarActor->GetActorLocation();
	const float Pushback = PushbackStrength.GetValueAtLevel(GetAbilityLevel());

	TArray<AActor*> OverlappingActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(AvatarActor);
	URageInMageAbilitySystemLibrary::GetLivePlayersWithinRadius(
		AvatarActor, SurfCollisionRadius, OverlappingActors, ActorsToIgnore, Origin);

	const FGameplayEffectSpecHandle DamageSpec = MakeSurfDamageSpec();

	for (AActor* Target : OverlappingActors)
	{
		if (URageInMageAbilitySystemLibrary::IsBothEnemy(AvatarActor, Target)) continue;

		// Skip already-damaged enemies (prevent multi-hit)
		if (DamagedEnemies.Contains(Target)) continue;
		DamagedEnemies.Add(Target);

		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
		if (!TargetASC) continue;

		// Apply damage
		if (DamageSpec.IsValid())
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageSpec.Data.Get());
		}

		// Apply on-hit effect (e.g. slow + wet)
		if (OnHitEffectClass)
		{
			const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
			FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
			Ctx.SetAbility(this);
			const FGameplayEffectSpecHandle OnHitSpec = SourceASC->MakeOutgoingSpec(
				OnHitEffectClass, GetAbilityLevel(), Ctx);
			TargetASC->ApplyGameplayEffectSpecToSelf(*OnHitSpec.Data.Get());
		}

		// Pushback in surf direction
		if (Pushback > 0.f)
		{
			if (ACharacter* TargetChar = Cast<ACharacter>(Target))
			{
				FVector PushDir = SurfDirection;
				PushDir.Z = 0.3f;
				PushDir.Normalize();
				TargetChar->LaunchCharacter(PushDir * Pushback, true, true);
			}
		}
	}
}

void URageInMageSurfAbility::SpawnTrailSegment(const FVector& Location)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !TrailZoneClass) return;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(Location);

	ARageInMageZone* Trail = GetWorld()->SpawnActorDeferred<ARageInMageZone>(
		TrailZoneClass, SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(AvatarActor),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (Trail)
	{
		Trail->DamageEffectSpecHandle = MakeTrailDamageSpec();
		Trail->ZoneRadius = TrailZoneRadius;
		Trail->ZoneDuration = TrailZoneDuration;
		Trail->DamageTickInterval = TrailZoneTickInterval;
		if (TrailZoneEffect) Trail->ZoneEffect = TrailZoneEffect;
		if (TrailZoneLoopSound) Trail->ZoneLoopSound = TrailZoneLoopSound;
		Trail->FinishSpawning(SpawnTransform);
	}
}

FGameplayEffectSpecHandle URageInMageSurfAbility::MakeSurfDamageSpec() const
{
	const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
	Ctx.SetAbility(this);
	Ctx.AddInstigator(GetOwningActorFromActorInfo(), GetAvatarActorFromActorInfo());

	const FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(
		DamageEffectClass, GetAbilityLevel(), Ctx);
	for (const auto& Pair : DamageTypeTags)
	{
		const float ScaledValue = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(Spec, Pair.Key, ScaledValue);
	}
	return Spec;
}

FGameplayEffectSpecHandle URageInMageSurfAbility::MakeTrailDamageSpec() const
{
	const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
	Ctx.SetAbility(this);
	Ctx.AddInstigator(GetOwningActorFromActorInfo(), GetAvatarActorFromActorInfo());

	TSubclassOf<UGameplayEffect> GEClass = TrailDamageEffectClass ? TrailDamageEffectClass : DamageEffectClass;
	const FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(
		GEClass, GetAbilityLevel(), Ctx);

	const TMap<FGameplayTag, FScalableFloat>& Tags = TrailDamageTypeTags.Num() > 0
		? TrailDamageTypeTags : DamageTypeTags;

	for (const auto& Pair : Tags)
	{
		const float ScaledValue = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(Spec, Pair.Key, ScaledValue);
	}
	return Spec;
}
