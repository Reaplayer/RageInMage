// Copyright Reaplays


#include "AbilitySystem/Abilities/RageInMageProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "RageInMageGameplayTag.h"
#include "Actor/RageInMageSphereProjectile.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


void URageInMageProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                 const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void URageInMageProjectileSpell::StartTrackingTarget(AActor* TargetToTrack)
{
	TrackedActor = TargetToTrack;
	TargetHistory.Empty();

	if (TrackedActor)
	{
		// Record the first sample immediately
		RecordTargetSample();
		
		// Start a timer to record samples at 30Hz (every 0.033s)
		GetWorld()->GetTimerManager().SetTimer(TrackingTimerHandle, this, &URageInMageProjectileSpell::RecordTargetSample, 0.033f, true);
	}
}

void URageInMageProjectileSpell::RecordTargetSample()
{
	if (TrackedActor)
	{
		FMotionSample NewSample;
		NewSample.Location = TrackedActor->GetActorLocation();
		NewSample.Time = GetWorld()->GetTimeSeconds();
		
		TargetHistory.Add(NewSample);

		// Keep history short (e.g. max 10 samples / 0.3 seconds is plenty for acceleration)
		if (TargetHistory.Num() > 10)
		{
			TargetHistory.RemoveAt(0);
		}
	}
}

void URageInMageProjectileSpell::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	// Clean up timer when ability ends
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TrackingTimerHandle);
	}
	TargetHistory.Empty();
	TrackedActor = nullptr;
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URageInMageProjectileSpell::SpawnProjectile(FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, TSubclassOf<AActor> ProjectileClass, bool bCalculatePitch, AActor* LeadingTarget)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

	if (ProjectileClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ProjectileClass is null. Unable to spawn projectile."));
		return;
	}
	ARageInMageSphereProjectile* ProjectileCDO = Cast<ARageInMageSphereProjectile>(ProjectileClass->GetDefaultObject());
	UProjectileMovementComponent* ProjectileMovement = ProjectileCDO->ProjectileMovement;
	

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
	{
		// Set where to spawn the Projectile from
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(),SocketTag);
		FVector FinalTargetLocation = ProjectileTargetLocation;
		// --- PREDICTION LOGIC START ---
		if (LeadingTarget)
		{
			// 1. PREPARE DATA
			const FVector TargetVelocity = LeadingTarget->GetVelocity();
			const FVector TargetLocation = LeadingTarget->GetActorLocation();
			
			// Calculate Acceleration if we have tracking data for THIS target
			FVector TargetAcceleration = FVector::ZeroVector;
			
			if (LeadingTarget == TrackedActor && TargetHistory.Num() >= 3)
			{
				// Calculate velocity at start of history vs end of history
				const FMotionSample& OldSample = TargetHistory[0];
				const FMotionSample& NewSample = TargetHistory.Last();
				
				if (NewSample.Time > OldSample.Time)
				{
					// Average velocity over the window
					// Note: Real acceleration is (V_final - V_initial) / t
					// But since we only have positions, we can infer curve.
					// Simplified: Let's assume the current Velocity is V_final.
					// We calculate V_avg from history.
					// Accel approx = (CurrentVelocity - AvgVelocity) / (Time / 2)
					
					FVector AverageVelocity = (NewSample.Location - OldSample.Location) / (NewSample.Time - OldSample.Time);
					float TimeDelta = NewSample.Time - OldSample.Time;
					
					if (TimeDelta > KINDA_SMALL_NUMBER)
					{
						TargetAcceleration = (TargetVelocity - AverageVelocity) / (TimeDelta * 0.5f);
					}
				}
			}
			
			// Clamp acceleration to avoid wild predictions (e.g. instantaneous teleports)
			// 2000 units/s^2 is reasonable for character movement changes
			TargetAcceleration = TargetAcceleration.GetClampedToMaxSize(2000.f);


			// 2. SOLVER
			float ProjectileSpeed = 0.f;
			if (ProjectileCDO)
			{
				if (ProjectileMovement)
				{
					ProjectileSpeed = ProjectileMovement->InitialSpeed;
				}
			}

			if (ProjectileSpeed > 0.f)
			{
				// Iterative Approximation for solving t with Acceleration
				// Equation: Find t where Dist(TargetPos(t), Source) = Speed * t
				
				float T = 0.f;
				
				// Initial Guess (Distance / Speed)
				float Dist = (TargetLocation - SocketLocation).Size();
				T = Dist / ProjectileSpeed;
				
				// Refine guess 3 times (usually sufficient)
				for (int i = 0; i < 3; i++)
				{
					// Predict where target is at Time T using: P + Vt + 0.5at^2
					FVector FuturePos = TargetLocation + (TargetVelocity * T) + (0.5f * TargetAcceleration * T * T);
					
					// Recalculate time to reach that new position
					float NewDist = (FuturePos - SocketLocation).Size();
					float NewT = NewDist / ProjectileSpeed;
					
					// Average to smooth convergence
					T = (T + NewT) * 0.5f;
				}

				// Safety Clamp (don't predict more than 1.5 seconds ahead)
				T = FMath::Min(T, 1.5f);

				// Final Calculation
				if (T > 0.f)
				{
					FinalTargetLocation = TargetLocation + (TargetVelocity * T) + (0.5f * TargetAcceleration * T * T);
				}
			}
		}
		// --- PREDICTION LOGIC END ---
		
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		if (bCalculatePitch)
		{
			if (LeadingTarget)
			{
				ProjectileTargetLocation = FinalTargetLocation;
			}
			// Calculate the launch pitch based on projectile speed and gravity
			if (ProjectileCDO)
			{
				if (ProjectileMovement)
				{
					const float LaunchSpeed = ProjectileMovement->InitialSpeed;
					FVector LaunchVelocity;
				
					// 1. Calculate Effective Gravity (World Gravity * Component Scale)
					// We need absolute value for the range calculation formula
					const float WorldGravityZ = GetWorld()->GetGravityZ();
					const float EffectiveGravityZ = FMath::Abs(WorldGravityZ * ProjectileMovement->ProjectileGravityScale);
					// No Gravity means we can shoot straight
					float CalculatedArc = 1.0f;
					
					if (EffectiveGravityZ > KINDA_SMALL_NUMBER)
					{
						const float MaxRange = (LaunchSpeed * LaunchSpeed) / EffectiveGravityZ;
						const float DistanceToTarget = FVector::Dist(SocketLocation, ProjectileTargetLocation);
						
						// 2. Map Range: 
						// Distance 0% of Max -> Arc 1.0 (Direct/Flat)
						// Distance 100% of Max -> Arc 0.5 (High/Optimal)
						CalculatedArc = UKismetMathLibrary::MapRangeClamped(DistanceToTarget, 0.0f, MaxRange, 1.0f, 0.5f);
					}
					
					
					// 3. Calculate the velocity required to hit the target
					const bool bHaveSolution = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
						this,
						LaunchVelocity,
						SocketLocation,
						ProjectileTargetLocation,
						0,
						CalculatedArc
					);

					if (bHaveSolution)
					{
						Rotation = LaunchVelocity.Rotation();
					}
				}
			}
		}
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		if (ProjectileMovement->ProjectileGravityScale < KINDA_SMALL_NUMBER)
		{
			Rotation.Pitch = 0.f;
			SpawnTransform.SetRotation(Rotation.Quaternion());
		}
		else
		{
			SpawnTransform.SetRotation(Rotation.Quaternion());
		}
		
		

		// Start the Spawning of the Projectile
		ARageInMageSphereProjectile* Projectile = GetWorld()->SpawnActorDeferred<ARageInMageSphereProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(CurrentActorInfo->AvatarActor),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		// Create and fill the Effect Context Handle
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddInstigator(GetOwningActorFromActorInfo(), GetAvatarActorFromActorInfo());
		EffectContextHandle.AddSourceObject(Projectile);
		TArray<TWeakObjectPtr<AActor>> Actors;
		EffectContextHandle.AddActors(Actors);
		FHitResult HitResult;
		HitResult.Location = ProjectileTargetLocation;
		EffectContextHandle.AddHitResult(HitResult);

		// Make the Spec Handle
		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

		// Assign Damage Spec Handle && Spawn
		for (auto& Pair : DamageTypeTags)
		{
			FGameplayTag GameplayTag = Pair.Key;
			const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
		}
		Projectile->DamageEffectSpecHandle = SpecHandle;
		Projectile->FinishSpawning(SpawnTransform);
	}
}

