// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageChargedSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Actor/RageInMageSphereProjectile.h"
#include "Actor/RageInMageZone.h"
#include "Character/RageInMageCharacterBase.h"
#include "RageInMageGameplayTag.h"
#include "Components/AudioComponent.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"


URageInMageChargedSpell::URageInMageChargedSpell()
{
}

// ──────────────────────────────────────────
// Charge State
// ──────────────────────────────────────────

void URageInMageChargedSpell::BeginCharge(const FGameplayTag& SocketTag)
{
	CurrentChargeTime = 0.f;
	bIsCharging = true;

	// Spawn charge VFX/SFX attached to the combat socket (weapon tip, hand, etc.)
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (AvatarActor)
	{
		// Resolve the socket tag to a mesh component + socket name
		USceneComponent* AttachComponent = AvatarActor->GetRootComponent();
		FName AttachSocket = NAME_None;

		if (const ARageInMageCharacterBase* CharBase = Cast<ARageInMageCharacterBase>(AvatarActor))
		{
			const FRageInMageGameplayTag& GameplayTags = FRageInMageGameplayTag::Get();
			if (SocketTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon) && CharBase->GetWeapon())
			{
				AttachComponent = CharBase->GetWeapon();
				AttachSocket = CharBase->GetWeaponTipSocketName();
			}
			else if (SocketTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))
			{
				AttachComponent = CharBase->GetMesh();
				AttachSocket = CharBase->GetLeftHandSocketName();
			}
			else if (SocketTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
			{
				AttachComponent = CharBase->GetMesh();
				AttachSocket = CharBase->GetRightHandSocketName();
			}
		}

		if (ChargeEffect)
		{
			ActiveChargeEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
				ChargeEffect, AttachComponent, AttachSocket,
				FVector::ZeroVector, FRotator::ZeroRotator,
				EAttachLocation::SnapToTarget, true);

			if (ActiveChargeEffectComponent)
			{
				// Force zero world rotation so particles aren't affected by the weapon socket's rotation
				ActiveChargeEffectComponent->SetWorldRotation(FRotator::ZeroRotator);
			}
		}
		if (ChargeSound)
		{
			ActiveChargeSoundComponent = UGameplayStatics::SpawnSoundAttached(
				ChargeSound, AttachComponent, AttachSocket);
		}
	}

	OnChargeBegin();
}

float URageInMageChargedSpell::GetChargePercent() const
{
	if (MaxChargeTime <= 0.f) return MaxChargePercent;

	// 0% during delay period, then ramp to MaxChargePercent over remaining time
	const float EffectiveTime = FMath::Max(0.f, CurrentChargeTime - ChargeDelay);
	const float RampDuration = FMath::Max(0.01f, MaxChargeTime - ChargeDelay);
	return FMath::Clamp(EffectiveTime / RampDuration, 0.f, MaxChargePercent);
}

float URageInMageChargedSpell::GetCurrentArc() const
{
	// Legacy fallback — prefer GetArcForDistance() for the new distance-based system
	return FMath::Lerp(MinChargeArc, MaxChargeArc, GetChargePercent());
}

float URageInMageChargedSpell::GetArcForDistance(const FVector& LaunchLocation, const FVector& TargetLocation) const
{
	const float Distance = FVector::Dist2D(LaunchLocation, TargetLocation);
	const float Range = (MaxSpellRange > 0.f) ? MaxSpellRange : 2000.f;
	return URageInMageAbilitySystemLibrary::GetArcFromDistance(Distance, Range, MinChargeArc, MaxChargeArc);
}

// ──────────────────────────────────────────
// Per-Frame Update
// ──────────────────────────────────────────

FVector URageInMageChargedSpell::UpdateChargeAndIndicator(
	float DeltaTime, const FVector& AimLocation, const FGameplayTag& SocketTag)
{
	if (!bIsCharging) return AimLocation;

	CurrentChargeTime = FMath::Min(CurrentChargeTime + DeltaTime, MaxChargeTime);

	// Push charge percent to Niagara so the VFX can scale with it (e.g. 1 + ChargePercent)
	if (ActiveChargeEffectComponent)
	{
		ActiveChargeEffectComponent->SetVariableFloat(FName("ChargePercent"), GetChargePercent());
	}

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return AimLocation;

	const FVector LaunchLocation = ICombatInterface::Execute_GetCombatSocketLocation(AvatarActor, SocketTag);

	// Use distance-based arc instead of charge-time-based
	FVector LandingLocation = URageInMageAbilitySystemLibrary::SnapToGround(this, AimLocation);
	const float Arc = GetArcForDistance(LaunchLocation, LandingLocation);
	TArray<FVector> ArcPoints = URageInMageAbilitySystemLibrary::CalculateProjectileArcPoints(
		this, LaunchLocation, LandingLocation, Arc, ArcPreviewPointCount);

	// Indicator radius matches actual AoE at release: base * (1 + charge)
	const float CurrentChargePercent = GetChargePercent();
	const float IndicatorRadius = ImpactExplosionRadius.GetValueAtLevel(GetAbilityLevel()) * (1.f + CurrentChargePercent);

	OnChargeUpdate(LandingLocation, CurrentChargePercent, IndicatorRadius, ArcPoints);

	return LandingLocation;
}

// ──────────────────────────────────────────
// Release (spawns the projectile -- server only)
// ──────────────────────────────────────────

