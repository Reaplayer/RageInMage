// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "RageInMageFireZone.generated.h"

class USphereComponent;
class UNiagaraSystem;
class UDecalComponent;

/**
 * A ground fire zone (AoE area) that:
 * - Damages and applies Heat to any enemy inside
 * - Ticks damage periodically
 * - Lasts for a configurable duration
 * Used for Blazing Flame Release ground fire and Fall Of The Scorching Sun meteor impact zones.
 */
UCLASS()
class RAGEINMAGE_API ARageInMageFireZone : public AActor
{
	GENERATED_BODY()

public:
	ARageInMageFireZone();

	/** The GE spec used for damage ticks. */
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

	/** Radius of the fire zone. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireZone")
	float ZoneRadius = 300.f;

	/** How long the zone lasts. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireZone")
	float ZoneDuration = 5.f;

	/** How often the zone ticks damage. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireZone")
	float DamageTickInterval = 1.f;

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	UFUNCTION()
	void OnZoneOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> ZoneCollision;

	UPROPERTY(EditAnywhere, Category = "FireZone|VFX")
	TObjectPtr<UNiagaraSystem> ZoneEffect;

	UPROPERTY(EditAnywhere, Category = "FireZone|SFX")
	TObjectPtr<USoundBase> ZoneLoopSound;

private:
	void DamageTickEnemiesInside();

	FTimerHandle DamageTickTimerHandle;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> ActorsInside;

	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;
};
