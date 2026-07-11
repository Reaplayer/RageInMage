// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageCatapultLaunchAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "Character/RageInMageCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

URageInMageCatapultLaunchAbility::URageInMageCatapultLaunchAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URageInMageCatapultLaunchAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	EndDash();
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(ThrowTrackTimerHandle);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URageInMageCatapultLaunchAbility::BeginCatapultLaunch()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	ARageInMageCharacterBase* CharacterBase = Cast<ARageInMageCharacterBase>(AvatarActor);
	if (!CharacterBase)
	{
		K2_EndAbility();
		return;
	}

	const URageInMageAttributeSet* RageAS = CharacterBase->GetRageInMageAttributeSet();
	const float MovementSpeedCoefficient = RageAS ? RageAS->GetMovementSpeed() : 1.f;

	OriginalBaseWalkSpeed = CharacterBase->GetBaseWalkSpeed();
	CharacterBase->SetBaseWalkSpeed(CharacterSpeed.GetValueAtLevel(GetAbilityLevel()));

	if (UCapsuleComponent* Capsule = CharacterBase->GetCapsuleComponent())
	{
		SavedPawnCollisionResponse = Capsule->GetCollisionResponseToChannel(ECC_Pawn);
		Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	}

	DashDirection = AvatarActor->GetActorForwardVector();
	DashDirection.Z = 0.f;
	DashDirection = DashDirection.GetSafeNormal();
	if (DashDirection.IsNearlyZero())
	{
		DashDirection = FVector::ForwardVector;
	}

	DashStartLocation = AvatarActor->GetActorLocation();
	const float MaxReach = DashDistance.GetValueAtLevel(GetAbilityLevel()) * MovementSpeedCoefficient;
	ActualDashDistance = MaxReach;

	// Find the NEAREST enemy along the dash path - that's who we grab. Dash only travels far enough to reach them.
	{
		const FVector SweepEnd = DashStartLocation + DashDirection * MaxReach;
		TArray<FHitResult> HitResults;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(AvatarActor);
		GetWorld()->SweepMultiByChannel(HitResults, DashStartLocation, SweepEnd, FQuat::Identity,
			ECC_Pawn, FCollisionShape::MakeSphere(DashWidth * 0.5f), QueryParams);

		float NearestDistSq = TNumericLimits<float>::Max();
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor || URageInMageAbilitySystemLibrary::IsFriendly(AvatarActor, HitActor)) continue;

			const float DistSq = FVector::DistSquared(DashStartLocation, HitActor->GetActorLocation());
			if (DistSq < NearestDistSq)
			{
				NearestDistSq = DistSq;
				GrabbedEnemy = HitActor;
			}
		}

		if (GrabbedEnemy.IsValid())
		{
			// Stop the dash right at the grabbed enemy.
			ActualDashDistance = FMath::Min(MaxReach, FMath::Sqrt(NearestDistSq));
		}
	}

	bIsDashing = true;
	DashElapsedSafety = 0.f;
	constexpr float TickInterval = 1.f / 60.f;
	GetWorld()->GetTimerManager().SetTimer(DashTickTimerHandle, this, &URageInMageCatapultLaunchAbility::DashTick, TickInterval, true);
}

void URageInMageCatapultLaunchAbility::DashTick()
{
	if (!bIsDashing) return;

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	UCharacterMovementComponent* MoveComp = Character ? Character->GetCharacterMovement() : nullptr;
	if (!MoveComp)
	{
		K2_EndAbility();
		return;
	}

	MoveComp->Velocity = DashDirection * MoveComp->MaxWalkSpeed;

	constexpr float DeltaTime = 1.f / 60.f;
	DashElapsedSafety += DeltaTime;

	const float DistanceTravelled = FVector::Dist2D(Character->GetActorLocation(), DashStartLocation);
	if (DistanceTravelled >= ActualDashDistance || DashElapsedSafety >= MaxDashSafetyDuration)
	{
		EndDash();

		if (GrabbedEnemy.IsValid())
		{
			DoThrow();
		}
		else
		{
			K2_EndAbility();
		}
	}
}