void URageInMageChargedSpell::ReleaseChargedSpell(
	const FVector& TargetLocation, const FGameplayTag& SocketTag)
{
	if (!bIsCharging) return;
	bIsCharging = false;

	if (ActiveChargeEffectComponent)
	{
		ActiveChargeEffectComponent->DestroyComponent();
		ActiveChargeEffectComponent = nullptr;
	}
	if (ActiveChargeSoundComponent)
	{
		ActiveChargeSoundComponent->Stop();
		ActiveChargeSoundComponent = nullptr;
	}

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

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	// Keep projectile upright so local-space VFX (flames from top) aren't rotated by aim direction.
	// Flight trajectory is unaffected because velocity is set in world space (bInitialVelocityInLocalSpace = false).
	SpawnTransform.SetRotation(FQuat::Identity);

	ARageInMageSphereProjectile* Projectile = GetWorld()->SpawnActorDeferred<ARageInMageSphereProjectile>(
		ChargedProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(AvatarActor),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!Projectile) return;

	// Charge multiplier: at 0% charge -> 1x, at 100% charge -> 2x
	const float Charge = GetChargePercent();
	const float ChargeMultiplier = 1.f + Charge;

	// Set charge percent for VFX scaling
	Projectile->ChargePercent = Charge;

	// Set impact damage spec (charge scaling applied inside MakeImpactDamageSpec)
	Projectile->DamageEffectSpecHandle = MakeImpactDamageSpec();

	// Set AoE explosion radius (scaled by charge) and knockback
	Projectile->AoERadius = ImpactExplosionRadius.GetValueAtLevel(GetAbilityLevel()) * ChargeMultiplier;
	Projectile->KnockbackStrength = KnockbackStrength.GetValueAtLevel(GetAbilityLevel()) * ChargeMultiplier;
	Projectile->KnockbackUpwardForce = KnockbackUpwardForce;

	// Pass VFX/SFX from the ability to the projectile
	if (ImpactExplosionEffect) Projectile->AoEExplosionEffect = ImpactExplosionEffect;
	if (ImpactSound) Projectile->ImpactSound = ImpactSound;

	// Configure zone data on the projectile (zone radius scaled by charge)
	if (ZoneClass)
	{
		Projectile->ZoneClass = ZoneClass;
		Projectile->ZoneDamageEffectSpecHandle = MakeZoneDamageSpec();
		Projectile->ZoneRadius = ZoneRadius.GetValueAtLevel(GetAbilityLevel()) * ChargeMultiplier;
		Projectile->ZoneDuration = ZoneDuration.GetValueAtLevel(GetAbilityLevel());
		Projectile->ZoneTickInterval = ZoneTickInterval;
		if (ZoneEffect) Projectile->ZoneEffect = ZoneEffect;
		if (ZoneLoopSound) Projectile->ZoneLoopSound = ZoneLoopSound;
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

void URageInMageChargedSpell::CancelCharge()
{
	if (bIsCharging)
	{
		bIsCharging = false;

		if (ActiveChargeEffectComponent)
		{
			ActiveChargeEffectComponent->DestroyComponent();
			ActiveChargeEffectComponent = nullptr;
		}
		if (ActiveChargeSoundComponent)
		{
			ActiveChargeSoundComponent->Stop();
			ActiveChargeSoundComponent = nullptr;
		}

		OnChargeEnd();
	}
}

// ──────────────────────────────────────────
// EndAbility cleanup
// ──────────────────────────────────────────

void URageInMageChargedSpell::EndAbility(
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

TArray<FVector> URageInMageChargedSpell::CalculateArcPoints(
	const FVector& LaunchLocation, const FVector& TargetLocation, float Arc) const
{
	return URageInMageAbilitySystemLibrary::CalculateProjectileArcPoints(
		this, LaunchLocation, TargetLocation, Arc, ArcPreviewPointCount);
}

FVector URageInMageChargedSpell::CalculateLandingLocation(
	const FVector& LaunchLocation, const FVector& AimLocation) const
{
	return URageInMageAbilitySystemLibrary::SnapToGround(this, AimLocation);
}

// ──────────────────────────────────────────
// Damage spec creation
// ──────────────────────────────────────────

FGameplayEffectSpecHandle URageInMageChargedSpell::MakeImpactDamageSpec() const
{
	const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddInstigator(GetOwningActorFromActorInfo(), GetAvatarActorFromActorInfo());

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
		DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

	// Apply charge-based damage multiplier: curve if set, otherwise (1 + ChargePercent)
	const float DamageChargeMultiplier = ChargeDamageMultiplierCurve
		? ChargeDamageMultiplierCurve->GetFloatValue(GetChargePercent())
		: (1.f + GetChargePercent());

	for (const auto& Pair : DamageTypeTags)
	{
		const float ScaledValue = Pair.Value.GetValueAtLevel(GetAbilityLevel()) * DamageChargeMultiplier;
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledValue);
	}

	return SpecHandle;
}

FGameplayEffectSpecHandle URageInMageChargedSpell::MakeZoneDamageSpec() const
{
	const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddInstigator(GetOwningActorFromActorInfo(), GetAvatarActorFromActorInfo());

	TSubclassOf<UGameplayEffect> GEClass = ZoneDamageEffectClass ? ZoneDamageEffectClass : DamageEffectClass;
	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
		GEClass, GetAbilityLevel(), EffectContextHandle);

	const TMap<FGameplayTag, FScalableFloat>& Tags = ZoneDamageTypeTags.Num() > 0
		? ZoneDamageTypeTags : DamageTypeTags;

	for (const auto& Pair : Tags)
	{
		const float ScaledValue = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledValue);
	}

	return SpecHandle;
}
