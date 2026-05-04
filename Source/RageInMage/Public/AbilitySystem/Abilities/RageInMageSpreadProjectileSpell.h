// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageProjectileSpell.h"
#include "RageInMageSpreadProjectileSpell.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

/**
 * Spread projectile spell — fires multiple projectiles in a cone pattern.
 * Each projectile is a standard ARageInMageProjectile spawned via the parent's SpawnProjectile().
 * Used for Simple Ice (Water Primary), and reusable for ThornSpread (Nature), etc.
 *
 * Blueprint workflow:
 * 1. ActivateAbility -> CommitAbility
 * 2. AimMode task -> OnInputReleased (with spread aim indicators)
 * 3. SpawnSpreadProjectiles(TargetLocation, SocketTag, ProjectileClass)
 * 4. EndAbility
 */
UCLASS()
class RAGEINMAGE_API URageInMageSpreadProjectileSpell : public URageInMageProjectileSpell
{
	GENERATED_BODY()

public:
	/**
	 * Spawn multiple projectiles in a cone pattern toward the target location.
	 * Each projectile gets its own damage spec via the parent's SpawnProjectile().
	 */
	UFUNCTION(BlueprintCallable, Category = "SpreadProjectile")
	void SpawnSpreadProjectiles(const FVector& TargetLocation, const FGameplayTag& SocketTag,
		TSubclassOf<AActor> ProjectileClass, bool bCalculatePitch = false);

	// ── Spread Aim Indicators ──

	/** Spawn N aim line Niagara instances — one per projectile in the spread.
	 *  Call once when aim mode begins. Each instance receives AimWidth/AimLength/AimColour params. */
	UFUNCTION(BlueprintCallable, Category = "SpreadProjectile|AimIndicator")
	void SpawnSpreadIndicators(UNiagaraSystem* AimLineSystem, FLinearColor AimColour);

	/** Update all spread indicator positions and rotations to face the current aim target.
	 *  Call every AimMode tick after SpawnSpreadIndicators. */
	UFUNCTION(BlueprintCallable, Category = "SpreadProjectile|AimIndicator")
	void UpdateSpreadIndicators(const FVector& AimPosition);

	/** Deactivate and destroy all spread indicator Niagara components. */
	UFUNCTION(BlueprintCallable, Category = "SpreadProjectile|AimIndicator")
	void DestroySpreadIndicators();

	/** Returns true if spread indicators are currently active. */
	UFUNCTION(BlueprintPure, Category = "SpreadProjectile|AimIndicator")
	bool HasActiveSpreadIndicators() const { return SpreadIndicatorComponents.Num() > 0; }

protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;

	/** Number of projectiles to fire per salvo. Scales with ability level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpreadProjectile")
	FScalableFloat ProjectileCount;

	/** Half-angle of the cone in degrees. 15 = 30-degree total spread. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpreadProjectile")
	float ConeHalfAngle = 15.f;

private:
	UPROPERTY()
	TArray<TObjectPtr<UNiagaraComponent>> SpreadIndicatorComponents;
};
