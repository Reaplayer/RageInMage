// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageZipNZapAbility.h"

#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "Character/RageInMageCharacterBase.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

URageInMageZipNZapAbility::URageInMageZipNZapAbility()
{
	// Charge state (CurrentCharges, recharge timer, in-flight dash state) must survive between
	// activations - InstancedPerActor keeps one persistent ability instance per ASC instead of a
	// fresh one per cast.
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URageInMageZipNZapAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	CurrentCharges = MaxCharges;
}

bool URageInMageZipNZapAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (CurrentCharges <= 0) return false;
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void URageInMageZipNZapAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	EndZip();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URageInMageZipNZapAbility::BeginZipNZap()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	ARageInMageCharacterBase* CharacterBase = Cast<ARageInMageCharacterBase>(AvatarActor);
	if (!CharacterBase) return;

	// Re-entrant cast (second charge fired while the first dash is still in flight): cleanly end the
	// current one first so OriginalBaseWalkSpeed below is always captured from the true resting state.
	if (bIsDashing)
	{
		EndZip();
	}

	ConsumeCharge();

	const URageInMageAttributeSet* RageAS = CharacterBase->GetRageInMageAttributeSet();
	const float MovementSpeedCoefficient = RageAS ? RageAS->GetMovementSpeed() : 1.f;

	OriginalBaseWalkSpeed = CharacterBase->GetBaseWalkSpeed();
	CharacterBase->SetBaseWalkSpeed(CharacterSpeed.GetValueAtLevel(GetAbilityLevel()));

	DashDirection = AvatarActor->GetActorForwardVector();
	DashDirection.Z = 0.f;
	DashDirection = DashDirection.GetSafeNormal();
	if (DashDirection.IsNearlyZero())
	{
		DashDirection = FVector::ForwardVector;
	}

	DashStartLocation = AvatarActor->GetActorLocation();
	// Same MovementSpeed coefficient that just boosted the dash's actual travel speed also extends
	// how far it goes, so a faster character zips both quicker and further.
	ActualDashDistance = DashDistance.GetValueAtLevel(GetAbilityLevel()) * MovementSpeedCoefficient;

	// Zap: one upfront sweep along the full dash path, chain-damaging everyone it crosses.
	if (AvatarActor->HasAuthority())
	{
		const FVector SweepEnd = DashStartLocation + DashDirection * ActualDashDistance;

		TArray<FHitResult> HitResults;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(AvatarActor);
		GetWorld()->SweepMultiByChannel(HitResults, DashStartLocation, SweepEnd, FQuat::Identity,
			ECC_Pawn, FCollisionShape::MakeSphere(DashWidth * 0.5f), QueryParams);

		TSet<AActor*> AlreadyHit;
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor || AlreadyHit.Contains(HitActor)) continue;
			if (URageInMageAbilitySystemLibrary::IsFriendly(AvatarActor, HitActor)) continue;

			AlreadyHit.Add(HitActor);
			CauseChainDamage(HitActor);
		}

#if ENABLE_DRAW_DEBUG
		if (bShowDebug)
		{
			DrawDebugLine(GetWorld(), DashStartLocation, SweepEnd, FColor::Cyan, false, 2.f, 0, DashWidth * 0.5f > 0.f ? 3.f : 1.f);
		}
#endif
	}

	bIsDashing = true;
	DashElapsedSafety = 0.f;
	constexpr float TickInterval = 1.f / 60.f;
	GetWorld()->GetTimerManager().SetTimer(DashTickTimerHandle, this, &URageInMageZipNZapAbility::ZipTick, TickInterval, true);
}

void URageInMageZipNZapAbility::ZipTick()
{
	if (!bIsDashing) return;

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	UCharacterMovementComponent* MoveComp = Character ? Character->GetCharacterMovement() : nullptr;
	if (!MoveComp)
	{
		EndZip();
		return;
	}

	// Drive movement by setting velocity directly (same convention as RageInMageSurfAbility::SurfTick) -
	// MaxWalkSpeed already reflects CharacterSpeed * MovementSpeed coefficient via SetBaseWalkSpeed above.
	MoveComp->Velocity = DashDirection * MoveComp->MaxWalkSpeed;

	constexpr float DeltaTime = 1.f / 60.f;
	DashElapsedSafety += DeltaTime;

	const float DistanceTravelled = FVector::Dist2D(Character->GetActorLocation(), DashStartLocation);
	if (DistanceTravelled >= ActualDashDistance || DashElapsedSafety >= MaxDashSafetyDuration)
	{
		EndZip();
	}
}

void URageInMageZipNZapAbility::EndZip()
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
		if (ACharacter* Character = Cast<ACharacter>(CharacterBase))
		{
			Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;
		}
	}
}

void URageInMageZipNZapAbility::ConsumeCharge()
{
	CurrentCharges = FMath::Max(0, CurrentCharges - 1);

	if (GetWorld() && !GetWorld()->GetTimerManager().IsTimerActive(RechargeTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(
			RechargeTimerHandle, this, &URageInMageZipNZapAbility::OnChargeRecharged, RechargeTime, false);
	}
}

void URageInMageZipNZapAbility::OnChargeRecharged()
{
	CurrentCharges = FMath::Min(MaxCharges, CurrentCharges + 1);

	// One timer re-arms itself to model N independently-recharging charges sequentially - behaviorally
	// identical to N separate timers at a fixed recharge interval, with far less bookkeeping.
	if (CurrentCharges < MaxCharges && GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			RechargeTimerHandle, this, &URageInMageZipNZapAbility::OnChargeRecharged, RechargeTime, false);
	}
}
