// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "RageInMageProjectile.generated.h"


class ARageInMageDecal;
class ARageInMageZone;
class UNiagaraSystem;
class UProjectileMovementComponent;

/**
 * Base class for all projectiles in RageInMage.
 * Owns all shared logic: damage, AoE, knockback, zones, impact VFX/SFX, decals, lifespan, looping sound.
 * Children provide only their collision shape by overriding GetCollisionComponent().
 */
UCLASS(Abstract)
class RAGEINMAGE_API ARageInMageProjectile : public AActor
{
	GENERATED_BODY()

public:
	ARageInMageProjectile();

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

	/* Zone - spawned on impact if ZoneClass is set (used by charged fireball, meteor, etc.) */
	UPROPERTY(BlueprintReadWrite, Category = "Zone")
	TSubclassOf<ARageInMageZone> ZoneClass;

	UPROPERTY(BlueprintReadWrite, Category = "Zone")
	FGameplayEffectSpecHandle ZoneDamageEffectSpecHandle;

	UPROPERTY(BlueprintReadWrite, Category = "Zone")
	float ZoneRadius = 0.f;

	UPROPERTY(BlueprintReadWrite, Category = "Zone")
	float ZoneDuration = 0.f;

	UPROPERTY(BlueprintReadWrite, Category = "Zone")
	float ZoneTickInterval = 1.f;

	UPROPERTY(BlueprintReadWrite, Category = "Zone|VFX")
	TObjectPtr<UNiagaraSystem> ZoneEffect;

	UPROPERTY(BlueprintReadWrite, Category = "Zone|SFX")
	TObjectPtr<USoundBase> ZoneLoopSound;

	/** Single-target impact VFX. Set on BP or overridden by ability at spawn time. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact")
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	/** Single-target impact sound. Set on BP or overridden by ability at spawn time. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact")
	TObjectPtr<USoundBase> ImpactSound;

	/** Charge percent at the time this projectile was fired (0-1). Used to scale VFX. */
	UPROPERTY(BlueprintReadWrite, Category = "Charge")
	float ChargePercent = 1.f;

	// -- Impact Decal (scorch mark) --

	/** Decal actor class to spawn on impact. If set, uses ARageInMageDecal with scale-in animation.
	 *  If null, falls back to raw SpawnDecalAtLocation (no scale-in). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact|Decal")
	TSubclassOf<ARageInMageDecal> ImpactDecalClass;

	/** Decal material to spawn at the ground impact point (e.g. burn/scorch mark). If null, no decal is spawned. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact|Decal")
	TObjectPtr<UMaterialInterface> ImpactDecalMaterial;

	/** Size of the impact decal in each axis (X=forward, Y=right, Z=projection depth). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact|Decal")
	FVector ImpactDecalSize = FVector(128.f, 128.f, 128.f);

	/** Duration of the scale-in animation on the decal (only used with ImpactDecalClass). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact|Decal")
	float ImpactDecalScaleInDuration = 0.5f;

	/** Seconds to wait before the decal begins fading. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact|Decal")
	float ImpactDecalFadeDelay = 3.f;

	/** Duration of the fade-out after the delay. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact|Decal")
	float ImpactDecalFadeDuration = 2.f;

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	/** Children must return their collision primitive (sphere, box, etc.). */
	virtual UPrimitiveComponent* GetCollisionComponent() const PURE_VIRTUAL(ARageInMageProjectile::GetCollisionComponent, return nullptr;);

	UFUNCTION()
	void OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void ApplyAoEDamage(const FVector& ImpactLocation);

private:

	float LifeSpan = 15.f;

	bool bHit = false;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound;

	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;
};
