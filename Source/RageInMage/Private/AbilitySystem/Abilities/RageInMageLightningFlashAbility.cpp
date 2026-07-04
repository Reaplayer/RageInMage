// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageLightningFlashAbility.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "DrawDebugHelpers.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "RageInMageGameplayTag.h"
#include "RageInMage/RageInMageLogChannels.h"

void URageInMageLightningFlashAbility::ApplyLightningFlash(const FVector& TargetLocation)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority()) return;

	const FVector Origin = AvatarActor->GetActorLocation();
	const float Range = ConeRange.GetValueAtLevel(GetAbilityLevel());
	const int32 NumRods = FMath::Max(1, FMath::RoundToInt(RodCount.GetValueAtLevel(GetAbilityLevel())));

	FVector ConeDirection = TargetLocation - Origin;
	ConeDirection.Z = 0.f;
	ConeDirection = ConeDirection.GetSafeNormal();
	if (ConeDirection.IsNearlyZero())
	{
		ConeDirection = AvatarActor->GetActorForwardVector();
	}

	if (LightningFlashSound)
	{
		UGameplayStatics::PlaySoundAtLocation(AvatarActor, LightningFlashSound, Origin);
	}

	// Debug: draw the cone hit area (flat if height-limited) — same convention as ConeBlastAbility.
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

	// Gather living enemies inside the cone
	TArray<AActor*> OverlappingActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(AvatarActor);
	URageInMageAbilitySystemLibrary::GetLivePlayersWithinRadius(
		AvatarActor, Range, OverlappingActors, ActorsToIgnore, Origin);

	const float ConeAngleCos = FMath::Cos(FMath::DegreesToRadians(ConeHalfAngle));

	TArray<AActor*> TargetsToHit;
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

		TargetsToHit.Add(Target);
	}

	if (TargetsToHit.Num() == 0) return;

	const FVector WeaponTipLocation = ICombatInterface::Execute_GetCombatSocketLocation(
		AvatarActor, FRageInMageGameplayTag::Get().CombatSocket_Weapon);

	/*  Fire rods at random targets in the cone, without replacement — each rod picks a target out of
		TargetsToHit and moves it to TargetsHit, so nobody gets hit twice before everyone's been hit
		once. Once TargetsToHit runs dry with rods still left, TargetsHit refills it and the random
		draw starts over. Each rod is its own chain-lightning run (own crit roll, own Charge gain,
		and can jump onward from its target via ApplyChainLightningDamage — the same helper any future
		Lightning spell should call to get chaining "for free") and spawns its own impact VFX, so the
		number of rods fired always equals both the initial-hit count and the VFX spawn count.	*/
	TArray<AActor*> TargetsHit;
	for (int32 RodIndex = 0; RodIndex < NumRods; ++RodIndex)
	{
		if (TargetsToHit.Num() == 0)
		{
			TargetsToHit = MoveTemp(TargetsHit);
			TargetsHit.Reset();
		}

		const int32 RandomIndex = FMath::RandRange(0, TargetsToHit.Num() - 1);
		AActor* Target = TargetsToHit[RandomIndex];
		TargetsToHit.RemoveAt(RandomIndex);
		TargetsHit.Add(Target);

		// Inherited from URageInMageLightningDamageAbility — builds this rod's own spec and routes it through ApplyChainLightningDamage, which applies it to Target and jumps onward on its own.
		CauseChainDamage(Target);

		if (RodImpactEffect)
		{
			// Spawn the rod at the weapon tip and aim its beam at the target's world position —
			// NS_LightningShock's only exposed user parameter, BeamEnd, expects an absolute world
			// position (same convention as RageInMageBeamSpell's BeamStart/BeamEnd), not an offset.
			UNiagaraComponent* RodVFX = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				AvatarActor, RodImpactEffect, WeaponTipLocation, FRotator::ZeroRotator);
			if (RodVFX)
			{
				RodVFX->SetVariablePosition(FName("BeamEnd"), Target->GetActorLocation());
			}
		}
	}
}
