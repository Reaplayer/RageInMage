// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageChargedFireball.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Actor/RageInMageSphereProjectile.h"
#include "Actor/RageInMageFireZone.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"


URageInMageChargedFireball::URageInMageChargedFireball()
{
}

// ──────────────────────────────────────────
// Charge State
// ──────────────────────────────────────────

void URageInMageChargedFireball::BeginCharge()
{
	CurrentChargeTime = 0.f;
	bIsCharging = true;
	OnChargeBegin();
}

float URageInMageChargedFireball::GetChargePercent() const
{
	if (MaxChargeTime <= 0.f) return 1.f;
	return FMath::Clamp(CurrentChargeTime / MaxChargeTime, 0.f, 1.f);
}

float URageInMageChargedFireball::GetCurrentArc() const
{
	// Legacy fallback — prefer GetArcForDistance() for the new distance-based system
	return FMath::Lerp(MinChargeArc, MaxChargeArc, GetChargePercent());
}

float URageInMageChargedFireball::GetArcForDistance(const FVector& LaunchLocation, const FVector& TargetLocation) const
{
	const float Distance = FVector::Dist2D(LaunchLocation, TargetLocation);
	const float Range = (MaxSpellRange > 0.f) ? MaxSpellRange : 2000.f;
	return URageInMageAbilitySystemLibrary::GetArcFromDistance(Distance, Range, MinChargeArc, MaxChargeArc);
}

// ──────────────────────────────────────────
// Per-Frame Update
// ──────────────────────────────────────────

FVector URageInMageChargedFireball::UpdateChargeAndIndicator(
	float DeltaTime, const FVector& AimLocation, const FGameplayTag& SocketTag)
{
	if (!bIsCharging) return AimLocation;

	CurrentChargeTime = FMath::Min(CurrentChargeTime + DeltaTime, MaxChargeTime);

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return AimLocation;

	const FVector LaunchLocation = ICombatInterface::Execute_GetCombatSocketLocation(AvatarActor, SocketTag);

	// Use distance-based arc instead of charge-time-based
	FVector LandingLocation = URageInMageAbilitySystemLibrary::SnapToGround(this, AimLocation);
	const float Arc = GetArcForDistance(LaunchLocation, LandingLocation);
	TArray<FVector> ArcPoints = URageInMageAbilitySystemLibrary::CalculateProjectileArcPoints(
		this, LaunchLocation, LandingLocation, Arc, ArcPreviewPointCount);

	OnChargeUpdate(LandingLocation, GetChargePercent(), ArcPoints);

	return LandingLocation;
}

// ──────────────────────────────────────────
// Release (spawns the projectile -- server only)
// ──────────────────────────────────────────

