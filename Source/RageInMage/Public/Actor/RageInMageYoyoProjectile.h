// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "RageInMageYoyoProjectile.generated.h"

class ARageInMageZone;
class UNiagaraSystem;
class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnYoyoImpact);

/**
 * Yo-yo projectile with custom tick-based physics (no ProjectileMovementComponent).
 * Launched outward, pulled gravitationally toward PullOrigin. Once it returns within
 * OriginReturnRadius of PullOrigin, pull stops and it flies off in its current direction.
 * Damage + knockback + puddle only on overlap (impact with enemy or world).
 */
UCLASS()
class RAGEINMAGE_API ARageInMageYoyoProjectile : public AActor
{
	GENERATED_BODY()

public:
	ARageInMageYoyoProjectile();

	virtual void Tick(float DeltaTime) override;

	// ── Set by owning ability at spawn time ──

	/** GE spec for damage on impact. */
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

	/** Knockback force applied to the target on impact. */
	UPROPERTY(BlueprintReadWrite, Category = "Yoyo|Knockback")
	float KnockbackStrength = 0.f;

	UPROPERTY(BlueprintReadWrite, Category = "Yoyo|Knockback")
	float KnockbackUpwardForce = 0.3f;

	// ── Pull physics ──

	/** The point toward which the yo-yo is pulled. Set to caster position at launch/recast. */
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Yoyo|Pull")
	FVector PullOrigin = FVector::ZeroVector;

	/** Acceleration magnitude toward PullOrigin (units/sec^2). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Yoyo|Pull")
	float PullStrength = 800.f;

	/** Once within this radius of PullOrigin, pull deactivates and yo-yo flies off straight. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Yoyo|Pull")
	float OriginReturnRadius = 150.f;

	/** Whether gravitational pull toward PullOrigin is currently active. */
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Yoyo|Pull")
	bool bPullActive = true;

	/** Current velocity vector. Replicated so clients see smooth movement. */
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Yoyo|Movement")
	FVector CurrentVelocity = FVector::ZeroVector;

	/** Current visual scale multiplier (increases with each recast chain). */
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Yoyo|Scale")
	float CurrentScaleMultiplier = 1.f;

	// ── Puddle zone (spawned on final impact only) ──

	UPROPERTY(BlueprintReadWrite, Category = "Yoyo|Puddle")
	TSubclassOf<ARageInMageZone> PuddleZoneClass;

	UPROPERTY(BlueprintReadWrite, Category = "Yoyo|Puddle")
	FGameplayEffectSpecHandle PuddleDamageEffectSpecHandle;

	UPROPERTY(BlueprintReadWrite, Category = "Yoyo|Puddle")
	float PuddleZoneRadius = 200.f;

	UPROPERTY(BlueprintReadWrite, Category = "Yoyo|Puddle")
	float PuddleZoneDuration = 4.f;

	UPROPERTY(BlueprintReadWrite, Category = "Yoyo|Puddle")
	float PuddleZoneTickInterval = 1.f;

	UPROPERTY(BlueprintReadWrite, Category = "Yoyo|Puddle|VFX")
	TObjectPtr<UNiagaraSystem> PuddleZoneEffect;

	UPROPERTY(BlueprintReadWrite, Category = "Yoyo|Puddle|SFX")
	TObjectPtr<USoundBase> PuddleZoneLoopSound;

	// ── Impact VFX/SFX ──

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Yoyo|Impact")
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Yoyo|Impact")
	TObjectPtr<USoundBase> ImpactSound;

	/** Broadcast when the yo-yo hits something and is destroyed. Used by the ability task. */
	UPROPERTY(BlueprintAssignable, Category = "Yoyo|Events")
	FOnYoyoImpact OnYoyoImpact;

	/** Collision sphere — public so the ability can read its radius. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Sphere;

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	void SpawnPuddleZone(const FVector& Location);

	float LifeSpan = 15.f;
	bool bHit = false;

	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;

	UPROPERTY(EditAnywhere, Category = "Yoyo|SFX")
	TObjectPtr<USoundBase> LoopingSound;
};
