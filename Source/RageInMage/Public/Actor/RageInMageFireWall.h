// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "RageInMageFireWall.generated.h"

class UBoxComponent;
class UNiagaraComponent;
class UNiagaraSystem;

/**
 * A stationary fire wall that:
 * - Destroys projectiles that hit it
 * - Damages enemies that enter or remain inside
 * - Ticks damage every second to enemies inside
 * - Has a configurable lifetime (default 5 seconds)
 */
UCLASS()
class RAGEINMAGE_API ARageInMageFireWall : public AActor
{
	GENERATED_BODY()

public:
	ARageInMageFireWall();

	/** Set the damage spec to apply to enemies. Must be called after spawning. */
	void SetDamageSpec(const FGameplayEffectSpecHandle& InSpecHandle);

	/** The GE spec handle used for damage ticks. */
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

	/** How long the wall lasts before being destroyed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireWall")
	float WallDuration = 5.f;

	/** How often the wall ticks damage on enemies inside. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireWall")
	float DamageTickInterval = 1.f;

	/** Whether this wall destroys enemy projectiles on contact. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireWall")
	bool bDestroyProjectiles = true;

	/** Whether this wall also destroys friendly (own) projectiles. If false, friendly projectiles pass through. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireWall")
	bool bDestroyFriendlyProjectiles = false;

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	UFUNCTION()
	void OnWallOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnWallEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> WallCollision;

	UPROPERTY(EditAnywhere, Category = "FireWall|VFX")
	TObjectPtr<UNiagaraSystem> WallEffect;

	UPROPERTY(EditAnywhere, Category = "FireWall|SFX")
	TObjectPtr<USoundBase> WallLoopSound;

private:
	void DamageTickEnemiesInside();

	FTimerHandle DamageTickTimerHandle;

	/** Actors currently inside the wall. */
	UPROPERTY()
	TArray<TObjectPtr<AActor>> ActorsInside;

	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;
};
