// Copyright Reaplays

#include "Actor/RageInMageBoulder.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Character/RageInMageCharacterBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


ARageInMageBoulder::ARageInMageBoulder()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	BoulderCollision = CreateDefaultSubobject<USphereComponent>(TEXT("BoulderCollision"));
	SetRootComponent(BoulderCollision);
	BoulderCollision->SetSphereRadius(StartRadius);
	// Driven kinematically, so it only needs to be queryable — it detects what it runs into
	// itself rather than being pushed around by it.
	BoulderCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoulderCollision->SetCollisionObjectType(ECC_WorldDynamic);
	BoulderCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoulderCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	BoulderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoulderMesh"));
	BoulderMesh->SetupAttachment(BoulderCollision);
	// Cosmetic only — the sphere above is the boulder as far as the game is concerned.
	BoulderMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoulderMesh->SetCollisionProfileName(TEXT("NoCollision"));

	// Child of the sphere but deliberately NOT of the mesh: the mesh spins to fake rolling and the
	// rider must not spin with it. Its Z is rewritten to the current radius every tick, which is
	// what keeps the rider planted on top as the boulder grows.
	RiderAttachPoint = CreateDefaultSubobject<USceneComponent>(TEXT("RiderAttachPoint"));
	RiderAttachPoint->SetupAttachment(BoulderCollision);
	RiderAttachPoint->SetRelativeLocation(FVector(0.f, 0.f, StartRadius));
}

void ARageInMageBoulder::BeginPlay()
{
	Super::BeginPlay();

	CurrentRadius = StartRadius;

	// Derive the mesh's natural radius so any rock mesh can be dropped in and still fit the sphere.
	if (BoulderMesh && BoulderMesh->GetStaticMesh())
	{
		MeshBaseRadius = BoulderMesh->GetStaticMesh()->GetBounds().SphereRadius;
	}
	if (MeshBaseRadius <= KINDA_SMALL_NUMBER)
	{
		MeshBaseRadius = 50.f; // engine sphere/cube primitives are 50uu at scale 1
	}

	UpdateGrowth(0.f);
}

float ARageInMageBoulder::GetGrowthAlpha() const
{
	if (GrowthDuration <= 0.f) return 1.f;
	return FMath::Clamp(TimeRolled / GrowthDuration, 0.f, 1.f);
}

void ARageInMageBoulder::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bRolling || !HasAuthority()) return;

	TimeRolled += DeltaSeconds;

	if (MaxLifetime > 0.f && TimeRolled >= MaxLifetime)
	{
		EndRoll();
		return;
	}

	UpdateGrowth(DeltaSeconds);
	UpdateMovement(DeltaSeconds);
	CheckForImpacts();
}

void ARageInMageBoulder::UpdateGrowth(float DeltaSeconds)
{
	const float Alpha = GetGrowthAlpha();

	CurrentRadius = FMath::Lerp(StartRadius, MaxRadius, Alpha);
	CurrentDamageMultiplier = FMath::Lerp(1.f, MaxDamageMultiplier, Alpha);

	BoulderCollision->SetSphereRadius(CurrentRadius);
	BoulderMesh->SetRelativeScale3D(FVector(CurrentRadius / MeshBaseRadius));
	// Top of the sphere. Grows with the radius, so the rider rises instead of sinking into it.
	RiderAttachPoint->SetRelativeLocation(FVector(0.f, 0.f, CurrentRadius));
}

void ARageInMageBoulder::UpdateMovement(float DeltaSeconds)
{
	// Steering is consumed each tick — Move() only fires while the stick is deflected, so a
	// leftover value would keep the boulder turning after the player let go.
	if (!FMath::IsNearlyZero(PendingSteer))
	{
		AddActorWorldRotation(FRotator(0.f, PendingSteer * SteerRate * DeltaSeconds, 0.f));
	}
	PendingSteer = 0.f;

	FVector NewLocation = GetActorLocation() + GetActorForwardVector() * RollSpeed * DeltaSeconds;

	// Sit the sphere tangent to whatever ground is under it: centre = ground + radius. This is the
	// same line that stops growth pushing the boulder through the floor, and it follows slopes.
	FCollisionQueryParams GroundParams;
	GroundParams.AddIgnoredActor(this);
	if (CurrentRider)
	{
		GroundParams.AddIgnoredActor(CurrentRider);
	}

	FHitResult GroundHit;
	const FVector TraceStart = NewLocation + FVector(0.f, 0.f, CurrentRadius);
	const FVector TraceEnd = NewLocation - FVector(0.f, 0.f, CurrentRadius * 3.f);
	if (GetWorld()->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_Visibility, GroundParams))
	{
		NewLocation.Z = GroundHit.ImpactPoint.Z + CurrentRadius;
	}

	SetActorLocation(NewLocation, false);

	// Roll the mesh at the rate a sphere of this radius would actually turn (v = wr), so a bigger
	// boulder visibly rolls slower. Only the mesh moves — the rider's attach point is untouched.
	if (CurrentRadius > KINDA_SMALL_NUMBER)
	{
		MeshRollAngle += FMath::RadiansToDegrees(RollSpeed * DeltaSeconds / CurrentRadius);
		BoulderMesh->SetRelativeRotation(FRotator(MeshRollAngle, 0.f, 0.f));
	}
}

