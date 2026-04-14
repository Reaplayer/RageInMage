// Copyright Krymson Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RageInMageDecal.generated.h"

class UDecalComponent;

/**
 * Fire-and-forget decal actor with scale-in animation and fade-out.
 * Use SpawnDecal() for easy one-liner spawning from any ability or actor.
 */
UCLASS()
class RAGEINMAGE_API ARageInMageDecal : public AActor
{
	GENERATED_BODY()

public:
	ARageInMageDecal();

	// ── Spawn Helper ──

	/** Spawn a decal actor at the given location. Returns the spawned instance (or nullptr on failure). */
	UFUNCTION(BlueprintCallable, Category = "RageInMageDecal", meta = (WorldContext = "WorldContextObject"))
	static ARageInMageDecal* SpawnDecal(
		UObject* WorldContextObject,
		TSubclassOf<ARageInMageDecal> DecalClass,
		const FVector& Location,
		UMaterialInterface* Material,
		const FVector& Size,
		float ScaleInTime = 0.5f,
		float InFadeDelay = 3.f,
		float InFadeDuration = 2.f);

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	// ── Components ──

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Decal")
	TObjectPtr<UDecalComponent> DecalComponent;

	// ── Decal Properties ──

	/** Material applied to the decal. Set via SpawnDecal() or in BP defaults. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
	TObjectPtr<UMaterialInterface> DecalMaterial;

	/** Target half-extents of the decal (X, Y = surface spread, Z = projection depth). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
	FVector DecalSize = FVector(128.f, 128.f, 30.f);

	// ── Animation ──

	/** Time (seconds) to scale from zero to full size on spawn. 0 = instant. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal|Animation")
	float ScaleInDuration = 0.5f;

	// ── Fade ──

	/** Seconds to wait (after scale-in completes) before the decal begins fading. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal|Fade")
	float FadeDelay = 3.f;

	/** Duration of the fade-out after the delay. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal|Fade")
	float FadeDuration = 2.f;

	/** If true, the decal spawns with a random yaw rotation for visual variation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal|Fade")
	bool bRandomYaw = true;

private:
	/** Called by repeating timer during scale-in. */
	void ScaleInTick();

	FTimerHandle ScaleInTimerHandle;

	/** World time when scale-in started. */
	float ScaleInStartTime = 0.f;
};
