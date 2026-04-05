// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "RageInMageZone.generated.h"

class USphereComponent;
class UNiagaraSystem;
class UNiagaraComponent;

/**
 * Base AoE zone actor that:
 * - Damages enemies inside on a configurable tick interval
 * - Passes ZoneRadius and ZoneDuration to a Niagara system as user parameters
 * - Lasts for a configurable duration
 * Subclass for element-specific zones (FireZone, PoisonZone, etc.).
 */
UCLASS()
class RAGEINMAGE_API ARageInMageZone : public AActor
{
	GENERATED_BODY()

public:
	ARageInMageZone();

	/** The GE spec used for damage ticks. */
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

	/** Radius of the zone. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
	float ZoneRadius = 300.f;

	/** How long the zone lasts. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
	float ZoneDuration = 5.f;

	/** How often the zone ticks damage. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
	float DamageTickInterval = 1.f;

	/** Niagara effect for the zone. Can be set at spawn time or on the BP. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone|VFX")
	TObjectPtr<UNiagaraSystem> ZoneEffect;

	/** Looping sound while the zone is active. Can be set at spawn time or on the BP. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone|SFX")
	TObjectPtr<USoundBase> ZoneLoopSound;

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	UFUNCTION()
	void OnZoneOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> ZoneCollision;

	/** Override in subclasses to add extra per-tick logic (e.g. progressive slow, freeze). */
	virtual void DamageTickEnemiesInside();

	/** Actors currently inside the zone. Maintained by overlap events. */
	UPROPERTY()
	TArray<TObjectPtr<AActor>> ActorsInside;

private:
	FTimerHandle DamageTickTimerHandle;

	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ZoneNiagaraComponent;
};
