// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageLeapAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"


FVector URageInMageLeapAbility::CalculateLaunchVelocityToTarget(const FVector& TargetLocation) const
{
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return FVector::ZeroVector;

	const FVector StartLocation = AvatarActor->GetActorLocation();
	const FVector ToTarget = TargetLocation - StartLocation;
	const float HeightDiff = ToTarget.Z;

	// Horizontal direction and distance
	const FVector HorizontalDir = FVector(ToTarget.X, ToTarget.Y, 0.f).GetSafeNormal();
	const float HorizontalDistance = FVector(ToTarget.X, ToTarget.Y, 0.f).Size();

	if (HorizontalDistance < 1.f) return FVector(0.f, 0.f, LeapHeight.GetValueAtLevel(GetAbilityLevel()));

	const float PeakHeight = LeapHeight.GetValueAtLevel(GetAbilityLevel());
	const float Gravity = FMath::Abs(GetWorld()->GetGravityZ());

	if (Gravity < KINDA_SMALL_NUMBER) return FVector::ZeroVector;

	// Velocity needed to reach peak height above start: Vz = sqrt(2 * g * PeakHeight)
	const float Vz = FMath::Sqrt(2.f * Gravity * PeakHeight);

	// Time to reach peak from start
	const float TimeToPeak = Vz / Gravity;

	// Height of peak above target = PeakHeight - HeightDiff
	const float PeakAboveTarget = PeakHeight - HeightDiff;
	if (PeakAboveTarget < 0.f) return HorizontalDir * HorizontalDistance + FVector(0.f, 0.f, Vz);

	// Time to fall from peak to target height: t = sqrt(2 * PeakAboveTarget / g)
	const float TimeFromPeak = FMath::Sqrt(2.f * PeakAboveTarget / Gravity);

	const float TotalAirTime = TimeToPeak + TimeFromPeak;

	if (TotalAirTime < KINDA_SMALL_NUMBER) return FVector::ZeroVector;

	// Horizontal speed needed to cover the distance in that time
	const float HorizontalSpeed = HorizontalDistance / TotalAirTime;

	return HorizontalDir * HorizontalSpeed + FVector(0.f, 0.f, Vz);
}

TArray<FVector> URageInMageLeapAbility::GetLeapArcPreviewPoints(const FVector& TargetLocation, int32 NumPoints) const
{
	TArray<FVector> Points;

	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || NumPoints < 2) return Points;

	const FVector LaunchVelocity = CalculateLaunchVelocityToTarget(TargetLocation);
	if (LaunchVelocity.IsNearlyZero()) return Points;

	const FVector StartLocation = AvatarActor->GetActorLocation();
	const float Gravity = FMath::Abs(GetWorld()->GetGravityZ());
	const float PeakHeight = LeapHeight.GetValueAtLevel(GetAbilityLevel());
	const float Vz = FMath::Sqrt(2.f * Gravity * PeakHeight);
	const float TimeToPeak = Vz / Gravity;
	const float HeightDiff = TargetLocation.Z - StartLocation.Z;
	const float PeakAboveTarget = PeakHeight - HeightDiff;
	const float TimeFromPeak = PeakAboveTarget > 0.f ? FMath::Sqrt(2.f * PeakAboveTarget / Gravity) : 0.f;
	const float TotalAirTime = TimeToPeak + TimeFromPeak;

	if (TotalAirTime < KINDA_SMALL_NUMBER) return Points;

	Points.Reserve(NumPoints);
	for (int32 i = 0; i < NumPoints; ++i)
	{
		const float T = (static_cast<float>(i) / static_cast<float>(NumPoints - 1)) * TotalAirTime;
		const FVector Point = StartLocation + LaunchVelocity * T + FVector(0.f, 0.f, -0.5f * Gravity * T * T);
		Points.Add(Point);
	}

	return Points;
}

void URageInMageLeapAbility::ApplyLandingExplosion()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority()) return;

	const FVector ExplosionCenter = AvatarActor->GetActorLocation();
	const float Radius = ExplosionRadius.GetValueAtLevel(GetAbilityLevel());
	const float Pushback = PushbackStrength.GetValueAtLevel(GetAbilityLevel());

	// VFX — spawn inactive, set ExplosionRadius, then activate
	if (ExplosionEffect)
	{
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			AvatarActor, ExplosionEffect, ExplosionCenter,
			FRotator::ZeroRotator, FVector(1.f), /*bAutoDestroy=*/true,
			/*bAutoActivate=*/false);
		if (NiagaraComp)
		{
			NiagaraComp->SetVariableFloat(FName("ExplosionRadius"), Radius);
			NiagaraComp->Activate();
		}
	}
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(AvatarActor, ExplosionSound, ExplosionCenter);
	}

	// Find all enemies in range
	TArray<AActor*> OverlappingActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(AvatarActor);
	URageInMageAbilitySystemLibrary::GetLivePlayersWithinRadius(
		AvatarActor, Radius, OverlappingActors, ActorsToIgnore, ExplosionCenter);

	// Create damage spec
	const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddInstigator(GetOwningActorFromActorInfo(), AvatarActor);
	const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
	for (auto& Pair : DamageTypeTags)
	{
		const float ScaledValue = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledValue);
	}

	for (AActor* Target : OverlappingActors)
	{
		if (URageInMageAbilitySystemLibrary::IsBothEnemy(AvatarActor, Target)) continue;

		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}

		// Apply pushback — direction away from explosion center, scaled by distance
		if (ACharacter* TargetCharacter = Cast<ACharacter>(Target))
		{
			FVector PushDirection = (Target->GetActorLocation() - ExplosionCenter).GetSafeNormal();
			PushDirection.Z = 0.3f; // Slight upward push
			PushDirection.Normalize();
			TargetCharacter->LaunchCharacter(PushDirection * Pushback, true, true);
		}
	}
}
