// Copyright Krymson Reaplays

#include "Actor/RageInMageDecal.h"
#include "Components/DecalComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

ARageInMageDecal::ARageInMageDecal()
{
	PrimaryActorTick.bCanEverTick = false;

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	SetRootComponent(DecalComponent);

	// Face downward by default (project onto ground)
	DecalComponent->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	DecalComponent->SetFadeScreenSize(0.f);
}

// ──────────────────────────────────────────
// Spawn Helper
// ──────────────────────────────────────────

ARageInMageDecal* ARageInMageDecal::SpawnDecal(
	UObject* WorldContextObject,
	TSubclassOf<ARageInMageDecal> DecalClass,
	const FVector& Location,
	UMaterialInterface* Material,
	const FVector& Size,
	float ScaleInTime,
	float InFadeDelay,
	float InFadeDuration)
{
	if (!WorldContextObject || !DecalClass) return nullptr;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ARageInMageDecal* Decal = World->SpawnActor<ARageInMageDecal>(DecalClass, FTransform(Location), SpawnParams);
	if (!Decal) return nullptr;

	Decal->DecalMaterial = Material;
	Decal->DecalSize = Size;
	Decal->ScaleInDuration = ScaleInTime;
	Decal->FadeDelay = InFadeDelay;
	Decal->FadeDuration = InFadeDuration;

	return Decal;
}

// ──────────────────────────────────────────
// Lifecycle
// ──────────────────────────────────────────

void ARageInMageDecal::BeginPlay()
{
	Super::BeginPlay();

	// Apply material and target size
	if (DecalMaterial)
	{
		DecalComponent->SetDecalMaterial(DecalMaterial);
	}
	DecalComponent->DecalSize = DecalSize;

	// Random yaw for visual variation
	if (bRandomYaw)
	{
		AddActorLocalRotation(FRotator(0.f, FMath::RandRange(0.f, 360.f), 0.f));
	}

	// Auto-destroy after everything completes
	SetLifeSpan(ScaleInDuration + FadeDelay + FadeDuration + 0.1f);

	if (ScaleInDuration > 0.f)
	{
		// Start tiny, scale up
		SetActorScale3D(FVector(0.01f));
		ScaleInStartTime = GetWorld()->GetTimeSeconds();

		GetWorld()->GetTimerManager().SetTimer(
			ScaleInTimerHandle, this, &ARageInMageDecal::ScaleInTick,
			0.016f, /*bLoop=*/true);
	}
	else
	{
		// No scale-in — start fade immediately
		DecalComponent->SetFadeOut(FadeDelay, FadeDuration);
	}
}

void ARageInMageDecal::Destroyed()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(ScaleInTimerHandle);
	}
	Super::Destroyed();
}

// ──────────────────────────────────────────
// Scale-In Animation
// ──────────────────────────────────────────

void ARageInMageDecal::ScaleInTick()
{
	const float Elapsed = GetWorld()->GetTimeSeconds() - ScaleInStartTime;
	const float Alpha = FMath::Clamp(Elapsed / ScaleInDuration, 0.f, 1.f);

	// Smooth ease-out curve for a satisfying pop
	const float EasedAlpha = FMath::InterpEaseOut(0.f, 1.f, Alpha, 2.f);
	SetActorScale3D(FVector(EasedAlpha));

	if (Alpha >= 1.f)
	{
		// Scale-in complete — stop timer, begin fade
		GetWorld()->GetTimerManager().ClearTimer(ScaleInTimerHandle);
		DecalComponent->SetFadeOut(FadeDelay, FadeDuration);
	}
}
