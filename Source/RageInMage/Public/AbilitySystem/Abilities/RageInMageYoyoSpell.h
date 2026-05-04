// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RageInMageDamageGameplayAbility.h"
#include "RageInMageYoyoSpell.generated.h"

class ARageInMageYoyoProjectile;
class ARageInMageZone;
class URageInMageAbilityTask_YoyoControl;

/**
 * Yo-yo spell — launches a projectile that curves back toward the cast origin via gravitational pull.
 * Can be recast in the opposite direction (within a cone) to redirect it, gaining speed/damage/scale
 * each chain. Puddle and damage only on final impact.
 *
 * Blueprint workflow:
 * 1. ActivateAbility -> CommitAbility
 * 2. AimMode task -> OnInputReleased
 * 3. LaunchYoyo(TargetLocation, SocketTag)
 * 4. YoyoControl task monitors projectile
 *    - OnRecastTriggered -> RecastYoyo(NewTarget)  (restarts task)
 *    - OnProjectileLost -> EndAbility
 */
UCLASS()
class RAGEINMAGE_API URageInMageYoyoSpell : public URageInMageDamageGameplayAbility
{
	GENERATED_BODY()

public:
	URageInMageYoyoSpell();

	/** Launch the yo-yo projectile toward the target. Called from Blueprint. */
	UFUNCTION(BlueprintCallable, Category = "Yoyo")
	void LaunchYoyo(const FVector& TargetLocation, const FGameplayTag& SocketTag);

	/** Redirect the yo-yo in a new direction (recast). Validates cone angle.
	 *  Returns true if the recast was valid. */
	UFUNCTION(BlueprintCallable, Category = "Yoyo")
	bool RecastYoyo(const FVector& NewTargetLocation);

	/** Current chain count (0 = initial launch). */
	UFUNCTION(BlueprintPure, Category = "Yoyo")
	int32 GetCurrentChain() const { return CurrentChain; }

	/** Called when the yo-yo is first launched. Override in BP for VFX. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Yoyo")
	void OnYoyoLaunched(const FVector& LaunchDirection);

	/** Called each time the yo-yo is successfully recast. Override in BP for chain VFX/SFX. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Yoyo")
	void OnYoyoRecast(int32 ChainCount, const FVector& NewDirection);

	/** Called when the yo-yo hits something and the ability ends. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Yoyo")
	void OnYoyoImpact();

	// ── Aim Indicator ──

	/** Called when aim mode begins. Override in BP to spawn aim indicator Niagara system. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Yoyo|AimIndicator")
	void OnAimBegin();

	/** Called when aim mode ends (release or cancel). Override in BP to destroy aim indicator VFX. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Yoyo|AimIndicator")
	void OnAimEnd();

	/** Call from Blueprint when aim mode begins (before AimMode task starts ticking). */
	UFUNCTION(BlueprintCallable, Category = "Yoyo|AimIndicator")
	void BeginAimMode();

	/** Call from Blueprint when aim mode ends (release or cancel). */
	UFUNCTION(BlueprintCallable, Category = "Yoyo|AimIndicator")
	void EndAimMode();

protected:
	// ── Projectile ──

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yoyo|Projectile")
	TSubclassOf<ARageInMageYoyoProjectile> YoyoProjectileClass;

	/** Initial launch speed (units/sec). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yoyo|Projectile")
	float InitialSpeed = 1200.f;

	/** Each recast multiplies the speed by this factor. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yoyo|Projectile")
	float SpeedMultiplierPerChain = 1.3f;

	/** Each recast multiplies the damage by this factor. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yoyo|Projectile")
	float DamageMultiplierPerChain = 1.25f;

	/** Each recast multiplies the visual scale by this factor. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yoyo|Projectile")
	float ScaleMultiplierPerChain = 1.15f;

	// ── Pull Physics ──

	/** Acceleration toward PullOrigin (units/sec^2). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yoyo|Pull")
	float PullStrength = 800.f;

	/** When within this radius of PullOrigin, pull stops and yo-yo flies off straight. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yoyo|Pull")
	float OriginReturnRadius = 150.f;

	/** Distance from CASTER at which recast becomes available. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yoyo|Recast")
	float RecastRadius = 300.f;

	/** Half-angle of the valid recast cone (in degrees). Recast must be roughly aligned with the current heading.
	 *  90 = full hemisphere (very lenient), 30 = tight boost. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yoyo|Recast")
	float RecastConeHalfAngle = 30.f;

	/** Maximum number of recasts allowed. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yoyo|Recast")
	int32 MaxChains = 5;

	// ── Puddle Zone (final impact only) ──

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yoyo|Puddle")
	TSubclassOf<ARageInMageZone> PuddleZoneClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yoyo|Puddle")
	float BasePuddleZoneRadius = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yoyo|Puddle")
	float PuddleZoneDuration = 4.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yoyo|Puddle")
	float PuddleZoneTickInterval = 1.f;

	/** Separate damage GE for the puddle zone DoT. If null, uses DamageEffectClass. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yoyo|Puddle")
	TSubclassOf<UGameplayEffect> PuddleDamageEffectClass;

	/** Separate damage tags for puddle DoT. If empty, uses main DamageTypeTags. */
	UPROPERTY(EditDefaultsOnly, Category = "Yoyo|Puddle")
	TMap<FGameplayTag, FScalableFloat> PuddleDamageTypeTags;

	/** Niagara effect for puddle zones. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yoyo|Puddle|VFX")
	TObjectPtr<UNiagaraSystem> PuddleZoneEffect;

	/** Looping sound for puddle zones. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yoyo|Puddle|SFX")
	TObjectPtr<USoundBase> PuddleZoneLoopSound;

	// ── VFX / SFX ──

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yoyo|SFX")
	TObjectPtr<USoundBase> LaunchSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yoyo|SFX")
	TObjectPtr<USoundBase> RecastSound;


	// ── Ability Lifecycle ──

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	/** Create a damage spec with current chain multiplier applied. */
	FGameplayEffectSpecHandle MakeYoyoDamageSpec() const;

	/** Create a damage spec for the puddle zone (uses PuddleDamage* if set, otherwise falls back). */
	FGameplayEffectSpecHandle MakePuddleDamageSpec() const;

	/** Start/restart the YoyoControl task to monitor the projectile. */
	void StartYoyoControlTask();

	/** Called by YoyoControl task when player recasts. */
	UFUNCTION()
	void HandleRecastTriggered(const FVector& AimPosition);

	/** Called by YoyoControl task when projectile is destroyed. */
	UFUNCTION()
	void HandleProjectileLost();

	UPROPERTY()
	TObjectPtr<ARageInMageYoyoProjectile> ActiveProjectile;

	int32 CurrentChain = 0;
	float CurrentSpeed = 0.f;
	float CurrentDamageMultiplier = 1.f;
	float CurrentScaleMultiplier = 1.f;

	bool bIsAiming = false;
};
