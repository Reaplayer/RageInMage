// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageYoyoSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AbilityTasks/RageInMageAbilityTask_YoyoControl.h"
#include "Actor/RageInMageYoyoProjectile.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"


void URageInMageYoyoSpell::LaunchYoyo(const FVector& TargetLocation, const FGameplayTag& SocketTag)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority() || !YoyoProjectileClass) return;

	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(AvatarActor, SocketTag);

	// Direction toward target (horizontal only)
	FVector LaunchDirection = TargetLocation - SocketLocation;
	LaunchDirection.Z = 0.f;
	LaunchDirection = LaunchDirection.GetSafeNormal();
	if (LaunchDirection.IsNearlyZero())
	{
		LaunchDirection = AvatarActor->GetActorForwardVector();
	}

	// Reset chain state
	CurrentChain = 0;
	CurrentSpeed = InitialSpeed;
	CurrentDamageMultiplier = 1.f;
	CurrentScaleMultiplier = 1.f;

	// Spawn the yo-yo projectile deferred
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(LaunchDirection.Rotation().Quaternion());

	ActiveProjectile = GetWorld()->SpawnActorDeferred<ARageInMageYoyoProjectile>(
		YoyoProjectileClass, SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(AvatarActor),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (ActiveProjectile)
	{
		// Damage spec
		ActiveProjectile->DamageEffectSpecHandle = MakeYoyoDamageSpec();

		// Knockback from parent class
		ActiveProjectile->KnockbackStrength = KnockbackStrength.GetValueAtLevel(GetAbilityLevel());
		ActiveProjectile->KnockbackUpwardForce = KnockbackUpwardForce;

		// Pull physics
		ActiveProjectile->PullOrigin = AvatarActor->GetActorLocation();
		ActiveProjectile->PullStrength = PullStrength;
		ActiveProjectile->OriginReturnRadius = OriginReturnRadius;
		ActiveProjectile->bPullActive = true;
		ActiveProjectile->CurrentVelocity = LaunchDirection * CurrentSpeed;
		ActiveProjectile->CurrentScaleMultiplier = CurrentScaleMultiplier;

		// Puddle zone config (spawned on impact by projectile)
		ActiveProjectile->PuddleZoneClass = PuddleZoneClass;
		ActiveProjectile->PuddleDamageEffectSpecHandle = MakePuddleDamageSpec();
		ActiveProjectile->PuddleZoneRadius = BasePuddleZoneRadius;
		ActiveProjectile->PuddleZoneDuration = PuddleZoneDuration;
		ActiveProjectile->PuddleZoneTickInterval = PuddleZoneTickInterval;
		if (PuddleZoneEffect) ActiveProjectile->PuddleZoneEffect = PuddleZoneEffect;
		if (PuddleZoneLoopSound) ActiveProjectile->PuddleZoneLoopSound = PuddleZoneLoopSound;

		ActiveProjectile->FinishSpawning(SpawnTransform);
	}

	// SFX
	if (LaunchSound)
	{
		UGameplayStatics::PlaySoundAtLocation(AvatarActor, LaunchSound, SocketLocation);
	}

	OnYoyoLaunched(LaunchDirection);

	// Start the control task
	StartYoyoControlTask();
}

