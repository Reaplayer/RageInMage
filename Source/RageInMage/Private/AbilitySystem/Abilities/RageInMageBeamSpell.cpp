// Copyright Reaplays

#include "AbilitySystem/Abilities/RageInMageBeamSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "RageInMageGameplayTag.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Actor/RageInMageFireWall.h"
#include "Components/AudioComponent.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


URageInMageBeamSpell::URageInMageBeamSpell()
{
	ActivationOwnedTags.AddTag(FRageInMageGameplayTag::Get().Status_Channeling);
}

void URageInMageBeamSpell::StartBeamChannel(const FVector& TargetLocation, const FGameplayTag& SocketTag)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;

	CurrentTargetLocation = TargetLocation;
	CurrentSocketTag = SocketTag;
	bIsChanneling = true;

	SpawnBeamVFX();

	// Tick damage immediately, then on interval
	BeamTickDamage();
	GetWorld()->GetTimerManager().SetTimer(
		BeamTickTimerHandle, this, &URageInMageBeamSpell::BeamTickDamage, BeamTickInterval, true);
}

void URageInMageBeamSpell::StopBeamChannel()
{
	bIsChanneling = false;
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(BeamTickTimerHandle);
	}
	DestroyBeamVFX();
}

void URageInMageBeamSpell::UpdateBeamTarget(const FVector& NewTargetLocation)
{
	CurrentTargetLocation = NewTargetLocation;
	UpdateBeamVFX();
}

void URageInMageBeamSpell::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	StopBeamChannel();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// ──────────────────────────────────────────
// VFX
// ──────────────────────────────────────────

void URageInMageBeamSpell::SpawnBeamVFX()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return;

	if (BeamVFX)
	{
		const FVector BeamStart = ICombatInterface::Execute_GetCombatSocketLocation(AvatarActor, CurrentSocketTag);

		BeamVFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			AvatarActor, BeamVFX, BeamStart, FRotator::ZeroRotator,
			FVector::OneVector, true, true, ENCPoolMethod::None);

		if (BeamVFXComponent)
		{
			// Initialize beam at full range toward current aim direction
			FVector BeamDirection = CurrentTargetLocation - BeamStart;
			BeamDirection.Z = 0.f;
			BeamDirection = BeamDirection.GetSafeNormal();
			const FVector BeamEnd = BeamStart + BeamDirection * BeamRange;

			bBeamBlocked = false;
			BlockedBeamDistance = BeamRange;

			const float BeamTravelTime = (BeamSpeed > 0.f) ? (BeamRange / BeamSpeed) : 1.f;

			BeamVFXComponent->SetVariablePosition(FName("BeamStart"), BeamStart);
			BeamVFXComponent->SetVariablePosition(FName("BeamEnd"), BeamEnd);
			BeamVFXComponent->SetVariableFloat(FName("BeamTravelTime"), BeamTravelTime);
			BeamVFXComponent->SetVariableBool(FName("BeamKill"), false);
			BeamVFXComponent->SetVariableBool(FName("SmokeKill"), false);
		}
	}

	if (BeamLoopSound)
	{
		BeamLoopSoundComponent = UGameplayStatics::SpawnSoundAttached(
			BeamLoopSound, AvatarActor->GetRootComponent());
	}
}

void URageInMageBeamSpell::UpdateBeamVFX()
{
	if (!BeamVFXComponent) return;

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return;

	const FVector BeamStart = ICombatInterface::Execute_GetCombatSocketLocation(AvatarActor, CurrentSocketTag);

	// Always recalculate beam direction from current aim target
	FVector BeamDirection = CurrentTargetLocation - BeamStart;
	BeamDirection.Z = 0.f;
	BeamDirection = BeamDirection.GetSafeNormal();

	// Use blocked distance if damage tick found a non-ignited target, otherwise full range
	const float EffectiveDistance = bBeamBlocked ? BlockedBeamDistance : BeamRange;
	const FVector BeamEnd = BeamStart + BeamDirection * EffectiveDistance;

	// Calculate travel time based on distance and speed
	const float BeamTravelTime = (BeamSpeed > 0.f) ? (EffectiveDistance / BeamSpeed) : 1.f;

	BeamVFXComponent->SetVariablePosition(FName("BeamStart"), BeamStart);
	BeamVFXComponent->SetVariablePosition(FName("BeamEnd"), BeamEnd);
	BeamVFXComponent->SetVariableFloat(FName("BeamTravelTime"), BeamTravelTime);
}

