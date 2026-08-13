// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "RageInMageBoulder.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UNiagaraSystem;
class ACharacter;

/**
 * The Earth mage's Ultimate — a slow boulder the caster RIDES on top of. It rolls forward on its
 * own and can be steered, but never stops. The longer it rolls the BIGGER and more damaging it
 * gets. It ends when it hits something or when its time runs out, and the rider can bail early —
 * after which it keeps rolling on its own until one of those two things happens.
 *
 * SHAPE: a sphere collision IS the boulder. Everything derives from one number, CurrentRadius:
 *   - the sphere's radius,
 *   - the mesh's scale (cosmetic only — the mesh never blocks),
 *   - the rider's attach height (RiderAttachPoint sits at +CurrentRadius, the top of the sphere),
 *   - the actor's height off the ground (centre Z = ground + CurrentRadius).
 * That last one is why growth doesn't sink the boulder into the floor: as the radius grows the
 * centre RISES by exactly as much, so the sphere always rests tangent to the ground.
 *
 * The rider hangs off RiderAttachPoint, which is a child of the sphere but is NOT scaled and does
 * NOT roll — only the mesh spins. Growth is applied via SetSphereRadius + the mesh's own relative
 * scale rather than by scaling the actor, precisely so the rider doesn't grow along with it.
 *
 * Kinematically driven (no physics sim): it sets its own location each tick and probes ahead for
 * walls, so it can't be shoved off course by the things it runs over.
 */
UCLASS()
class RAGEINMAGE_API ARageInMageBoulder : public AActor
{
	GENERATED_BODY()

public:
	ARageInMageBoulder();

	virtual void Tick(float DeltaSeconds) override;

	// ── Riding ──

	/** Attach a character to the top of the boulder and take over its movement. */
	UFUNCTION(BlueprintCallable, Category = "Boulder|Riding")
	void Mount(ACharacter* Rider);

	/** Detach the current rider and hand movement back. The boulder keeps rolling. */
	UFUNCTION(BlueprintCallable, Category = "Boulder|Riding")
	void Dismount();

	UFUNCTION(BlueprintPure, Category = "Boulder|Riding")
	ACharacter* GetRider() const { return CurrentRider; }

	UFUNCTION(BlueprintPure, Category = "Boulder|Riding")
	bool HasRider() const { return CurrentRider != nullptr; }

	/** Steering input, -1 (left) to +1 (right). Fed from the PlayerController while riding. */
	UFUNCTION(BlueprintCallable, Category = "Boulder|Riding")
	void AddSteerInput(float Value);

	// ── State the ability fills in at spawn ──

	/** Damage spec applied to whatever the boulder runs into. Scaled by growth on impact. */
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

	/** Base damage per type, re-applied to the spec (scaled by growth) at impact time. */
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	TMap<FGameplayTag, float> BaseDamageByType;

	/**
	 * Seconds of rolling to reach full size. The ability multiplies this by the Immovable Mass
	 * stance scalar BEFORE spawn (stance cuts grow-time, so a higher stance grows faster).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boulder|Growth", meta = (ExposeOnSpawn = true))
	float GrowthDuration = 6.f;

	/** How long the boulder lasts before crumbling, rider or not. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boulder", meta = (ExposeOnSpawn = true))
	float MaxLifetime = 12.f;

	// ── Tunables ──

	/** Radius the boulder starts at. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boulder|Growth")
	float StartRadius = 90.f;

	/** Radius at full growth. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boulder|Growth")
	float MaxRadius = 220.f;

	/** Damage multiplier at full growth (1 = no growth bonus). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boulder|Growth")
	float MaxDamageMultiplier = 3.f;

	/** Forward roll speed. "Slow" by design — it is a rolling threat, not a dash. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boulder|Movement")
	float RollSpeed = 450.f;

	/** Degrees per second of steering at full stick deflection. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boulder|Movement")
	float SteerRate = 90.f;

	/** Knockback applied to whatever it runs over. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boulder|Movement")
	float ImpactKnockback = 1200.f;

	/** Upward share of that knockback (0-1). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boulder|Movement")
	float ImpactKnockbackUpward = 0.4f;

	/** End the roll when it runs into a character (per the design). False = plough on through. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boulder|Movement")
	bool bEndOnCharacterHit = true;

	/** Upward hop given to the rider when they bail, so they clear the boulder. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boulder|Riding")
	float DismountHopStrength = 400.f;

	/** VFX on the boulder's final impact / crumble. */
	UPROPERTY(EditAnywhere, Category = "Boulder|VFX")
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditAnywhere, Category = "Boulder|SFX")
	TObjectPtr<USoundBase> ImpactSound;

	/** The boulder body. Its radius is the single source of truth for the boulder's size. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boulder")
	TObjectPtr<USphereComponent> BoulderCollision;

	/** Cosmetic rock. Scaled to match the sphere and spun to fake rolling; never collides. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boulder")
	TObjectPtr<UStaticMeshComponent> BoulderMesh;

	/** Where the rider sits — the top of the sphere. Never scaled, never rolls. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boulder")
	TObjectPtr<USceneComponent> RiderAttachPoint;

	/** Radius the boulder is at right now. */
	UFUNCTION(BlueprintPure, Category = "Boulder|Growth")
	float GetCurrentRadius() const { return CurrentRadius; }

	/** Growth progress, 0-1. */
	UFUNCTION(BlueprintPure, Category = "Boulder|Growth")
	float GetGrowthAlpha() const;

protected:
	virtual void BeginPlay() override;

private:
	/** Recompute radius/damage from time rolled and push it into the sphere, mesh and rider point. */
	void UpdateGrowth(float DeltaSeconds);

	/** Roll forward, steer, and keep the sphere resting on the ground. */
	void UpdateMovement(float DeltaSeconds);

	/** Look ahead for a wall, and sweep for characters to run over. */
	void CheckForImpacts();

	/** Damage + knock back a character the boulder has run into. */
	void HitCharacter(AActor* Target);

	/** Final impact: VFX, drop the rider, destroy. */
	void EndRoll();

	UPROPERTY()
	TObjectPtr<ACharacter> CurrentRider;

	/** Everyone already run over, so a target can't be hit twice by one roll. */
	UPROPERTY()
	TArray<TObjectPtr<AActor>> AlreadyHit;

	float CurrentRadius = 90.f;
	float TimeRolled = 0.f;
	float CurrentDamageMultiplier = 1.f;
	float PendingSteer = 0.f;
	float MeshRollAngle = 0.f;
	bool bRolling = true;

	/** Mesh radius at scale 1, derived from its bounds so any rock mesh fits the sphere. */
	float MeshBaseRadius = 0.f;
};
