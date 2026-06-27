// Copyright Reaplays

#include "Actor/RageInMageWaveBox.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/RageInMageProjectile.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Character/RageInMageCharacterBase.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "RageInMageGameplayTag.h"


ARageInMageWaveBox::ARageInMageWaveBox()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(BoxCollision);

	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollision->SetCollisionObjectType(ECC_WorldDynamic);
	BoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	// Overlap projectiles so we can intercept them
	BoxCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	// Overlap pawns so enemies walking into the wave get hit by SurfDamageCheck
	BoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxCollision->SetGenerateOverlapEvents(true);

	BoxCollision->SetBoxExtent(FVector(100.f, 30.f, 10.f));
}

void ARageInMageWaveBox::BeginPlay()
{
	Super::BeginPlay();

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ARageInMageWaveBox::OnBoxOverlap);
}

void ARageInMageWaveBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		ProcessEnemyOverlaps();
	}
}

void ARageInMageWaveBox::UpdatePosition(const FVector& CharacterLocation, float CurrentHeight, float CapsuleHalfHeight)
{
	// Position the box so its top surface is at the character's feet.
	// Character feet are at CharacterLocation.Z - CapsuleHalfHeight.
	// Box center should be at feet - box Z extent.
	const float BoxZExtent = BoxCollision->GetScaledBoxExtent().Z;
	FVector BoxLocation = CharacterLocation;
	BoxLocation.Z = CharacterLocation.Z - CapsuleHalfHeight - BoxZExtent;
	SetActorLocation(BoxLocation);
}

void ARageInMageWaveBox::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	ARageInMageProjectile* Projectile = Cast<ARageInMageProjectile>(OtherActor);
	if (!Projectile) return;

	// Let friendly projectiles pass through
	if (URageInMageAbilitySystemLibrary::IsFriendly(GetInstigator(), Projectile->GetInstigator()))
	{
		return;
	}

	// Lightning projectiles route damage to the rider
	if (IsLightningProjectile(Projectile))
	{
		RouteDamageToRider(Projectile);
	}
	// Non-lightning projectiles are simply absorbed (destroyed with no effect)

	if (HasAuthority())
	{
		Projectile->Destroy();
	}
}

bool ARageInMageWaveBox::IsLightningProjectile(ARageInMageProjectile* Projectile) const
{
	if (!Projectile || !Projectile->DamageEffectSpecHandle.IsValid()) return false;

	const FGameplayEffectSpec* Spec = Projectile->DamageEffectSpecHandle.Data.Get();
	if (!Spec) return false;

	const float ElectricDamage = Spec->GetSetByCallerMagnitude(
		FRageInMageGameplayTag::Get().DamageType_MagicalDamage_Electric, false, 0.f);

	return ElectricDamage > 0.f;
}

void ARageInMageWaveBox::RouteDamageToRider(ARageInMageProjectile* Projectile)
{
	if (!RidingCharacter.IsValid() || !HasAuthority()) return;
	if (!Projectile || !Projectile->DamageEffectSpecHandle.IsValid()) return;

	UAbilitySystemComponent* RiderASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(RidingCharacter.Get());
	if (RiderASC && Projectile->DamageEffectSpecHandle.Data.IsValid())
	{
		RiderASC->ApplyGameplayEffectSpecToSelf(*Projectile->DamageEffectSpecHandle.Data.Get());
	}
}

void ARageInMageWaveBox::ProcessEnemyOverlaps()
{
	TArray<AActor*> OverlappingActors;
	BoxCollision->GetOverlappingActors(OverlappingActors);

	if (OverlappingActors.Num() == 0) return;

	AActor* Caster = GetInstigator();
	const double CurrentTime = GetWorld()->GetTimeSeconds();

	for (AActor* Overlapped : OverlappingActors)
	{
		if (!Overlapped || Overlapped == RidingCharacter.Get()) continue;
		if (!Overlapped->Implements<UCombatInterface>() || ICombatInterface::Execute_IsDead(Overlapped)) continue;

		AActor* Target = ICombatInterface::Execute_GetAvatar(Overlapped);
		if (!Target || Target == RidingCharacter.Get()) continue;
		if (URageInMageAbilitySystemLibrary::IsBothEnemy(Caster, Target)) continue;

		ARageInMageCharacterBase* TargetChar = Cast<ARageInMageCharacterBase>(Target);

		// Drag the enemy along with the wave every tick it remains caught in the box —
		// this is what makes it feel like the wave is sweeping them along rather than just
		// bumping them once. bZOverride=false leaves gravity/Z velocity untouched so repeated
		// calls don't launch them upward.
		if (TargetChar && DragSpeed > 0.f)
		{
			const FVector Forward = GetActorForwardVector();
			const FVector DragVelocity = FVector(Forward.X, Forward.Y, 0.f) * DragSpeed;
			TargetChar->ApplyKnockbackImpulse(DragVelocity, true, false);
		}

		// Per-enemy tick interval check (damage + on-hit effect only — drag above is unconditional)
		if (const double* LastHitTime = DamagedEnemyTimestamps.Find(Target))
		{
			if (DamageTickInterval <= 0.f || (CurrentTime - *LastHitTime) < DamageTickInterval)
			{
				continue;
			}
		}
		DamagedEnemyTimestamps.Add(Target, CurrentTime);

		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
		if (!TargetASC) continue;

		if (DamageEffectSpecHandle.IsValid() && DamageEffectSpecHandle.Data.IsValid())
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}

		if (OnHitEffectSpecHandle.IsValid() && OnHitEffectSpecHandle.Data.IsValid())
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*OnHitEffectSpecHandle.Data.Get());
		}

		if (PushbackStrength > 0.f && TargetChar)
		{
			TargetChar->ApplyKnockbackImpulse(FVector(0.f, 0.f, PushbackStrength), false, true);
		}
	}
}
