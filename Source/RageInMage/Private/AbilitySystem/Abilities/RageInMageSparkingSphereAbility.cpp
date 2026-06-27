// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageSparkingSphereAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/RageInMageSparkingSphereZone.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/ProjectileMovementComponent.h"

void URageInMageSparkingSphereAbility::BeginSparkingSphere(const FVector& TargetLocation)
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar || !ZoneClass) return;

	const FVector SpawnLocation = Avatar->GetActorLocation();

	FVector Direction = TargetLocation - SpawnLocation;
	Direction.Z = 0.f;
	Direction = Direction.GetSafeNormal();
	if (Direction.IsNearlyZero())
	{
		Direction = Avatar->GetActorForwardVector();
	}

#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		DrawDebugDirectionalArrow(GetWorld(), SpawnLocation, SpawnLocation + Direction * 500.f, 50.f, FColor::Cyan, false, 3.f, 0, 4.f);
	}
#endif

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);
	SpawnTransform.SetRotation(Direction.Rotation().Quaternion());

	ARageInMageSparkingSphereZone* Zone = GetWorld()->SpawnActorDeferred<ARageInMageSparkingSphereZone>(
		ZoneClass, SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(Avatar),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (Zone)
	{
		Zone->DamageEffectSpecHandle = MakeSparkingSphereDamageSpec();
		Zone->ZoneRadius = ZoneRadius.GetValueAtLevel(GetAbilityLevel());
		Zone->ZoneDuration = ZoneDuration.GetValueAtLevel(GetAbilityLevel());
		Zone->DamageTickInterval = DamageTickInterval;
		Zone->ZoneEffect = ZoneEffect;
		Zone->ZoneLoopSound = ZoneLoopSound;
		Zone->bShowDebug = bShowDebug;

		if (Zone->ProjectileMovement)
		{
			Zone->ProjectileMovement->InitialSpeed = SphereSpeed;
			Zone->ProjectileMovement->MaxSpeed = SphereSpeed;
		}

		Zone->FinishSpawning(SpawnTransform);
	}
}

FGameplayEffectSpecHandle URageInMageSparkingSphereAbility::MakeSparkingSphereDamageSpec() const
{
	const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddInstigator(GetOwningActorFromActorInfo(), GetAvatarActorFromActorInfo());

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
	for (const auto& Pair : DamageTypeTags)
	{
		const float ScaledValue = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledValue);
	}
	return SpecHandle;
}
