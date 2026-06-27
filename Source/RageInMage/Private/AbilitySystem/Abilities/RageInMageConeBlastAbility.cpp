// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageConeBlastAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Character/RageInMageCharacterBase.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


void URageInMageConeBlastAbility::ApplyConeBlast(const FVector& TargetLocation)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority()) return;

	const FVector Origin = AvatarActor->GetActorLocation();
	const float Range = ConeRange.GetValueAtLevel(GetAbilityLevel());
	const float Pushback = PushbackStrength.GetValueAtLevel(GetAbilityLevel());

	// Direction of the cone
	FVector ConeDirection = TargetLocation - Origin;
	ConeDirection.Z = 0.f;
	ConeDirection = ConeDirection.GetSafeNormal();
	if (ConeDirection.IsNearlyZero())
	{
		ConeDirection = AvatarActor->GetActorForwardVector();
	}

	// Debug: draw the cone hit area (flat if height-limited)
#if ENABLE_DRAW_DEBUG
	{
		const float VerticalAngle = (ConeHeightLimit > 0.f)
			? FMath::Atan2(ConeHeightLimit, Range)
			: FMath::DegreesToRadians(ConeHalfAngle);
		DrawDebugCone(
			GetWorld(), Origin, ConeDirection, Range,
			FMath::DegreesToRadians(ConeHalfAngle),
			VerticalAngle,
			12, FColor::Cyan, false, 3.f, 0, 2.f);
	}
#endif

	// VFX — spawn inactive, set user params, then activate
	if (ConeBlastEffect)
	{
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			AvatarActor, ConeBlastEffect, Origin,
			ConeDirection.Rotation(), FVector(1.f), /*bAutoDestroy=*/true,
			/*bAutoActivate=*/false);
		if (NiagaraComp)
		{
			NiagaraComp->SetVariableFloat(FName("ConeRange"), Range);
			NiagaraComp->SetVariableFloat(FName("ConeAngle"), ConeHalfAngle * 2.f);
			NiagaraComp->Activate();
		}
	}
	if (ConeBlastSound)
	{
		UGameplayStatics::PlaySoundAtLocation(AvatarActor, ConeBlastSound, Origin);
	}

	// Find all enemies in range
	TArray<AActor*> OverlappingActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(AvatarActor);
	URageInMageAbilitySystemLibrary::GetLivePlayersWithinRadius(
		AvatarActor, Range, OverlappingActors, ActorsToIgnore, Origin);

	// Create damage spec
	const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddInstigator(GetOwningActorFromActorInfo(), AvatarActor);
	const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
		DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
	for (const auto& Pair : DamageTypeTags)
	{
		const float ScaledValue = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledValue);
	}

	const float ConeAngleCos = FMath::Cos(FMath::DegreesToRadians(ConeHalfAngle));

	for (AActor* Target : OverlappingActors)
	{
		if (URageInMageAbilitySystemLibrary::IsBothEnemy(AvatarActor, Target)) continue;

		// Height check — skip targets too far above or below the caster
		if (ConeHeightLimit > 0.f)
		{
			const float HeightDiff = FMath::Abs(Target->GetActorLocation().Z - Origin.Z);
			if (HeightDiff > ConeHeightLimit) continue;
		}

		// Cone angle check via dot product
		FVector ToTarget = Target->GetActorLocation() - Origin;
		ToTarget.Z = 0.f;
		ToTarget = ToTarget.GetSafeNormal();
		if (FVector::DotProduct(ConeDirection, ToTarget) < ConeAngleCos) continue;

		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
		if (!TargetASC) continue;

		// Apply damage
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		// Apply on-hit effect (e.g. slow)
		if (OnHitEffectClass)
		{
			const FGameplayEffectSpecHandle OnHitSpec = SourceASC->MakeOutgoingSpec(
				OnHitEffectClass, GetAbilityLevel(), EffectContextHandle);
			TargetASC->ApplyGameplayEffectSpecToSelf(*OnHitSpec.Data.Get());
		}

		// Pushback in cone direction (not radial from center)
		if (Pushback > 0.f)
		{
			if (ARageInMageCharacterBase* TargetChar = Cast<ARageInMageCharacterBase>(Target))
			{
				// Switch to Falling so ground friction and AI pathfinding can't eat the impulse.
				// CMC auto-restores to Walking on landing.
				TargetChar->GetCharacterMovement()->SetMovementMode(MOVE_Falling);

				FVector PushDir = ConeDirection;
				PushDir.Z = FMath::Clamp(PushbackUpwardForce, 0.f, 1.f);
				PushDir.Normalize();
				TargetChar->ApplyKnockbackImpulse(PushDir * Pushback, true, true);
			}
		}
	}
}
