// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "RageInMageSphereProjectile.generated.h"


class ARageInMageFireZone;
class UNiagaraSystem;
class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class RAGEINMAGE_API ARageInMageSphereProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ARageInMageSphereProjectile();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

	/* AoE Explosion - if > 0, the projectile deals AoE damage on impact */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AoE")
	float AoERadius = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AoE")
	TObjectPtr<UNiagaraSystem> AoEExplosionEffect;

	/* Knockback - applied to hit targets on impact */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Knockback")
	float KnockbackStrength = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Knockback")
	float KnockbackUpwardForce = 0.3f;

	/* Fire Zone - spawned on impact if FireZoneClass is set (used by charged fireball, meteor, etc.) */
	UPROPERTY(BlueprintReadWrite, Category = "FireZone")
	TSubclassOf<ARageInMageFireZone> FireZoneClass;

	UPROPERTY(BlueprintReadWrite, Category = "FireZone")
	FGameplayEffectSpecHandle FireZoneDamageEffectSpecHandle;

	UPROPERTY(BlueprintReadWrite, Category = "FireZone")
	float FireZoneRadius = 0.f;

	UPROPERTY(BlueprintReadWrite, Category = "FireZone")
	float FireZoneDuration = 0.f;

	UPROPERTY(BlueprintReadWrite, Category = "FireZone")
	float FireZoneTickInterval = 1.f;

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void ApplyAoEDamage(const FVector& ImpactLocation);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Sphere;

private:

	float LifeSpan = 15.f;

	bool bHit = false;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound;

	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;
};