void URageInMageBeamSpell::DestroyBeamVFX()
{
	if (BeamVFXComponent)
	{
		BeamVFXComponent->SetVariableBool(FName("BeamKill"), true);

		// Capture weak refs — the ability will be GC'd after EndAbility, but the
		// Niagara component lives in the world and survives independently.
		TWeakObjectPtr<UNiagaraComponent> WeakVFX = BeamVFXComponent;
		TWeakObjectPtr<UWorld> WeakWorld = GetWorld();

		// Release our reference — the lambda owns the weak ref now
		BeamVFXComponent = nullptr;

		// Start linger timer — smoke stays alive until timer fires SmokeKill
		if (WeakWorld.IsValid())
		{
			FTimerHandle LingerHandle;
			WeakWorld->GetTimerManager().SetTimer(
				LingerHandle, FTimerDelegate::CreateLambda([WeakVFX, WeakWorld]()
				{
					if (WeakVFX.IsValid())
					{
						UE_LOG(LogTemp, Warning, TEXT("Setting SmokeKill to TRUE on %s"), *WeakVFX->GetName());
						WeakVFX->SetVariableBool(FName("SmokeKill"), true);

						// Short delay so Niagara processes SmokeKill before we destroy
						if (WeakWorld.IsValid())
						{
							FTimerHandle DestroyHandle;
							WeakWorld->GetTimerManager().SetTimer(
								DestroyHandle, FTimerDelegate::CreateLambda([WeakVFX]()
								{
									if (WeakVFX.IsValid())
									{
										WeakVFX->DestroyComponent();
									}
								}),
								0.3f, false);
						}
					}
				}),
				SmokeLingerDuration, false);
		}
	}

	if (BeamLoopSoundComponent)
	{
		BeamLoopSoundComponent->Stop();
		BeamLoopSoundComponent = nullptr;
	}
}

// ──────────────────────────────────────────
// Damage
// ──────────────────────────────────────────

void URageInMageBeamSpell::BeamTickDamage()
{
	if (!bIsChanneling) return;

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority()) return;

	// Get beam start location from combat socket
	const FVector BeamStart = ICombatInterface::Execute_GetCombatSocketLocation(AvatarActor, CurrentSocketTag);
	// Flatten direction to ignore Z axis (top-down beam stays horizontal)
	FVector BeamDirection = CurrentTargetLocation - BeamStart;
	BeamDirection.Z = 0.f;
	BeamDirection = BeamDirection.GetSafeNormal();
	const FVector BeamEnd = BeamStart + BeamDirection * BeamRange;

#if ENABLE_DRAW_DEBUG
	// Debug: draw the beam line (orange) and sweep radius spheres at start/end
	DrawDebugLine(GetWorld(), BeamStart, BeamEnd, FColor::Orange, false, BeamTickInterval + 0.05f, 0, 2.f);
	DrawDebugSphere(GetWorld(), BeamStart, BeamRadius, 8, FColor::Yellow, false, BeamTickInterval + 0.05f);
	DrawDebugSphere(GetWorld(), BeamEnd, BeamRadius, 8, FColor::Yellow, false, BeamTickInterval + 0.05f);
	DrawDebugSphere(GetWorld(), CurrentTargetLocation, 15.f, 6, FColor::Cyan, false, BeamTickInterval + 0.05f);
#endif

	// Create the damage spec
	const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddInstigator(GetOwningActorFromActorInfo(), AvatarActor);
	const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
	for (auto& Pair : DamageTypeTags)
	{
		const float ScaledValue = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledValue);
	}

	// Sphere trace along the beam
	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);
	QueryParams.AddIgnoredActor(GetOwningActorFromActorInfo());

	GetWorld()->SweepMultiByChannel(
		HitResults,
		BeamStart,
		BeamEnd,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(BeamRadius),
		QueryParams);

	// Default: beam goes full range
	bool bBlockedThisTick = false;
	TSet<AActor*> AlreadyHit;

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || AlreadyHit.Contains(HitActor)) continue;
		if (Cast<ARageInMageFireWall>(HitActor)) continue;
		if (URageInMageAbilitySystemLibrary::IsFriendly(AvatarActor, HitActor)) continue;

		AlreadyHit.Add(HitActor);

#if ENABLE_DRAW_DEBUG
		DrawDebugSphere(GetWorld(), HitActor->GetActorLocation(), 40.f, 8, FColor::Green, false, BeamTickInterval + 0.05f);
#endif

		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor))
		{
			// Always damage the target we hit
			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

			// Check if THIS target is Ignited — if not, beam stops here
			const bool bTargetIgnited = IgnitedTag.IsValid() && TargetASC->HasMatchingGameplayTag(IgnitedTag);
			if (!bTargetIgnited)
			{
				// Beam is blocked by this non-ignited target — store distance for VFX
				BlockedBeamDistance = FVector::Dist(BeamStart, HitActor->GetActorLocation());
				bBlockedThisTick = true;
				break;
			}
			// Target is ignited — beam pierces through, continue to next target
		}
	}

	// Update blocked state for VFX
	bBeamBlocked = bBlockedThisTick;
}
