// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RageInMageSpikeField.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UNiagaraComponent;
class UNiagaraSystem;

/**
 * The field of stone spikes left behind by Jagged Terra Form.
 *
 * All the damage and the directional push happen in the ABILITY at cast time (the spikes erupt in
 * one burst), so this actor is only the aftermath: an impassable barrier with a lifetime.
 *
 * Blocking collision switches on after BlockingActivationDelay rather than immediately — the
 * enemies caught in the eruption are being shoved out at that exact moment, and spawning a solid
 * volume on top of them would trap them inside their own knockback.
 *
 * Size comes from the spawn transform's SCALE (the ability applies the Immovable Mass stance
 * scalar there), which scales the collision box and the mesh together.
 */
UCLASS()
class RAGEINMAGE_API ARageInMageSpikeField : public AActor
{
	GENERATED_BODY()

public:
	ARageInMageSpikeField();

	/** Half-extent of the spike patch on the ground (box extent X and Y). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpikeField|Dimensions", meta = (ExposeOnSpawn = true))
	float FieldHalfExtent = 350.f;

	/** Half-height of the spikes (box extent Z). Full height = 2x this. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpikeField|Dimensions", meta = (ExposeOnSpawn = true))
	float FieldHalfHeight = 100.f;

	/** How long the spikes stay up before sinking back into the ground. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpikeField", meta = (ExposeOnSpawn = true))
	float FieldDuration = 4.f;

	/** Grace period before the barrier turns solid, so the pushed enemies clear it first. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpikeField", meta = (ExposeOnSpawn = true))
	float BlockingActivationDelay = 0.15f;

	/** Optional VFX played once as the spikes erupt. */
	UPROPERTY(EditAnywhere, Category = "SpikeField|VFX")
	TObjectPtr<UNiagaraSystem> EruptEffect;

	/** Optional sound played once as the spikes erupt. */
	UPROPERTY(EditAnywhere, Category = "SpikeField|SFX")
	TObjectPtr<USoundBase> EruptSound;

	/** The blocking box. Public so the spawning ability can read its extents for the aim preview. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpikeField")
	TObjectPtr<UBoxComponent> SpikeCollision;

	/** Visual spikes. Purely cosmetic — the box above is what actually blocks. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpikeField")
	TObjectPtr<UStaticMeshComponent> SpikeMesh;

protected:
	virtual void BeginPlay() override;

private:
	/** Flips the box from no-collision to blocking once the eruption push has resolved. */
	void EnableBlocking();

	FTimerHandle BlockingTimerHandle;
};
