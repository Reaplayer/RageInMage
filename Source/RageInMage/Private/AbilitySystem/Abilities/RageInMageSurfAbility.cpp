// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageSurfAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Actor/RageInMageWaveBox.h"
#include "Actor/RageInMageZone.h"
#include "Character/RageInMageCharacterBase.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RageInMagePlayerController.h"
#include "RageInMageGameplayTag.h"


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
	DamagedEnemyTimestamps.Empty();

	// Start surf tick at ~60Hz
	constexpr float TickInterval = 1.f / 60.f;
	GetWorld()->GetTimerManager().SetTimer(
		SurfTickTimerHandle, this, &URageInMageSurfAbility::SurfTick,
		TickInterval, true);

	// Store total duration and starting Z for wave height calculation
	SurfTotalDuration = SurfTimeRemaining;
	SurfStartZ = AvatarActor->GetActorLocation().Z;

	// Spawn wave box if configured
	if (WaveBoxClass)
	{
		if (ACharacter* Character = Cast<ACharacter>(AvatarActor))
		{
			// Disable plane constraint and switch to flying so Z-axis movement works
			Character->GetCharacterMovement()->bConstrainToPlane = false;
			Character->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

			FTransform SpawnTransform;
			SpawnTransform.SetLocation(AvatarActor->GetActorLocation());
			SpawnTransform.SetRotation(SurfDirection.ToOrientationQuat());

			ActiveWaveBox = GetWorld()->SpawnActorDeferred<ARageInMageWaveBox>(
				WaveBoxClass, SpawnTransform,
				GetOwningActorFromActorInfo(),
				Cast<APawn>(AvatarActor),
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

			if (ActiveWaveBox)
			{
				ActiveWaveBox->RidingCharacter = Character;
				ActiveWaveBox->BoxCollision->SetBoxExtent(WaveBoxHalfExtent);

				// Hand the wave box everything it needs to act as a moving wall: anything it
				// overlaps gets dragged along (every tick) and damaged/popped (on its own interval).
				ActiveWaveBox->DamageEffectSpecHandle = MakeSurfDamageSpec();
				ActiveWaveBox->DragSpeed = SurfSpeed.GetValueAtLevel(GetAbilityLevel());
				ActiveWaveBox->PushbackStrength = PushbackStrength.GetValueAtLevel(GetAbilityLevel());
				ActiveWaveBox->DamageTickInterval = SurfDamageTickInterval;
				if (OnHitEffectClass)
				{
					ActiveWaveBox->OnHitEffectSpecHandle = MakeSurfOnHitSpec();
				}

				ActiveWaveBox->FinishSpawning(SpawnTransform);
			}
		}
	}

	// Register for hit interruption during surf
	if (bInterruptOnHit)
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			HitReactionDelegateHandle = ASC->RegisterGameplayTagEvent(
				FRageInMageGameplayTag::Get().Effects_HitReaction,
				EGameplayTagEventType::NewOrRemoved)
				.AddUObject(this, &URageInMageSurfAbility::OnCasterHitDuringSurf);
		}
	}

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

	// Destroy wave box
	if (ActiveWaveBox)
	{
		ActiveWaveBox->Destroy();
		ActiveWaveBox = nullptr;
	}

	// Restore character movement
	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;

		// Restore plane constraint and walking mode if wave box was used
		if (WaveBoxClass)
		{
			Character->GetCharacterMovement()->bConstrainToPlane = true;
			Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
	}

	// Unregister hit reaction delegate
	if (bInterruptOnHit)
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->RegisterGameplayTagEvent(
				FRageInMageGameplayTag::Get().Effects_HitReaction,
				EGameplayTagEventType::NewOrRemoved)
				.Remove(HitReactionDelegateHandle);
		}
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

	// Drive movement by setting velocity directly
	if (ACharacter* Character = Cast<ACharacter>(AvatarActor))
	{
		// Steer toward the player's aim direction (cursor for M&K, right thumbstick for controller)
		if (bCanSteer)
		{
			if (const ARageInMagePlayerController* MagePC = Cast<ARageInMagePlayerController>(Character->GetController()))
			{
				const FVector AimPos = MagePC->GetCurrentAimWorldPosition();
				const FVector ToAim = AimPos - Character->GetActorLocation();
				if (ToAim.SizeSquared2D() > 100.f)
				{
					const FRotator CurrentRot = SurfDirection.ToOrientationRotator();
					const FRotator TargetRot = ToAim.GetSafeNormal2D().ToOrientationRotator();
					const FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, SurfSteerRate);
					SurfDirection = NewRot.Vector();
					SurfDirection.Z = 0.f;
					SurfDirection.Normalize();
				}
			}
		}

		const float Speed = SurfSpeed.GetValueAtLevel(GetAbilityLevel());
		FVector DesiredVelocity = SurfDirection * Speed;

		// If wave box is active, also manage Z position via height interpolation
		if (ActiveWaveBox)
		{
			const float CurrentHeight = CalculateCurrentWaveHeight();

			// Set character Z position: ground start + current wave height
			FVector CharLoc = Character->GetActorLocation();
			CharLoc.Z = SurfStartZ + CurrentHeight;
			Character->SetActorLocation(CharLoc);

			// Keep velocity horizontal only (Z is managed by SetActorLocation)
			DesiredVelocity.Z = 0.f;

			// Update wave box position and rotation to follow character
			const float CapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			ActiveWaveBox->UpdatePosition(CharLoc, CurrentHeight, CapsuleHalfHeight);
			ActiveWaveBox->SetActorRotation(SurfDirection.ToOrientationRotator());
		}

		Character->GetCharacterMovement()->Velocity = DesiredVelocity;
	}

	// Server-only: damage check and trail spawning
	if (AvatarActor->HasAuthority())
	{
		// Wave box (when active) drives its own overlap-based damage/drag every tick.
		// Without one, fall back to the radius check around the caster.
		if (!ActiveWaveBox)
		{
			SurfDamageCheck();
		}

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
	// Fallback path for surf abilities with no WaveBoxClass configured (e.g. a ground-slide
	// variant with no wave VFX/collision actor). When a wave box is active, it drives its own
	// overlap-based damage/drag in ARageInMageWaveBox::Tick instead — see BeginSurf.
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

	const double CurrentTime = GetWorld()->GetTimeSeconds();

	for (AActor* Target : OverlappingActors)
	{
		if (URageInMageAbilitySystemLibrary::IsBothEnemy(AvatarActor, Target)) continue;

		// Per-enemy tick interval check
		if (const double* LastHitTime = DamagedEnemyTimestamps.Find(Target))
		{
			if (SurfDamageTickInterval <= 0.f || (CurrentTime - *LastHitTime) < SurfDamageTickInterval)
			{
				continue;
			}
		}
		DamagedEnemyTimestamps.Add(Target, CurrentTime);

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
			if (ARageInMageCharacterBase* TargetChar = Cast<ARageInMageCharacterBase>(Target))
			{
				FVector PushDir = SurfDirection;
				PushDir.Z = 0.3f;
				PushDir.Normalize();
				TargetChar->ApplyKnockbackImpulse(PushDir * Pushback, true, true);
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

FGameplayEffectSpecHandle URageInMageSurfAbility::MakeSurfOnHitSpec() const
{
	const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC || !OnHitEffectClass) return FGameplayEffectSpecHandle();

	FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
	Ctx.SetAbility(this);
	Ctx.AddInstigator(GetOwningActorFromActorInfo(), GetAvatarActorFromActorInfo());

	return SourceASC->MakeOutgoingSpec(OnHitEffectClass, GetAbilityLevel(), Ctx);
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

float URageInMageSurfAbility::CalculateCurrentWaveHeight() const
{
	if (MaxWaveHeight <= 0.f) return 0.f;

	const float Elapsed = SurfTotalDuration - SurfTimeRemaining;

	// Rise phase: interpolate from 0 to MaxWaveHeight
	if (Elapsed < WaveRiseDuration && WaveRiseDuration > 0.f)
	{
		const float Alpha = Elapsed / WaveRiseDuration;
		return MaxWaveHeight * FMath::InterpEaseOut(0.f, 1.f, Alpha, 2.f);
	}

	// Descend phase: interpolate from MaxWaveHeight back to 0
	if (SurfTimeRemaining < WaveDescendDuration && WaveDescendDuration > 0.f)
	{
		const float Alpha = SurfTimeRemaining / WaveDescendDuration;
		return MaxWaveHeight * FMath::InterpEaseIn(0.f, 1.f, Alpha, 2.f);
	}

	// Cruise phase: hold at max height
	return MaxWaveHeight;
}

void URageInMageSurfAbility::OnCasterHitDuringSurf(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0 && bIsSurfing)
	{
		EndSurf();
	}
}
