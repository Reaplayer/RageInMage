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

void URageInMageProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, TSubclassOf<AActor> ProjectileClass, bool bCalculatePitch)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

	if (ProjectileClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ProjectileClass is null. Unable to spawn projectile."));
		return;
	}

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
	{
		// Set where to spawn the Projectile from
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
			GetAvatarActorFromActorInfo(),
			SocketTag);
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		if (bCalculatePitch)
		{
			// Calculate the launch pitch based on projectile speed and gravity
			if (ARageInMageSphereProjectile* ProjectileCDO = Cast<ARageInMageSphereProjectile>(ProjectileClass->GetDefaultObject()))
			{
				if (UProjectileMovementComponent* ProjectileMovement = ProjectileCDO->ProjectileMovement)
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
		SpawnTransform.SetRotation(Rotation.Quaternion());

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
