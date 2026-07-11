// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageCleanWindDashAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "Character/RageInMageCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

URageInMageCleanWindDashAbility::URageInMageCleanWindDashAbility()
{
	// Charge state (CurrentCharges, recharge timer, in-flight dash state) must survive between
	// activations - InstancedPerActor keeps one persistent ability instance per ASC instead of a
	// fresh one per cast.
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URageInMageCleanWindDashAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	CurrentCharges = MaxCharges;
}

bool URageInMageCleanWindDashAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (CurrentCharges <= 0) return false;
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void URageInMageCleanWindDashAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	EndDash();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URageInMageCleanWindDashAbility::BeginCleanWindDash()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	ARageInMageCharacterBase* CharacterBase = Cast<ARageInMageCharacterBase>(AvatarActor);
	if (!CharacterBase)
	{
		// Nothing to dash - don't leave the ability hanging active.
		K2_EndAbility();
		return;
	}

	// Re-entrant cast (second charge fired while the first dash is still in flight): cleanly end the
	// current one first so OriginalBaseWalkSpeed below is always captured from the true resting state.
	if (bIsDashing)
	{
		EndDash();
	}

	ConsumeCharge();

	const URageInMageAttributeSet* RageAS = CharacterBase->GetRageInMageAttributeSet();
	const float MovementSpeedCoefficient = RageAS ? RageAS->GetMovementSpeed() : 1.f;

	OriginalBaseWalkSpeed = CharacterBase->GetBaseWalkSpeed();
	CharacterBase->SetBaseWalkSpeed(CharacterSpeed.GetValueAtLevel(GetAbilityLevel()));

	// Phase through other pawns for the duration of the dash (anime-style pass-through). Only the Pawn
	// channel is dropped, so world geometry still blocks and the safety timer still bails on a wall.
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
	// Same MovementSpeed coefficient that just boosted the dash's actual travel speed also extends
	// how far it goes, so a faster character dashes both quicker and further.
	ActualDashDistance = DashDistance.GetValueAtLevel(GetAbilityLevel()) * MovementSpeedCoefficient;

	// Strike + stun: one upfront sweep along the full dash path, hitting everyone it crosses.
	if (AvatarActor->HasAuthority())
	{
		const FVector SweepEnd = DashStartLocation + DashDirection * ActualDashDistance;

		TArray<FHitResult> HitResults;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(AvatarActor);
		GetWorld()->SweepMultiByChannel(HitResults, DashStartLocation, SweepEnd, FQuat::Identity,
			ECC_Pawn, FCollisionShape::MakeSphere(DashWidth * 0.5f), QueryParams);

		UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();

		TSet<AActor*> AlreadyHit;
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor || AlreadyHit.Contains(HitActor)) continue;
			if (URageInMageAbilitySystemLibrary::IsFriendly(AvatarActor, HitActor)) continue;

			AlreadyHit.Add(HitActor);
			CauseDamage(HitActor);

			if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor))
			{
				URageInMageAbilitySystemLibrary::ApplyGuaranteedStun(
					SourceASC, TargetASC, AvatarActor, StunDuration, StunGraceSeconds);
			}
		}

#if ENABLE_DRAW_DEBUG
		if (bShowDebug)
		{
			DrawDebugLine(GetWorld(), DashStartLocation, SweepEnd, FColor::Cyan, false, 2.f, 0, 3.f);
		}
#endif
	}

	bIsDashing = true;
	DashElapsedSafety = 0.f;
	constexpr float TickInterval = 1.f / 60.f;
	GetWorld()->GetTimerManager().SetTimer(DashTickTimerHandle, this, &URageInMageCleanWindDashAbility::DashTick, TickInterval, true);
}

void URageInMageCleanWindDashAbility::DashTick()
{
	if (!bIsDashing) return;

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	UCharacterMovementComponent* MoveComp = Character ? Character->GetCharacterMovement() : nullptr;
	if (!MoveComp)
	{
		K2_EndAbility();
		return;
	}

	// Drive movement by setting velocity directly (same convention as RageInMageZipNZapAbility::ZipTick) -
	// MaxWalkSpeed already reflects CharacterSpeed * MovementSpeed coefficient via SetBaseWalkSpeed above.
	MoveComp->Velocity = DashDirection * MoveComp->MaxWalkSpeed;

	constexpr float DeltaTime = 1.f / 60.f;
	DashElapsedSafety += DeltaTime;

	const float DistanceTravelled = FVector::Dist2D(Character->GetActorLocation(), DashStartLocation);
	if (DistanceTravelled >= ActualDashDistance || DashElapsedSafety >= MaxDashSafetyDuration)
	{
		// Dash finished under its own power - end the ability (EndAbility -> EndDash does the cleanup).
		K2_EndAbility();
	}
}

void URageInMageCleanWindDashAbility::EndDash()
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

		// Restore pawn-vs-pawn collision now the dash is over so the character blocks normally again.
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

void URageInMageCleanWindDashAbility::ConsumeCharge()
{
	CurrentCharges = FMath::Max(0, CurrentCharges - 1);

	if (GetWorld() && !GetWorld()->GetTimerManager().IsTimerActive(RechargeTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(
			RechargeTimerHandle, this, &URageInMageCleanWindDashAbility::OnChargeRecharged, RechargeTime, false);
	}
}

void URageInMageCleanWindDashAbility::OnChargeRecharged()
{
	CurrentCharges = FMath::Min(MaxCharges, CurrentCharges + 1);

	// One timer re-arms itself to model N independently-recharging charges sequentially - behaviorally
	// identical to N separate timers at a fixed recharge interval, with far less bookkeeping.
	if (CurrentCharges < MaxCharges && GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			RechargeTimerHandle, this, &URageInMageCleanWindDashAbility::OnChargeRecharged, RechargeTime, false);
	}
}
