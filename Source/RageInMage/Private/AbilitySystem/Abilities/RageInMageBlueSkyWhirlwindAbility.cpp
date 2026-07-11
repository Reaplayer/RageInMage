// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageBlueSkyWhirlwindAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "Character/RageInMageCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

URageInMageBlueSkyWhirlwindAbility::URageInMageBlueSkyWhirlwindAbility()
{
	// In-flight dash state (timer + members) lives for the whole activation; one instance per actor keeps
	// the timer bound to a stable 'this' and matches the proven CleanWindDash dash template.
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URageInMageBlueSkyWhirlwindAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	EndDash();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URageInMageBlueSkyWhirlwindAbility::BeginBlueSkyWhirlwind()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	ARageInMageCharacterBase* CharacterBase = Cast<ARageInMageCharacterBase>(AvatarActor);
	if (!CharacterBase)
	{
		K2_EndAbility();
		return;
	}

	// First kick: radial blast around the caster before we move.
	DoKick(AvatarActor->GetActorLocation());

	// --- Set up the forward dash (mirrors CleanWindDash) ---
	const URageInMageAttributeSet* RageAS = CharacterBase->GetRageInMageAttributeSet();
	const float MovementSpeedCoefficient = RageAS ? RageAS->GetMovementSpeed() : 1.f;

	OriginalBaseWalkSpeed = CharacterBase->GetBaseWalkSpeed();
	CharacterBase->SetBaseWalkSpeed(CharacterSpeed.GetValueAtLevel(GetAbilityLevel()));

	// Phase through other pawns for the duration of the dash (walls still block; safety timer bails).
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
	ActualDashDistance = DashDistance.GetValueAtLevel(GetAbilityLevel()) * MovementSpeedCoefficient;

	bIsDashing = true;
	DashElapsedSafety = 0.f;
	constexpr float TickInterval = 1.f / 60.f;
	GetWorld()->GetTimerManager().SetTimer(DashTickTimerHandle, this, &URageInMageBlueSkyWhirlwindAbility::DashTick, TickInterval, true);
}

void URageInMageBlueSkyWhirlwindAbility::DoKick(const FVector& Origin)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority()) return;

	const float Radius = KickRadius.GetValueAtLevel(GetAbilityLevel());

	TArray<AActor*> OverlappingActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(AvatarActor);
	URageInMageAbilitySystemLibrary::GetLivePlayersWithinRadius(
		AvatarActor, Radius, OverlappingActors, ActorsToIgnore, Origin);

	for (AActor* Target : OverlappingActors)
	{
		if (!Target || URageInMageAbilitySystemLibrary::IsFriendly(AvatarActor, Target)) continue;

		CauseDamage(Target);
		ApplyKnockback(Target, Origin);
	}

#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		DrawDebugSphere(GetWorld(), Origin, Radius, 16, FColor::Cyan, false, 2.f, 0, 2.f);
	}
#endif
}

void URageInMageBlueSkyWhirlwindAbility::DashTick()
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
		// Landed: second kick at the arrival spot, then end (EndAbility -> EndDash cleans up the dash).
		DoKick(Character->GetActorLocation());
		K2_EndAbility();
	}
}

void URageInMageBlueSkyWhirlwindAbility::EndDash()
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
