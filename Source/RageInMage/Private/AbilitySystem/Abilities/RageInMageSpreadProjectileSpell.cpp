// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageSpreadProjectileSpell.h"

#include "Interaction/CombatInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


void URageInMageSpreadProjectileSpell::SpawnSpreadProjectiles(
	const FVector& TargetLocation, const FGameplayTag& SocketTag,
	TSubclassOf<AActor> ProjectileClass, bool bCalculatePitch)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority()) return;

	const int32 Count = FMath::RoundToInt(ProjectileCount.GetValueAtLevel(GetAbilityLevel()));
	if (Count <= 0 || !ProjectileClass) return;

	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
		AvatarActor, SocketTag);

	// Base direction toward target (horizontal only)
	FVector BaseDirection = TargetLocation - SocketLocation;
	const float TargetDistance = FVector(BaseDirection.X, BaseDirection.Y, 0.f).Size();
	BaseDirection.Z = 0.f;
	BaseDirection = BaseDirection.GetSafeNormal();

	if (BaseDirection.IsNearlyZero())
	{
		BaseDirection = AvatarActor->GetActorForwardVector();
	}

	for (int32 i = 0; i < Count; ++i)
	{
		float AngleOffset = 0.f;
		if (Count > 1)
		{
			// Evenly distribute across the cone: from -ConeHalfAngle to +ConeHalfAngle
			AngleOffset = FMath::Lerp(-ConeHalfAngle, ConeHalfAngle,
				static_cast<float>(i) / static_cast<float>(Count - 1));
		}

		// Rotate direction around Z axis by the angle offset
		const FVector SpreadDirection = BaseDirection.RotateAngleAxis(AngleOffset, FVector::UpVector);
		FVector SpreadTarget = SocketLocation + SpreadDirection * TargetDistance;
		SpreadTarget.Z = TargetLocation.Z;

		// Use parent's SpawnProjectile — each gets its own damage spec, knockback, etc.
		SpawnProjectile(SpreadTarget, SocketTag, ProjectileClass, bCalculatePitch);
	}
}

// ── Spread Aim Indicators ──────────────────────────────────────────────────────

void URageInMageSpreadProjectileSpell::SpawnSpreadIndicators(
	UNiagaraSystem* AimLineSystem, FLinearColor AimColour)
{
	DestroySpreadIndicators();

	if (!AimLineSystem) return;

	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return;

	const int32 Count = FMath::RoundToInt(ProjectileCount.GetValueAtLevel(GetAbilityLevel()));
	if (Count <= 0) return;

	const FVector SpawnLocation = AvatarActor->GetActorLocation();
	const FRotator SpawnRotation = AvatarActor->GetActorRotation();

	SpreadIndicatorComponents.Reserve(Count);

	for (int32 i = 0; i < Count; ++i)
	{
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			AvatarActor->GetWorld(),
			AimLineSystem,
			SpawnLocation,
			SpawnRotation,
			FVector(1.f),
			false,  // bAutoDestroy — we manage lifecycle
			true,   // bAutoActivate
			ENCPoolMethod::None,
			true    // bPreCullCheck
		);

		if (NiagaraComp)
		{
			NiagaraComp->SetVariableFloat(FName("AimWidth"), AimIndicatorWidth);
			NiagaraComp->SetVariableFloat(FName("AimLength"), AimIndicatorLength);
			NiagaraComp->SetVariableLinearColor(FName("AimColour"), AimColour);
			SpreadIndicatorComponents.Add(NiagaraComp);
		}
	}
}

void URageInMageSpreadProjectileSpell::UpdateSpreadIndicators(const FVector& AimPosition)
{
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return;

	const int32 Count = SpreadIndicatorComponents.Num();
	if (Count == 0) return;

	const FVector ActorLocation = AvatarActor->GetActorLocation();

	// Horizontal direction from caster to aim target — matches SpawnSpreadProjectiles logic
	FVector BaseDirection = AimPosition - ActorLocation;
	BaseDirection.Z = 0.f;
	BaseDirection = BaseDirection.GetSafeNormal();

	if (BaseDirection.IsNearlyZero())
	{
		BaseDirection = AvatarActor->GetActorForwardVector();
	}

	const float BaseYaw = BaseDirection.Rotation().Yaw;

	// Position: actor location + forward offset (matching BP's existing pattern)
	const FVector ForwardOffset = AvatarActor->GetActorForwardVector() * IndicatorForwardOffset;
	const FVector BasePosition = ActorLocation + ForwardOffset;

	// Projectile count for angle calc (may differ from indicator count if level changed, but shouldn't)
	const int32 ProjectileNum = FMath::RoundToInt(ProjectileCount.GetValueAtLevel(GetAbilityLevel()));

	for (int32 i = 0; i < Count; ++i)
	{
		UNiagaraComponent* Comp = SpreadIndicatorComponents[i];
		if (!IsValid(Comp)) continue;

		float AngleOffset = 0.f;
		if (ProjectileNum > 1)
		{
			AngleOffset = FMath::Lerp(-ConeHalfAngle, ConeHalfAngle,
				static_cast<float>(i) / static_cast<float>(ProjectileNum - 1));
		}

		Comp->SetWorldLocation(BasePosition);
		Comp->SetWorldRotation(FRotator(0.f, BaseYaw + AngleOffset, 0.f));
	}
}

void URageInMageSpreadProjectileSpell::DestroySpreadIndicators()
{
	for (UNiagaraComponent* Comp : SpreadIndicatorComponents)
	{
		if (IsValid(Comp))
		{
			Comp->Deactivate();
			Comp->DestroyComponent();
		}
	}
	SpreadIndicatorComponents.Empty();
}

void URageInMageSpreadProjectileSpell::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	DestroySpreadIndicators();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
