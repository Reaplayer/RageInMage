// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageDanceOfTheWindDragonAbility.h"

#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Character/RageInMageCharacterBase.h"
#include "DrawDebugHelpers.h"

URageInMageDanceOfTheWindDragonAbility::URageInMageDanceOfTheWindDragonAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URageInMageDanceOfTheWindDragonAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(AfterimageTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(DurationTimerHandle);
	}

	// Restore targetability.
	if (bRemovedPlayerTag)
	{
		if (AActor* AvatarActor = GetAvatarActorFromActorInfo())
		{
			AvatarActor->Tags.AddUnique(FName("Player"));
		}
		bRemovedPlayerTag = false;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URageInMageDanceOfTheWindDragonAbility::BeginDanceOfTheWindDragon()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !GetWorld())
	{
		K2_EndAbility();
		return;
	}

	// The zone is fixed at the cast location.
	ZoneCenter = AvatarActor->GetActorLocation();

	// Become untargetable: enemy AI locates players via the "Player" actor tag, so drop it for the duration.
	if (AvatarActor->ActorHasTag(FName("Player")))
	{
		AvatarActor->Tags.Remove(FName("Player"));
		bRemovedPlayerTag = true;
	}

	const float TotalDuration = FMath::Max(Duration.GetValueAtLevel(GetAbilityLevel()), 0.1f);
	const int32 Attacks = FMath::Max(NumAttacks, 1);
	const float AttackInterval = TotalDuration / Attacks;
	AttacksRemaining = Attacks;

	FTimerManager& TM = GetWorld()->GetTimerManager();
	TM.SetTimer(AttackTimerHandle, this, &URageInMageDanceOfTheWindDragonAbility::DoZoneAttack, AttackInterval, true, AttackInterval);
	TM.SetTimer(AfterimageTimerHandle, this, &URageInMageDanceOfTheWindDragonAbility::SpawnAfterimage, AfterimageInterval, true, 0.f);
	TM.SetTimer(DurationTimerHandle, this, &URageInMageDanceOfTheWindDragonAbility::FinishDance, TotalDuration, false);
}

void URageInMageDanceOfTheWindDragonAbility::DoZoneAttack()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return;

	if (AvatarActor->HasAuthority())
	{
		const float Radius = ZoneRadius.GetValueAtLevel(GetAbilityLevel());

		TArray<AActor*> Targets;
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(AvatarActor);
		URageInMageAbilitySystemLibrary::GetLivePlayersWithinRadius(
			AvatarActor, Radius, Targets, ActorsToIgnore, ZoneCenter);

		for (AActor* Target : Targets)
		{
			if (!Target || URageInMageAbilitySystemLibrary::IsFriendly(AvatarActor, Target)) continue;
			CauseDamage(Target);
			ApplyKnockback(Target, ZoneCenter);
		}

#if ENABLE_DRAW_DEBUG
		if (bShowDebug)
		{
			DrawDebugSphere(GetWorld(), ZoneCenter, Radius, 24, FColor::Cyan, false, 0.4f, 0, 2.f);
		}
#endif
	}

	if (--AttacksRemaining <= 0 && GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	}
}

void URageInMageDanceOfTheWindDragonAbility::SpawnAfterimage()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AfterimageActorClass || !GetWorld()) return;

	// Purely cosmetic ghost at the caster's current pose - trails their movement during the dance.
	FActorSpawnParameters Params;
	Params.Owner = AvatarActor;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<AActor>(AfterimageActorClass, AvatarActor->GetActorLocation(), AvatarActor->GetActorRotation(), Params);
}

void URageInMageDanceOfTheWindDragonAbility::FinishDance()
{
	K2_EndAbility();
}
