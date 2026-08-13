// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RageInMageRockWall.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UNiagaraComponent;
class UNiagaraSystem;

/**
 * The Earth mage's Rock Solid wall — an indestructible slab of rock that exists for a few
 * seconds and then crumbles.
 *
 * Deliberately the OPPOSITE of ARageInMageFireWall: the fire wall OVERLAPS (things walk through
 * it and take damage), this one BLOCKS. It deals no damage, has no health, cannot be destroyed
 * early, and is solid from both sides — for the caster too. It is purely a piece of temporary
 * level geometry you drop on the battlefield to path around.
 *
 * Size comes from the spawn transform's SCALE (the ability applies the Immovable Mass stance
 * scalar there), which scales the collision box and the mesh together.
 */
UCLASS()
class RAGEINMAGE_API ARageInMageRockWall : public AActor
{
	GENERATED_BODY()

public:
	ARageInMageRockWall();

	/** Half-length of the wall along its span (box extent X). Full length = 2x this. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RockWall|Dimensions", meta = (ExposeOnSpawn = true))
	float WallHalfWidth = 250.f;

	/** Half-thickness of the wall (box extent Y). Full thickness = 2x this. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RockWall|Dimensions", meta = (ExposeOnSpawn = true))
	float WallHalfThickness = 40.f;

	/** Half-height of the wall (box extent Z). Full height = 2x this. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RockWall|Dimensions", meta = (ExposeOnSpawn = true))
	float WallHalfHeight = 150.f;

	/** How long the wall stands before crumbling. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RockWall", meta = (ExposeOnSpawn = true))
	float WallDuration = 5.f;

	/** Optional VFX played once as the wall erupts from the ground. */
	UPROPERTY(EditAnywhere, Category = "RockWall|VFX")
	TObjectPtr<UNiagaraSystem> SpawnEffect;

	/** Optional VFX played once as the wall crumbles away. */
	UPROPERTY(EditAnywhere, Category = "RockWall|VFX")
	TObjectPtr<UNiagaraSystem> CrumbleEffect;

	/** Optional sound played once as the wall erupts. */
	UPROPERTY(EditAnywhere, Category = "RockWall|SFX")
	TObjectPtr<USoundBase> SpawnSound;

	/** The blocking box. Public so the spawning ability can read/preview its extents. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RockWall")
	TObjectPtr<UBoxComponent> WallCollision;

	/** Visual slab. Purely cosmetic — the box above is what actually blocks. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RockWall")
	TObjectPtr<UStaticMeshComponent> WallMesh;

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
};
