// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageYoyoSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "RageInMageGameplayTag.h"
#include "AbilitySystem/AbilityTasks/RageInMageAbilityTask_YoyoControl.h"
#include "Actor/RageInMageYoyoProjectile.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"


URageInMageYoyoSpell::URageInMageYoyoSpell()
{
	ActivationOwnedTags.AddTag(FRageInMageGameplayTag::Get().Status_Channeling);
}

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

		// Pull physics — pull toward the spawn (socket) location, not the character center
		ActiveProjectile->PullOrigin = SocketLocation;
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
	if (!ActiveProjectile)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, TEXT("Recast BLOCKED — No projectile"));
		}
		return false;
	}

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

	// The recast direction should be within RecastConeHalfAngle of the CURRENT heading (boost, not redirect)
	const float DotProduct = FVector::DotProduct(CurrentHeading, NewDirection);
	const float CosThreshold = FMath::Cos(FMath::DegreesToRadians(RecastConeHalfAngle));

	if (DotProduct < CosThreshold)
	{
		// New direction is too far from current heading — recast rejected
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
				FString::Printf(TEXT("Recast FAILED! Dot: %.2f (need >= %.2f)"), DotProduct, CosThreshold));
		}
		return false;
	}

	// Apply chain multipliers only if below max chains
	const bool bCanMultiply = CurrentChain < MaxChains;
	if (bCanMultiply)
	{
		CurrentChain++;
		CurrentSpeed *= SpeedMultiplierPerChain;
		CurrentDamageMultiplier *= DamageMultiplierPerChain;
		CurrentScaleMultiplier *= ScaleMultiplierPerChain;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green,
			FString::Printf(TEXT("Recast SUCCESS! Chain: %d | Dot: %.2f | Speed: %.0f%s"),
				CurrentChain, DotProduct, CurrentSpeed,
				bCanMultiply ? TEXT("") : TEXT(" (MAX — no multiplier)")));
	}

	// Update the projectile — pull back toward the caster's current position
	ActiveProjectile->CurrentVelocity = NewDirection * CurrentSpeed;
	ActiveProjectile->PullOrigin = AvatarActor->GetActorLocation();
	ActiveProjectile->bPullActive = true;
	ActiveProjectile->bHasLeftOrigin = false;
	ActiveProjectile->CurrentScaleMultiplier = CurrentScaleMultiplier;
	ActiveProjectile->SetActorScale3D(FVector(CurrentScaleMultiplier));
	ActiveProjectile->SetLifeSpan(15.f);

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

// ════════════════════════════════════════════════════════════════════════════════
// Aim Indicator
// ════════════════════════════════════════════════════════════════════════════════

void URageInMageYoyoSpell::BeginAimMode()
{
	bIsAiming = true;
	OnAimBegin();
}

void URageInMageYoyoSpell::EndAimMode()
{
	if (bIsAiming)
	{
		bIsAiming = false;
		OnAimEnd();
	}
}

// ════════════════════════════════════════════════════════════════════════════════
// Ability Lifecycle
// ════════════════════════════════════════════════════════════════════════════════

void URageInMageYoyoSpell::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	EndAimMode();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// ════════════════════════════════════════════════════════════════════════════════
// Damage Specs
// ════════════════════════════════════════════════════════════════════════════════

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