/* --- PREDICTION LOGIC START ---
 * Simple Straight line prediction
		if (LeadingTarget)
		{
			const FVector TargetVelocity = LeadingTarget->GetVelocity();
			const FVector TargetLocation = LeadingTarget->GetActorLocation();
			const FVector ToTarget = TargetLocation - SocketLocation;

			// Get Projectile Speed from CDO
			float ProjectileSpeed = 0.f;
			if (ARageInMageSphereProjectile* ProjectileCDO = Cast<ARageInMageSphereProjectile>(ProjectileClass->GetDefaultObject()))
			{
				if (UProjectileMovementComponent* ProjectileMovement = ProjectileCDO->ProjectileMovement)
				{
					ProjectileSpeed = ProjectileMovement->InitialSpeed;
				}
			}

			// Solve quadratic equation: At^2 + Bt + C = 0
			// A = V^2 - S^2
			// B = 2 * (D . V)
			// C = D . D
			if (ProjectileSpeed > 0.f)
			{
				const float A = FVector::DotProduct(TargetVelocity, TargetVelocity) - (ProjectileSpeed * ProjectileSpeed);
				const float B = 2.f * FVector::DotProduct(TargetVelocity, ToTarget);
				const float C = FVector::DotProduct(ToTarget, ToTarget);

				float TimeToImpact = 0.f;

				// If A is near zero, the target speed is almost equal to projectile speed (Linear case)
				if (FMath::IsNearlyZero(A))
				{
					if (!FMath::IsNearlyZero(B))
					{
						float T = -C / B;
						if (T > 0.f)
						{
							TimeToImpact = T;
						}
					}
				}
				else
				{
					// Quadratic case: Calculate Determinant
					const float Delta = (B * B) - (4.f * A * C);

					if (Delta >= 0.f) // If Delta < 0, interception is impossible (target too fast)
					{
						const float SqrtDelta = FMath::Sqrt(Delta);
						const float T1 = (-B - SqrtDelta) / (2.f * A);
						const float T2 = (-B + SqrtDelta) / (2.f * A);

						// We want the smallest positive time
						if (T1 > 0.f && T2 > 0.f)
						{
							TimeToImpact = FMath::Min(T1, T2);
						}
						else if (T1 > 0.f)
						{
							TimeToImpact = T1;
						}
						else if (T2 > 0.f)
						{
							TimeToImpact = T2;
						}
					}
				}

				// Apply the precise time calculation
				if (TimeToImpact > 0.f)
				{
					FinalTargetLocation = TargetLocation + (TargetVelocity * TimeToImpact);
				}
			}
		}
		// --- PREDICTION LOGIC END ---
*/