void URageInMageChargedFireball::ReleaseChargedFireball(
	const FVector& TargetLocation, const FGameplayTag& SocketTag)
{
	if (!bIsCharging) return;
	bIsCharging = false;
	OnChargeEnd();

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority()) return;
	if (!ChargedProjectileClass) return;

	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(AvatarActor, SocketTag);
	const float Arc = GetArcForDistance(SocketLocation, TargetLocation);

	// Calculate launch velocity using distance-based arc
	FVector LaunchVelocity;
	const bool bHaveSolution = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		this, LaunchVelocity, SocketLocation, TargetLocation, 0, Arc);

	FRotator Rotation = bHaveSolution
		? LaunchVelocity.Rotation()
		: (TargetLocation - SocketLocation).Rotation();

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	ARageInMageSphereProjectile* Projectile = GetWorld()->SpawnActorDeferred<ARageInMageSphereProjectile>(
		ChargedProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(AvatarActor),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!Projectile) return;

	// Set impact damage spec
	Projectile->DamageEffectSpecHandle = MakeImpactDamageSpec();

	// Set AoE explosion radius and knockback
	Projectile->AoERadius = ImpactExplosionRadius.GetValueAtLevel(GetAbilityLevel());
	Projectile->KnockbackStrength = KnockbackStrength.GetValueAtLevel(GetAbilityLevel());
	Projectile->KnockbackUpwardForce = KnockbackUpwardForce;

	// Configure fire zone data on the projectile
	if (FireZoneClass)
	{
		Projectile->FireZoneClass = FireZoneClass;
		Projectile->FireZoneDamageEffectSpecHandle = MakeFireZoneDamageSpec();
		Projectile->FireZoneRadius = FireZoneRadius.GetValueAtLevel(GetAbilityLevel());
		Projectile->FireZoneDuration = FireZoneDuration.GetValueAtLevel(GetAbilityLevel());
		Projectile->FireZoneTickInterval = FireZoneTickInterval;
	}

	// Override the projectile movement for arc trajectory
	if (Projectile->ProjectileMovement)
	{
		Projectile->ProjectileMovement->ProjectileGravityScale = 1.f;
		// LaunchVelocity is in world space, not local space
		Projectile->ProjectileMovement->bInitialVelocityInLocalSpace = false;
		if (bHaveSolution)
		{
			Projectile->ProjectileMovement->InitialSpeed = LaunchVelocity.Size();
			Projectile->ProjectileMovement->MaxSpeed = LaunchVelocity.Size();
			Projectile->ProjectileMovement->Velocity = LaunchVelocity;
		}
	}

	Projectile->FinishSpawning(SpawnTransform);
}

void URageInMageChargedFireball::CancelCharge()
{
	if (bIsCharging)
	{
		bIsCharging = false;
		OnChargeEnd();
	}
}

// ──────────────────────────────────────────
// EndAbility cleanup
// ──────────────────────────────────────────

void URageInMageChargedFireball::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (bIsCharging)
	{
		CancelCharge();
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// ──────────────────────────────────────────
// Arc calculation helpers (delegating to shared utilities)
// ──────────────────────────────────────────

TArray<FVector> URageInMageChargedFireball::CalculateArcPoints(
	const FVector& LaunchLocation, const FVector& TargetLocation, float Arc) const
{
	return URageInMageAbilitySystemLibrary::CalculateProjectileArcPoints(
		this, LaunchLocation, TargetLocation, Arc, ArcPreviewPointCount);
}

FVector URageInMageChargedFireball::CalculateLandingLocation(
	const FVector& LaunchLocation, const FVector& AimLocation) const
{
	return URageInMageAbilitySystemLibrary::SnapToGround(this, AimLocation);
}

// ──────────────────────────────────────────
// Damage spec creation
// ──────────────────────────────────────────

FGameplayEffectSpecHandle URageInMageChargedFireball::MakeImpactDamageSpec() const
{
	const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddInstigator(GetOwningActorFromActorInfo(), GetAvatarActorFromActorInfo());

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
		DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

	// Apply charge-based damage multiplier
	const float ChargeMultiplier = ChargeDamageMultiplierCurve
		? ChargeDamageMultiplierCurve->GetFloatValue(GetChargePercent())
		: 1.0f;

	for (const auto& Pair : DamageTypeTags)
	{
		const float ScaledValue = Pair.Value.GetValueAtLevel(GetAbilityLevel()) * ChargeMultiplier;
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledValue);
	}

	return SpecHandle;
}

FGameplayEffectSpecHandle URageInMageChargedFireball::MakeFireZoneDamageSpec() const
{
	const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddInstigator(GetOwningActorFromActorInfo(), GetAvatarActorFromActorInfo());

	TSubclassOf<UGameplayEffect> GEClass = FireZoneDamageEffectClass ? FireZoneDamageEffectClass : DamageEffectClass;
	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
		GEClass, GetAbilityLevel(), EffectContextHandle);

	const TMap<FGameplayTag, FScalableFloat>& Tags = FireZoneDamageTypeTags.Num() > 0
		? FireZoneDamageTypeTags : DamageTypeTags;

	for (const auto& Pair : Tags)
	{
		const float ScaledValue = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledValue);
	}

	return SpecHandle;
}
