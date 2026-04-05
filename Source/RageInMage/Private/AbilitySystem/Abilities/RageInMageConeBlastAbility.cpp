// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageConeBlastAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
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
			if (ACharacter* TargetChar = Cast<ACharacter>(Target))
			{
				FVector PushDir = ConeDirection;
				PushDir.Z = FMath::Clamp(PushbackUpwardForce, 0.f, 1.f);
				PushDir.Normalize();
				TargetChar->LaunchCharacter(PushDir * Pushback, true, true);
			}
		}
	}
}
