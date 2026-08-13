// Copyright Reaplays


#include "AbilitySystem/Components/MomentumComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "AbilitySystem/Abilities/RageInMageGameplayAbility.h"
#include "RageInMageGameplayTag.h"
#include "GameFramework/Character.h"

UMomentumComponent::UMomentumComponent()
{
	// Server-authoritative gauge: tick enabled only once ActivateMomentum() runs on the authority.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(false); // the gauge replicates via the Momentum attribute, not this component
}

void UMomentumComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMomentumComponent::ActivateMomentum()
{
	if (bActivated || !GetOwner() || !GetOwner()->HasAuthority()) return;

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	// Resolve through IAbilitySystemInterface, NOT FindComponentByClass — see the matching note in
	// UImmovableMassComponent::ActivateStance. A pawn component search can find a stray ASC with no
	// attribute sets, and every SetNumericAttributeBase against it ensures inside GAS.
	OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (!OwnerCharacter.IsValid() || !OwnerASC.IsValid()) return;

	// Movement-spell casts grant a momentum burst — no change needed to the abilities themselves.
	OwnerASC->AbilityActivatedCallbacks.AddUObject(this, &UMomentumComponent::OnAbilityActivated);

	LastLocation = OwnerCharacter->GetActorLocation();
	SetGauge(0.f);
	CurrentStage = 0;
	bActivated = true;

	SetComponentTickEnabled(true);
}

void UMomentumComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bActivated || !OwnerCharacter.IsValid() || !OwnerASC.IsValid()) return;

	UpdateGauge(DeltaTime);
	EvaluateStage();
}

void UMomentumComponent::UpdateGauge(float DeltaTime)
{
	if (DeltaTime <= 0.f) return;

	const FVector Location = OwnerCharacter->GetActorLocation();
	const float Speed = (Location - LastLocation).Size2D() / DeltaTime;
	LastLocation = Location;

	float Gauge = GetGauge();
	if (Speed > MovingSpeedThreshold)
	{
		// Moving builds momentum.
		TimeStill = 0.f;
		Gauge += MomentumPerSecondMoving * DeltaTime;
	}
	else
	{
		// Standing still bleeds it, after a short grace so tiny pauses don't punish you.
		TimeStill += DeltaTime;
		if (TimeStill >= StillnessDecayDelay)
		{
			Gauge -= DecayPerSecond * DeltaTime;
		}
	}

	SetGauge(FMath::Clamp(Gauge, 0.f, MaxGauge));
}

void UMomentumComponent::AddMomentum(float Delta)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !OwnerASC.IsValid()) return;
	SetGauge(FMath::Clamp(GetGauge() + Delta, 0.f, MaxGauge));
	EvaluateStage();
}

void UMomentumComponent::SetGauge(float NewGauge)
{
	if (OwnerASC.IsValid())
	{
		OwnerASC->SetNumericAttributeBase(URageInMageAttributeSet::GetMomentumAttribute(), NewGauge);
	}
}

float UMomentumComponent::GetGauge() const
{
	return OwnerASC.IsValid() ? OwnerASC->GetNumericAttribute(URageInMageAttributeSet::GetMomentumAttribute()) : 0.f;
}

int32 UMomentumComponent::StageForGauge(float Gauge) const
{
	if (Gauge >= Stage3Threshold) return 3;
	if (Gauge >= Stage2Threshold) return 2;
	if (Gauge >= Stage1Threshold) return 1;
	return 0;
}

void UMomentumComponent::EvaluateStage()
{
	const int32 NewStage = StageForGauge(GetGauge());
	if (NewStage != CurrentStage)
	{
		ApplyStageEffect(NewStage);
	}
}

void UMomentumComponent::ApplyStageEffect(int32 NewStage)
{
	if (!OwnerASC.IsValid()) return;

	// Swap the stage buff GE. Bidirectional — works climbing or dropping (unlike the one-way
	// Charge threshold-cross system, which must not be reused here).
	if (CurrentStageEffectHandle.IsValid())
	{
		OwnerASC->RemoveActiveGameplayEffect(CurrentStageEffectHandle);
		CurrentStageEffectHandle.Invalidate();
	}

	TSubclassOf<UGameplayEffect> StageGE = nullptr;
	switch (NewStage)
	{
	case 1: StageGE = Stage1Effect; break;
	case 2: StageGE = Stage2Effect; break;
	case 3: StageGE = Stage3Effect; break;
	default: break; // Stage 0 — no effect
	}

	if (StageGE)
	{
		FGameplayEffectContextHandle Ctx = OwnerASC->MakeEffectContext();
		Ctx.AddSourceObject(GetOwner());
		const FGameplayEffectSpecHandle Spec = OwnerASC->MakeOutgoingSpec(StageGE, 1.f, Ctx);
		if (Spec.IsValid())
		{
			CurrentStageEffectHandle = OwnerASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}

	CurrentStage = NewStage;
}

void UMomentumComponent::OnAbilityActivated(UGameplayAbility* Ability)
{
	if (const URageInMageGameplayAbility* RageAbility = Cast<URageInMageGameplayAbility>(Ability))
	{
		if (RageAbility->StartupAbilityTypeTag == FRageInMageGameplayTag::Get().Ability_Type_Movement)
		{
			AddMomentum(MomentumPerMovementCast);
		}
	}
}
