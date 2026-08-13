// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageChargingBullAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "Character/RageInMageCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

URageInMageChargingBullAbility::URageInMageChargingBullAbility()
{
	// In-flight charge state (dash direction, hit set, timers) must persist across the ability's lifetime.
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URageInMageChargingBullAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	EndDash();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URageInMageChargingBullAbility::BeginChargingBull()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	ARageInMageCharacterBase* CharacterBase = Cast<ARageInMageCharacterBase>(AvatarActor);
	if (!CharacterBase)
	{
		// Nothing to charge - don't leave the ability hanging active.
		K2_EndAbility();
		return;
	}

	// Re-entrant cast while a charge is still in flight: cleanly end the current one first so
	// OriginalBaseWalkSpeed below is always captured from the true resting state.
	if (bIsDashing)
	{
		EndDash();
	}

	const URageInMageAttributeSet* RageAS = CharacterBase->GetRageInMageAttributeSet();
	const float MovementSpeedCoefficient = RageAS ? RageAS->GetMovementSpeed() : 1.f;

	OriginalBaseWalkSpeed = CharacterBase->GetBaseWalkSpeed();
	CharacterBase->SetBaseWalkSpeed(CharacterSpeed.GetValueAtLevel(GetAbilityLevel()));

	// Phase through other pawns for the charge (plow through instead of bouncing off the first enemy).
	// Only the Pawn channel is dropped, so world geometry still blocks and the safety timer bails on a wall.
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
	// The MovementSpeed coefficient that boosts the charge's travel speed also extends how far it goes.
	ActualDashDistance = DashDistance.GetValueAtLevel(GetAbilityLevel()) * MovementSpeedCoefficient;

	// Damage + knockback both scale with the caster's Immovable Mass stage, evaluated once at charge start.
	StanceScalar = URageInMageAbilitySystemLibrary::GetImmovableMassStageScalar(
		AvatarActor, StanceBonusStage1, StanceBonusStage2, StanceBonusStage3);

	HitActors.Reset();

	bIsDashing = true;
	DashElapsedSafety = 0.f;
	constexpr float TickInterval = 1.f / 60.f;
	GetWorld()->GetTimerManager().SetTimer(DashTickTimerHandle, this, &URageInMageChargingBullAbility::DashTick, TickInterval, true);
}

void URageInMageChargingBullAbility::DashTick()
{
	if (!bIsDashing) return;

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	UCharacterMovementComponent* MoveComp = Character ? Character->GetCharacterMovement() : nullptr;
	if (!MoveComp)
	{
		K2_EndAbility();
		return;
	}

	// Drive movement by setting velocity directly (same convention as the other dash abilities) -
	// MaxWalkSpeed already reflects CharacterSpeed * MovementSpeed via SetBaseWalkSpeed above.
	MoveComp->Velocity = DashDirection * MoveComp->MaxWalkSpeed;

	// Plow-through: hit fresh enemies at the bull's current position as it reaches them (authority only).
	if (Character->HasAuthority())
	{
		const FVector Center = Character->GetActorLocation();

		TArray<FHitResult> HitResults;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Character);
		GetWorld()->SweepMultiByChannel(HitResults, Center, Center + DashDirection, FQuat::Identity,
			ECC_Pawn, FCollisionShape::MakeSphere(DashWidth * 0.5f), QueryParams);

		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor || HitActors.Contains(HitActor)) continue;
			if (URageInMageAbilitySystemLibrary::IsFriendly(Character, HitActor)) continue;

			HitActors.Add(HitActor);
			CauseScaledDamage(HitActor, StanceScalar);
			ApplyForwardKnockback(HitActor);
		}

#if ENABLE_DRAW_DEBUG
		if (bShowDebug)
		{
			DrawDebugSphere(GetWorld(), Center, DashWidth * 0.5f, 12, FColor::Orange, false, 0.5f);
		}
#endif
	}

	constexpr float DeltaTime = 1.f / 60.f;
	DashElapsedSafety += DeltaTime;

	const float DistanceTravelled = FVector::Dist2D(Character->GetActorLocation(), DashStartLocation);
	if (DistanceTravelled >= ActualDashDistance || DashElapsedSafety >= MaxDashSafetyDuration)
	{
		// Charge finished under its own power - end the ability (EndAbility -> EndDash does the cleanup).
		K2_EndAbility();
	}
}

void URageInMageChargingBullAbility::EndDash()
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

		// Restore pawn-vs-pawn collision now the charge is over so the mage blocks normally again.
		if (UCapsuleComponent* Capsule = CharacterBase->GetCapsuleComponent())
		{
			Capsule->SetCollisionResponseToChannel(ECC_Pawn, SavedPawnCollisionResponse);
		}

		CharacterBase->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	}
}

void URageInMageChargingBullAbility::CauseScaledDamage(AActor* TargetActor, float Multiplier)
{
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
	if (!SpecHandle.IsValid()) return;

	for (const TTuple<FGameplayTag, FScalableFloat>& Pair : DamageTypeTags)
	{
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
			SpecHandle, Pair.Key, Pair.Value.GetValueAtLevel(GetAbilityLevel()) * Multiplier);
	}

	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
	{
		GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
	}
}

void URageInMageChargingBullAbility::ApplyForwardKnockback(AActor* TargetActor) const
{
	const float Strength = KnockbackStrength.GetValueAtLevel(GetAbilityLevel()) * StanceScalar;
	if (Strength <= 0.f || !TargetActor) return;

	ARageInMageCharacterBase* TargetCharacter = Cast<ARageInMageCharacterBase>(TargetActor);
	if (!TargetCharacter) return;

	// Rammed forward along the charge direction (not radially away from the caster).
	FVector Push = DashDirection;
	Push.Z = FMath::Clamp(KnockbackUpwardForce, 0.f, 1.f);
	Push.Normalize();
	TargetCharacter->ApplyKnockbackImpulse(Push * Strength, true, true);
}