void URageInMageCatapultLaunchAbility::EndDash()
{
	if (!bIsDashing) return;
	bIsDashing = false;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(DashTickTimerHandle);
	}

	if (ARageInMageCharacterBase* CharacterBase = Cast<ARageInMageCharacterBase>(GetAvatarActorFromActorInfo()))
	{
		CharacterBase->SetBaseWalkSpeed(OriginalBaseWalkSpeed);

		if (UCapsuleComponent* Capsule = CharacterBase->GetCapsuleComponent())
		{
			Capsule->SetCollisionResponseToChannel(ECC_Pawn, SavedPawnCollisionResponse);
		}

		if (ACharacter* Character = Cast<ACharacter>(CharacterBase))
		{
			Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;
		}
	}
}

void URageInMageCatapultLaunchAbility::DoThrow()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	AActor* Enemy = GrabbedEnemy.Get();
	if (!AvatarActor || !Enemy)
	{
		K2_EndAbility();
		return;
	}

	if (AvatarActor->HasAuthority())
	{
		// Initial hit on the grabbed enemy.
		CauseDamage(Enemy);

		// Throw the enemy backward relative to ITS own facing (mage vaults over and hurls it behind), with an arc.
		FVector ThrowDir = -Enemy->GetActorForwardVector();
		ThrowDir.Z = 0.f;
		ThrowDir = ThrowDir.GetSafeNormal();
		if (ThrowDir.IsNearlyZero())
		{
			ThrowDir = -DashDirection; // fallback: opposite the dash
		}
		ThrowDir.Z = FMath::Clamp(ThrowUpwardForce, 0.f, 1.f);
		ThrowDir.Normalize();

		if (ARageInMageCharacterBase* EnemyChar = Cast<ARageInMageCharacterBase>(Enemy))
		{
			EnemyChar->ApplyKnockbackImpulse(ThrowDir * ThrowStrength.GetValueAtLevel(GetAbilityLevel()), true, true);
		}
	}

	// Track the flying enemy for mid-air collisions with other enemies.
	ThrowElapsed = 0.f;
	ThrowAlreadyHit.Empty();
	constexpr float TickInterval = 1.f / 60.f;
	GetWorld()->GetTimerManager().SetTimer(ThrowTrackTimerHandle, this, &URageInMageCatapultLaunchAbility::ThrowTrackTick, TickInterval, true);
}

void URageInMageCatapultLaunchAbility::ThrowTrackTick()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	AActor* Enemy = GrabbedEnemy.Get();
	if (!AvatarActor || !Enemy)
	{
		K2_EndAbility();
		return;
	}

	constexpr float DeltaTime = 1.f / 60.f;
	ThrowElapsed += DeltaTime;

	if (AvatarActor->HasAuthority())
	{
		const FVector ThrownLocation = Enemy->GetActorLocation();

		TArray<AActor*> Nearby;
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(AvatarActor);
		ActorsToIgnore.Add(Enemy);
		URageInMageAbilitySystemLibrary::GetLivePlayersWithinRadius(
			AvatarActor, ThrowCollisionRadius, Nearby, ActorsToIgnore, ThrownLocation);

		for (AActor* Other : Nearby)
		{
			if (!Other || Other == Enemy) continue;
			if (URageInMageAbilitySystemLibrary::IsFriendly(AvatarActor, Other)) continue;
			if (ThrowAlreadyHit.Contains(Other)) continue;

			ThrowAlreadyHit.Add(Other);
			CauseDamage(Other);
			ApplyKnockback(Other, ThrownLocation);
		}

#if ENABLE_DRAW_DEBUG
		if (bShowDebug)
		{
			DrawDebugSphere(GetWorld(), ThrownLocation, ThrowCollisionRadius, 12, FColor::Cyan, false, DeltaTime * 1.1f);
		}
#endif
	}

	if (ThrowElapsed >= ThrowTrackDuration)
	{
		K2_EndAbility();
	}
}