void ARageInMageBoulder::CheckForImpacts()
{
	AActor* Caster = GetInstigator();

	// Run over anyone standing in it.
	TArray<AActor*> OverlappingActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	if (CurrentRider)
	{
		ActorsToIgnore.Add(CurrentRider);
	}
	URageInMageAbilitySystemLibrary::GetLivePlayersWithinRadius(
		this, CurrentRadius, OverlappingActors, ActorsToIgnore, GetActorLocation());

	bool bHitSomeone = false;
	for (AActor* Target : OverlappingActors)
	{
		if (!Target || Target == CurrentRider || AlreadyHit.Contains(Target)) continue;
		if (Caster && URageInMageAbilitySystemLibrary::IsFriendly(Caster, Target)) continue;

		HitCharacter(Target);
		bHitSomeone = true;
	}

	if (bHitSomeone && bEndOnCharacterHit)
	{
		EndRoll();
		return;
	}

	// Probe ahead for a wall. Swept rather than a line so it catches anything the boulder's full
	// width would meet, not just what's dead centre.
	FCollisionQueryParams WallParams;
	WallParams.AddIgnoredActor(this);
	if (CurrentRider)
	{
		WallParams.AddIgnoredActor(CurrentRider);
	}

	const FVector Start = GetActorLocation();
	const FVector End = Start + GetActorForwardVector() * (CurrentRadius * 0.5f);

	FHitResult WallHit;
	if (GetWorld()->SweepSingleByChannel(WallHit, Start, End, FQuat::Identity, ECC_WorldStatic,
		FCollisionShape::MakeSphere(CurrentRadius), WallParams))
	{
		EndRoll();
	}
}

void ARageInMageBoulder::HitCharacter(AActor* Target)
{
	AlreadyHit.Add(Target);

	if (DamageEffectSpecHandle.IsValid())
	{
		// Re-stamp the magnitudes at the boulder's CURRENT size — the spec was built at spawn,
		// when the boulder was still small.
		for (const TPair<FGameplayTag, float>& Pair : BaseDamageByType)
		{
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
				DamageEffectSpecHandle, Pair.Key, Pair.Value * CurrentDamageMultiplier);
		}

		if (UAbilitySystemComponent* TargetASC =
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
	}

	if (ARageInMageCharacterBase* TargetCharacter = Cast<ARageInMageCharacterBase>(Target))
	{
		FVector Push = GetActorForwardVector().GetSafeNormal2D();
		Push.Z = FMath::Clamp(ImpactKnockbackUpward, 0.f, 1.f);
		Push.Normalize();
		TargetCharacter->ApplyKnockbackImpulse(Push * ImpactKnockback, true, true);
	}
}

void ARageInMageBoulder::Mount(ACharacter* Rider)
{
	if (!Rider || CurrentRider) return;

	CurrentRider = Rider;

	Rider->AttachToComponent(RiderAttachPoint,
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld,
			EAttachmentRule::KeepWorld, /*bWeldSimulatedBodies=*/false));

	if (UCharacterMovementComponent* MoveComp = Rider->GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->SetMovementMode(MOVE_None);
	}

	// Neither should shove the other around while they're travelling as one.
	Rider->MoveIgnoreActorAdd(this);
	if (UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		RootPrimitive->IgnoreActorWhenMoving(Rider, true);
	}
}

void ARageInMageBoulder::Dismount()
{
	if (!CurrentRider) return;

	ACharacter* Rider = CurrentRider;
	CurrentRider = nullptr;

	Rider->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));

	if (UCharacterMovementComponent* MoveComp = Rider->GetCharacterMovement())
	{
		MoveComp->SetMovementMode(MOVE_Falling);
	}

	Rider->MoveIgnoreActorRemove(this);
	if (UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		RootPrimitive->IgnoreActorWhenMoving(Rider, false);
	}

	// A small hop so they clear the boulder instead of landing back on it or clipping through.
	if (ARageInMageCharacterBase* RiderCharacter = Cast<ARageInMageCharacterBase>(Rider))
	{
		RiderCharacter->ApplyKnockbackImpulse(FVector(0.f, 0.f, DismountHopStrength), false, true);
	}
}

void ARageInMageBoulder::AddSteerInput(float Value)
{
	PendingSteer = FMath::Clamp(Value, -1.f, 1.f);
}

void ARageInMageBoulder::EndRoll()
{
	if (!bRolling) return;
	bRolling = false;

	if (ImpactEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation(), GetActorRotation());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	Dismount();
	Destroy();
}
