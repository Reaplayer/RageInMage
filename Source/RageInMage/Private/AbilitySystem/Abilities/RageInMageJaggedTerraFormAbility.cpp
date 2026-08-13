// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageJaggedTerraFormAbility.h"

#include "DrawDebugHelpers.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Actor/RageInMageSpikeField.h"
#include "Character/RageInMageCharacterBase.h"


float URageInMageJaggedTerraFormAbility::GetFieldAreaScalar() const
{
	return URageInMageAbilitySystemLibrary::GetImmovableMassStageScalar(
		GetAvatarActorFromActorInfo(), StanceBonusStage1, StanceBonusStage2, StanceBonusStage3);
}

float URageInMageJaggedTerraFormAbility::GetScaledFieldRadius() const
{
	return FieldRadius * GetFieldAreaScalar();
}

void URageInMageJaggedTerraFormAbility::ApplyDirectionalKnockback(AActor* Target, const FVector& Direction) const
{
	const float Strength = KnockbackStrength.GetValueAtLevel(GetAbilityLevel());
	if (Strength <= 0.f || !Target) return;

	ARageInMageCharacterBase* TargetCharacter = Cast<ARageInMageCharacterBase>(Target);
	if (!TargetCharacter) return;

	// Same impulse shape as the parent's ApplyKnockback, but the horizontal direction is given
	// rather than derived from an origin — everyone gets shoved the same way.
	FVector PushDirection = Direction.GetSafeNormal2D();
	if (PushDirection.IsNearlyZero()) return;

	PushDirection.Z = FMath::Clamp(KnockbackUpwardForce, 0.f, 1.f);
	PushDirection.Normalize();
	TargetCharacter->ApplyKnockbackImpulse(PushDirection * Strength, true, true);
}

ARageInMageSpikeField* URageInMageJaggedTerraFormAbility::EruptSpikes(const FVector& Location, const FVector& PushDirection)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority()) return nullptr;

	FVector Push = PushDirection.GetSafeNormal2D();
	if (Push.IsNearlyZero())
	{
		Push = AvatarActor->GetActorForwardVector().GetSafeNormal2D();
	}

	const float AreaScalar = GetFieldAreaScalar();
	const float ScaledRadius = FieldRadius * AreaScalar;

	// Damage + push everyone caught in the eruption. One burst, at cast time — the spawned field
	// is only the barrier that stays behind.
	TArray<AActor*> OverlappingActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(AvatarActor);
	URageInMageAbilitySystemLibrary::GetLivePlayersWithinRadius(
		GetAvatarActorFromActorInfo(), ScaledRadius, OverlappingActors, ActorsToIgnore, Location);

	for (AActor* Target : OverlappingActors)
	{
		if (!Target || URageInMageAbilitySystemLibrary::IsFriendly(AvatarActor, Target)) continue;

		CauseDamage(Target);
		ApplyDirectionalKnockback(Target, Push);
	}

	if (!SpikeFieldClass) return nullptr;

	// The barrier faces along the push, so the spikes read as a wall the enemies were shoved into.
	const FTransform SpawnTransform(Push.Rotation(), Location, FVector(AreaScalar));

	ARageInMageSpikeField* Field = GetWorld()->SpawnActorDeferred<ARageInMageSpikeField>(
		SpikeFieldClass, SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(AvatarActor),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (Field)
	{
		// Only override the actor's own default when the ability actually specifies a duration —
		// a 0 here would reach SetLifeSpan(0), which means "lives forever", not "expires instantly".
		const float Duration = FieldDuration.GetValueAtLevel(GetAbilityLevel());
		if (Duration > 0.f)
		{
			Field->FieldDuration = Duration;
		}
		Field->FinishSpawning(SpawnTransform);
	}

	return Field;
}

void URageInMageJaggedTerraFormAbility::DrawDebugSpikeFieldPreview(const FVector& Location, const FVector& Direction)
{
#if ENABLE_DRAW_DEBUG
	if (!GetWorld()) return;

	const float ScaledRadius = GetScaledFieldRadius();

	FVector Push = Direction.GetSafeNormal2D();
	if (Push.IsNearlyZero())
	{
		const AActor* AvatarActor = GetAvatarActorFromActorInfo();
		Push = AvatarActor ? AvatarActor->GetActorForwardVector().GetSafeNormal2D() : FVector::ForwardVector;
	}

	// The circle is the damage/push query (the real hit area); the arrow is which way they go.
	DrawDebugCircle(GetWorld(), Location + FVector(0.f, 0.f, 5.f), ScaledRadius, 48,
		FColor::Orange, false, 0.05f, 0, 3.f,
		FVector(1.f, 0.f, 0.f), FVector(0.f, 1.f, 0.f), /*bDrawAxis=*/false);

	DrawDebugDirectionalArrow(GetWorld(), Location + FVector(0.f, 0.f, 5.f),
		Location + Push * ScaledRadius + FVector(0.f, 0.f, 5.f),
		80.f, FColor::Orange, false, 0.05f, 0, 4.f);
#endif
}