bool URageInMageYoyoSpell::RecastYoyo(const FVector& NewTargetLocation)
{
	if (!ActiveProjectile || CurrentChain >= MaxChains) return false;

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority()) return false;

	// Validate recast direction: new direction must be roughly opposite the current heading
	const FVector CurrentHeading = ActiveProjectile->CurrentVelocity.GetSafeNormal();
	FVector NewDirection = NewTargetLocation - ActiveProjectile->GetActorLocation();
	NewDirection.Z = 0.f;
	NewDirection = NewDirection.GetSafeNormal();

	if (NewDirection.IsNearlyZero())
	{
		NewDirection = -CurrentHeading;
	}

	// The recast direction should be within RecastConeHalfAngle of the OPPOSITE of current heading
	const FVector OppositeHeading = -CurrentHeading;
	const float DotProduct = FVector::DotProduct(OppositeHeading, NewDirection);
	const float CosThreshold = FMath::Cos(FMath::DegreesToRadians(RecastConeHalfAngle));

	if (DotProduct < CosThreshold)
	{
		// New direction is too far from the opposite heading — recast rejected
		return false;
	}

	// Apply chain multipliers
	CurrentChain++;
	CurrentSpeed *= SpeedMultiplierPerChain;
	CurrentDamageMultiplier *= DamageMultiplierPerChain;
	CurrentScaleMultiplier *= ScaleMultiplierPerChain;

	// Update the projectile
	ActiveProjectile->CurrentVelocity = NewDirection * CurrentSpeed;
	ActiveProjectile->PullOrigin = AvatarActor->GetActorLocation();
	ActiveProjectile->bPullActive = true;
	ActiveProjectile->CurrentScaleMultiplier = CurrentScaleMultiplier;
	ActiveProjectile->SetActorScale3D(FVector(CurrentScaleMultiplier));

	// Update damage spec with the new multiplier
	ActiveProjectile->DamageEffectSpecHandle = MakeYoyoDamageSpec();

	// SFX
	if (RecastSound)
	{
		UGameplayStatics::PlaySoundAtLocation(AvatarActor, RecastSound, ActiveProjectile->GetActorLocation());
	}

	OnYoyoRecast(CurrentChain, NewDirection);

	// Restart the control task for the next potential recast
	StartYoyoControlTask();

	return true;
}

void URageInMageYoyoSpell::StartYoyoControlTask()
{
	if (!ActiveProjectile) return;

	URageInMageAbilityTask_YoyoControl* ControlTask = URageInMageAbilityTask_YoyoControl::CreateYoyoControlTask(
		this, ActiveProjectile, RecastRadius);

	if (ControlTask)
	{
		ControlTask->OnRecastTriggered.AddDynamic(this, &URageInMageYoyoSpell::HandleRecastTriggered);
		ControlTask->OnProjectileLost.AddDynamic(this, &URageInMageYoyoSpell::HandleProjectileLost);
		ControlTask->ReadyForActivation();
	}
}

void URageInMageYoyoSpell::HandleRecastTriggered(const FVector& AimPosition)
{
	if (CurrentChain < MaxChains)
	{
		RecastYoyo(AimPosition);
	}
}

void URageInMageYoyoSpell::HandleProjectileLost()
{
	OnYoyoImpact();
	ActiveProjectile = nullptr;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

FGameplayEffectSpecHandle URageInMageYoyoSpell::MakeYoyoDamageSpec() const
{
	const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC || !DamageEffectClass) return FGameplayEffectSpecHandle();

	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddInstigator(GetOwningActorFromActorInfo(), GetAvatarActorFromActorInfo());

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
	for (const auto& Pair : DamageTypeTags)
	{
		const float ScaledValue = Pair.Value.GetValueAtLevel(GetAbilityLevel()) * CurrentDamageMultiplier;
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledValue);
	}
	return SpecHandle;
}

FGameplayEffectSpecHandle URageInMageYoyoSpell::MakePuddleDamageSpec() const
{
	const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC) return FGameplayEffectSpecHandle();

	// Use puddle-specific GE/tags if set, otherwise fall back to main
	const TSubclassOf<UGameplayEffect> GEClass = PuddleDamageEffectClass ? PuddleDamageEffectClass : DamageEffectClass;
	if (!GEClass) return FGameplayEffectSpecHandle();

	const TMap<FGameplayTag, FScalableFloat>& Tags = PuddleDamageTypeTags.Num() > 0 ? PuddleDamageTypeTags : DamageTypeTags;

	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddInstigator(GetOwningActorFromActorInfo(), GetAvatarActorFromActorInfo());

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(GEClass, GetAbilityLevel(), EffectContextHandle);
	for (const auto& Pair : Tags)
	{
		const float ScaledValue = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledValue);
	}
	return SpecHandle;
}
