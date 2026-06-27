// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageFlashAndAweAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "RageInMage/RageInMageLogChannels.h"

void URageInMageFlashAndAweAbility::ApplyFlashAndAwe(const FVector& TargetLocation)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority()) return;

	const FVector DepartLocation = AvatarActor->GetActorLocation();

	// Clamp to MaxSpellRange (0 = unlimited, inherited from URageInMageGameplayAbility).
	FVector Destination = TargetLocation;
	if (MaxSpellRange > 0.f)
	{
		const FVector ToTarget = TargetLocation - DepartLocation;
		if (ToTarget.SizeSquared() > FMath::Square(MaxSpellRange))
		{
			Destination = DepartLocation + ToTarget.GetSafeNormal() * MaxSpellRange;
		}
	}

	if (FlashDepartEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(AvatarActor, FlashDepartEffect, DepartLocation, FRotator::ZeroRotator);
	}

	// TeleportTo validates against blocking geometry (encroachment check) — it simply fails (returns
	// false, caster doesn't move) rather than dropping the caster inside a wall.
	const bool bTeleported = AvatarActor->TeleportTo(Destination, AvatarActor->GetActorRotation());
	if (!bTeleported)
	{
		UE_LOG(LogRageInMage, Warning, TEXT("[FlashAndAwe] TeleportTo blocked — stunning from current location instead."));
	}
	const FVector LandingCenter = AvatarActor->GetActorLocation();

	if (FlashSound)
	{
		UGameplayStatics::PlaySoundAtLocation(AvatarActor, FlashSound, LandingCenter);
	}

	const float Radius = StunRadius.GetValueAtLevel(GetAbilityLevel());

	if (FlashLandEffect)
	{
		UNiagaraComponent* LandVFX = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			AvatarActor, FlashLandEffect, LandingCenter, FRotator::ZeroRotator,
			FVector(1.f), /*bAutoDestroy=*/true, /*bAutoActivate=*/false);
		if (LandVFX)
		{
			LandVFX->SetVariableFloat(FName("StunRadius"), Radius);
			LandVFX->Activate();
		}
	}

	// Gather every living enemy within range of the landing point and stun them all — no damage,
	// guaranteed to land regardless of Charge/OverCharged or any existing StunImmune (see
	// ApplyGuaranteedStun, which strips StunImmune before applying).
	TArray<AActor*> OverlappingActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(AvatarActor);
	URageInMageAbilitySystemLibrary::GetLivePlayersWithinRadius(
		AvatarActor, Radius, OverlappingActors, ActorsToIgnore, LandingCenter);

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	const float ResolvedStunDuration = StunDuration.GetValueAtLevel(GetAbilityLevel());

	for (AActor* Target : OverlappingActors)
	{
		if (URageInMageAbilitySystemLibrary::IsBothEnemy(AvatarActor, Target)) continue;

		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
		{
			URageInMageAbilitySystemLibrary::ApplyGuaranteedStun(
				SourceASC, TargetASC, AvatarActor, ResolvedStunDuration, StunImmunityGraceSeconds);
		}
	}
}
