// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageBoulderAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Actor/RageInMageBoulder.h"
#include "GameFramework/Character.h"
#include "Player/RageInMagePlayerController.h"


float URageInMageBoulderAbility::GetGrowthTimeScalar() const
{
	// Negative percents, so this comes back BELOW 1 and shortens the grow time.
	return URageInMageAbilitySystemLibrary::GetImmovableMassStageScalar(
		GetAvatarActorFromActorInfo(), StanceBonusStage1, StanceBonusStage2, StanceBonusStage3);
}

ARageInMageBoulder* URageInMageBoulderAbility::SummonBoulder()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority()) return nullptr;
	if (!BoulderClass || ActiveBoulder) return nullptr;

	const FVector Forward = AvatarActor->GetActorForwardVector().GetSafeNormal2D();
	FVector SpawnLocation = AvatarActor->GetActorLocation() + Forward * SpawnForwardOffset;

	// Drop it onto the ground at the boulder's starting radius, so it doesn't spawn buried or
	// hovering. Its own tick keeps it there from then on.
	const ARageInMageBoulder* BoulderCDO = BoulderClass.GetDefaultObject();
	const float StartRadius = BoulderCDO ? BoulderCDO->StartRadius : 90.f;

	FCollisionQueryParams GroundParams;
	GroundParams.AddIgnoredActor(AvatarActor);
	FHitResult GroundHit;
	if (GetWorld()->LineTraceSingleByChannel(GroundHit, SpawnLocation + FVector(0.f, 0.f, 500.f),
		SpawnLocation - FVector(0.f, 0.f, 1000.f), ECC_Visibility, GroundParams))
	{
		SpawnLocation.Z = GroundHit.ImpactPoint.Z + StartRadius;
	}

	const FTransform SpawnTransform(Forward.Rotation(), SpawnLocation);

	ARageInMageBoulder* Boulder = GetWorld()->SpawnActorDeferred<ARageInMageBoulder>(
		BoulderClass, SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(AvatarActor),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!Boulder) return nullptr;

	// Build the damage spec once, and hand over the raw per-type values too — the boulder
	// re-stamps them as it grows, so a bigger boulder hits harder off the same spec.
	if (const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddInstigator(GetOwningActorFromActorInfo(), AvatarActor);

		FGameplayEffectSpecHandle SpecHandle =
			SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

		TMap<FGameplayTag, float> BaseDamage;
		for (const TPair<FGameplayTag, FScalableFloat>& Pair : DamageTypeTags)
		{
			const float ScaledValue = Pair.Value.GetValueAtLevel(GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledValue);
			BaseDamage.Add(Pair.Key, ScaledValue);
		}

		Boulder->DamageEffectSpecHandle = SpecHandle;
		Boulder->BaseDamageByType = BaseDamage;
	}

	Boulder->GrowthDuration = GrowthDuration * GetGrowthTimeScalar();
	Boulder->MaxLifetime = MaxLifetime;
	Boulder->FinishSpawning(SpawnTransform);

	ActiveBoulder = Boulder;
	Boulder->OnDestroyed.AddDynamic(this, &URageInMageBoulderAbility::OnBoulderDestroyed);

	if (ACharacter* Caster = Cast<ACharacter>(AvatarActor))
	{
		Boulder->Mount(Caster);
	}

	// Hand the movement stick over to steering for as long as the ride lasts.
	if (ARageInMagePlayerController* MagePC =
		Cast<ARageInMagePlayerController>(GetCurrentActorInfo() ? GetCurrentActorInfo()->PlayerController.Get() : nullptr))
	{
		MagePC->BeginBoulderRide(Boulder);
	}

	return Boulder;
}

void URageInMageBoulderAbility::DismountBoulder()
{
	if (!ActiveBoulder) return;

	ActiveBoulder->Dismount();

	// Movement goes back to the pawn immediately, even though the boulder rolls on without them.
	if (ARageInMagePlayerController* MagePC =
		Cast<ARageInMagePlayerController>(GetCurrentActorInfo() ? GetCurrentActorInfo()->PlayerController.Get() : nullptr))
	{
		MagePC->EndBoulderRide();
	}
}

void URageInMageBoulderAbility::OnBoulderDestroyed(AActor* DestroyedActor)
{
	ActiveBoulder = nullptr;

	if (ARageInMagePlayerController* MagePC =
		Cast<ARageInMagePlayerController>(GetCurrentActorInfo() ? GetCurrentActorInfo()->PlayerController.Get() : nullptr))
	{
		MagePC->EndBoulderRide();
	}

	K2_EndAbility();
}
