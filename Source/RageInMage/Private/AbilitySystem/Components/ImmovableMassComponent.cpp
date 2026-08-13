// Copyright Reaplays


#include "AbilitySystem/Components/ImmovableMassComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "RageInMageGameplayTag.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "TimerManager.h"

UImmovableMassComponent::UImmovableMassComponent()
{
	// Server-authoritative gauge: tick is enabled only once ActivateStance() runs on the authority.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(false); // the gauge replicates via the ImmovableMass attribute, not this component
}

void UImmovableMassComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UImmovableMassComponent::ActivateStance()
{
	// Authority owns the whole simulation; the ImmovableMass attribute carries state down to clients/UI.
	if (bActivated || !GetOwner() || !GetOwner()->HasAuthority()) return;

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	// Resolve through IAbilitySystemInterface, NOT FindComponentByClass. The player's authoritative ASC
	// lives on the PlayerState, and a component search on the pawn can turn up a stray pawn-owned ASC
	// that has no attribute sets registered — writing the gauge to that one ensures inside GAS.
	// GetAbilitySystemComponent() already tries the interface first and falls back to a component search.
	OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (!OwnerCharacter.IsValid() || !OwnerASC.IsValid()) return;

	const FRageInMageGameplayTag& Tags = FRageInMageGameplayTag::Get();

	// Lazy default: any effect granting one of these tags is a convertible hard CC (anything that
	// removes free movement). Leaving HardCCTags set in the editor overrides this.
	if (HardCCTags.Num() == 0)
	{
		HardCCTags = {
			Tags.Condition_Stunned, Tags.Condition_Rooted, Tags.Condition_Petrified,
			Tags.Condition_Frozen, Tags.Condition_Paralysed, Tags.Condition_Airborne,
			Tags.Condition_Grappled, Tags.Condition_Constricted, Tags.Condition_Slammed
		};
	}

	// Intercept each hard-CC tag as it lands (mirrors ARageInMageCharacterBase::BindCrowdControlDelegates).
	for (const FGameplayTag& CCTag : HardCCTags)
	{
		if (!CCTag.IsValid()) continue;
		if (!OwnerASC->RegisterGameplayTagEvent(CCTag, EGameplayTagEventType::NewOrRemoved).IsBoundToObject(this))
		{
			OwnerASC->RegisterGameplayTagEvent(CCTag, EGameplayTagEventType::NewOrRemoved)
				.AddUObject(this, &UImmovableMassComponent::OnHardCCTagChanged);
		}
	}

	// Overload: enemy slows and converted CCs both accrue on the Slow attribute (percent). Watch it
	// directly instead of hand-summing a pool — the shatter fires the instant Slow crosses the ceiling.
	OwnerASC->GetGameplayAttributeValueChangeDelegate(URageInMageAttributeSet::GetSlowAttribute())
		.AddUObject(this, &UImmovableMassComponent::OnSlowChanged);

	// Diagnostic kept (commented) from the 2026-07-24 stray-ASC hunt: it proved ActivateStance was
	// resolving a phantom pawn-owned ASC with zero attribute sets. Re-enable if gauge writes ever
	// start ensuring inside GAS again.
	//// ---- TEMP DIAGNOSTIC: confirms the ASC fix, and hunts the stray pawn-owned ASC. Remove when done. ----
	//{
	//	UAbilitySystemComponent* ASC = OwnerASC.Get();
	//	UE_LOG(LogTemp, Warning, TEXT("[IMMASS] resolved ASC '%s' owned by '%s' | SpawnedAttributes=%d | HasImmovableMass=%s"),
	//		*GetNameSafe(ASC),
	//		*GetNameSafe(ASC ? ASC->GetOwner() : nullptr),
	//		ASC ? ASC->GetSpawnedAttributes().Num() : -1,
	//		(ASC && ASC->HasAttributeSetForAttribute(URageInMageAttributeSet::GetImmovableMassAttribute()))
	//			? TEXT("YES") : TEXT("NO <<<<<<"));

	//	// Where does the pawn-owned ASC come from? Dump every component on the character.
	//	if (OwnerCharacter.IsValid())
	//	{
	//		TArray<UActorComponent*> Comps;
	//		OwnerCharacter->GetComponents(Comps);
	//		UE_LOG(LogTemp, Warning, TEXT("[IMMASS] character '%s' has %d components:"), *GetNameSafe(OwnerCharacter.Get()), Comps.Num());
	//		for (UActorComponent* C : Comps)
	//		{
	//			const bool bIsASC = C && C->IsA<UAbilitySystemComponent>();
	//			UE_LOG(LogTemp, Warning, TEXT("[IMMASS]    %s : %s%s"),
	//				*GetNameSafe(C), *GetNameSafe(C ? C->GetClass() : nullptr),
	//				bIsASC ? TEXT("   <<<<<< STRAY ASC ON PAWN") : TEXT(""));
	//		}
	//	}
	//}
	//// ---- END TEMP DIAGNOSTIC ----

	LastLocation = OwnerCharacter->GetActorLocation();
	SetGauge(0.f);
	CurrentStage = 0;
	bStage3IgnoreCharged = true;
	bActivated = true;

	SetComponentTickEnabled(true);
}

void UImmovableMassComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bActivated || !OwnerCharacter.IsValid() || !OwnerASC.IsValid()) return;

	UpdateGauge(DeltaTime);
	EvaluateStage();
}

/* ============================ Gauge / staging ============================ */

void UImmovableMassComponent::UpdateGauge(float DeltaTime)
{
	if (DeltaTime <= 0.f) return;

	const FVector Location = OwnerCharacter->GetActorLocation();
	const float Distance = (Location - LastLocation).Size2D();
	LastLocation = Location;

	const float Speed = Distance / DeltaTime;
	float Gauge = GetGauge();

	if (Speed < StillnessSpeedThreshold)
	{
		// Standing still: charge after the stillness delay, unless a shatter locked us out.
		TimeStill += DeltaTime;
		if (!bInVulnerability && TimeStill >= StillnessDelay)
		{
			Gauge += ChargePerSecond * DeltaTime;
		}
	}
	else
	{
		// Moving: distance-based drain (dashing bleeds faster). Self-slow -> less ground -> less drain.
		TimeStill = 0.f;
		const bool bDashing = OwnerASC->HasMatchingGameplayTag(FRageInMageGameplayTag::Get().State_Dashing);
		const float DecayRate = DecayPerUnitMoved * (bDashing ? DashDecayMultiplier : 1.f);
		Gauge -= Distance * DecayRate;
	}

	SetGauge(FMath::Clamp(Gauge, 0.f, MaxGauge));
}

void UImmovableMassComponent::SetGauge(float NewGauge)
{
	if (OwnerASC.IsValid())
	{
		OwnerASC->SetNumericAttributeBase(URageInMageAttributeSet::GetImmovableMassAttribute(), NewGauge);
	}
}

float UImmovableMassComponent::GetGauge() const
{
	return OwnerASC.IsValid() ? OwnerASC->GetNumericAttribute(URageInMageAttributeSet::GetImmovableMassAttribute()) : 0.f;
}

int32 UImmovableMassComponent::StageForGauge(float Gauge) const
{
	if (Gauge >= Stage3Threshold) return 3;
	if (Gauge >= Stage2Threshold) return 2;
	if (Gauge >= Stage1Threshold) return 1;
	return 0;
}

void UImmovableMassComponent::EvaluateStage()
{
	const int32 NewStage = StageForGauge(GetGauge());
	if (NewStage != CurrentStage)
	{
		ApplyStageEffect(NewStage);
	}
}

void UImmovableMassComponent::ApplyStageEffect(int32 NewStage)
{
	if (!OwnerASC.IsValid()) return;

	// Swap the stage buff GE (self-slow + damage reduction). Bidirectional — works climbing or dropping.
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

/* ============================ Hard CC handling ============================ */

float UImmovableMassComponent::CurrentCCConvertSlow() const
{
	switch (CurrentStage)
	{
	case 1: return CCConvertSlowStage1;
	case 2: return CCConvertSlowStage2;
	case 3: return CCConvertSlowStage3;
	default: return 0.f;
	}
}

EImmovableCCResult UImmovableMassComponent::HandleHardCC()
{
	if (!IsStanceActive() || !OwnerASC.IsValid()) return EImmovableCCResult::NotHandled;

	// Stage 3: the first hard CC while charged is negated outright (no slow).
	if (CurrentStage >= 3 && bStage3IgnoreCharged)
	{
		ConsumeStage3Ignore();
		return EImmovableCCResult::Ignored;
	}

	// Otherwise the CC becomes a slow on the Slow channel, which itself feeds the overload (the Slow
	// attribute delegate runs CheckOverload when this lands — no manual pool tracking needed).
	const float SlowPercent = CurrentCCConvertSlow();
	if (ConvertedSlowEffect && SlowPercent > 0.f)
	{
		FGameplayEffectContextHandle Ctx = OwnerASC->MakeEffectContext();
		Ctx.AddSourceObject(GetOwner());
		const FGameplayEffectSpecHandle Spec = OwnerASC->MakeOutgoingSpec(ConvertedSlowEffect, 1.f, Ctx);
		if (Spec.IsValid())
		{
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
				Spec, FRageInMageGameplayTag::Get().Condition_Slowed, SlowPercent);
			OwnerASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}
	return EImmovableCCResult::Converted;
}

void UImmovableMassComponent::ConsumeStage3Ignore()
{
	bStage3IgnoreCharged = false;
	// Real-time cooldown that keeps running regardless of stage transitions (no cheesing via re-entry).
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			IgnoreCooldownTimer, this, &UImmovableMassComponent::OnIgnoreCooldownExpired, IgnoreHardCCCooldown, false);
	}
}

void UImmovableMassComponent::OnIgnoreCooldownExpired()
{
	bStage3IgnoreCharged = true;
}

void UImmovableMassComponent::OnHardCCTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount <= 0 || !OwnerASC.IsValid()) return;
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	const EImmovableCCResult Result = HandleHardCC();
	if (Result == EImmovableCCResult::Ignored || Result == EImmovableCCResult::Converted)
	{
		// Strip the CC effect(s) that granted this tag — the stance replaced it.
		FGameplayTagContainer Container;
		Container.AddTag(CallbackTag);
		OwnerASC->RemoveActiveEffectsWithGrantedTags(Container);
	}
}

/* ============================ Overload ============================ */

void UImmovableMassComponent::CheckOverload()
{
	if (!bActivated || !IsStanceActive() || !OwnerASC.IsValid()) return;

	// Slow is the accumulated slow percent (enemy + converted-CC). Self-slow lives on MovementSpeed and
	// never reaches this, so the threshold is a clean "total real slow hit 100%".
	const float TotalSlow = OwnerASC->GetNumericAttribute(URageInMageAttributeSet::GetSlowAttribute());
	if (TotalSlow >= SlowOverloadThreshold)
	{
		Shatter();
	}
}

void UImmovableMassComponent::Shatter()
{
	if (!OwnerASC.IsValid()) return;

	// Nullify every slow feeding the overload in one shot: they all grant Condition.Slowed (and modify the
	// Slow attribute). Self-slow lives on MovementSpeed and grants no such tag, so the stance's own slow —
	// which we're about to strip anyway by dropping to Stage 0 — is the only MovementSpeed effect touched.
	FGameplayTagContainer SlowTags;
	SlowTags.AddTag(FRageInMageGameplayTag::Get().Condition_Slowed);
	OwnerASC->RemoveActiveEffectsWithGrantedTags(SlowTags);

	// Reset to Stage 0 and lock out recharging. The Stage 3 ignore cooldown is deliberately untouched.
	SetGauge(0.f);
	ApplyStageEffect(0);
	TimeStill = 0.f;
	StartVulnerability();
}

void UImmovableMassComponent::StartVulnerability()
{
	bInVulnerability = true;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			VulnerabilityTimer, this, &UImmovableMassComponent::OnVulnerabilityExpired, VulnerabilityDuration, false);
	}
}

void UImmovableMassComponent::OnVulnerabilityExpired()
{
	bInVulnerability = false;
	TimeStill = 0.f; // must re-earn the stillness delay before charging resumes
}

void UImmovableMassComponent::OnSlowChanged(const FOnAttributeChangeData& Data)
{
	// Any change to the Slow attribute (a slow landing, stacking, or expiring) re-tests the ceiling.
	CheckOverload();
}

/* ============================ Knockback queries ============================ */

float UImmovableMassComponent::GetKnockbackMultiplier() const
{
	float Resist = 0.f;
	switch (CurrentStage)
	{
	case 1: Resist = KnockbackResistStage1; break;
	case 2: Resist = KnockbackResistStage2; break;
	case 3: Resist = KnockbackResistStage3; break;
	default: break;
	}
	return FMath::Clamp(1.f - Resist / 100.f, 0.f, 1.f);
}

EImmovableKnockbackDecision UImmovableMassComponent::EvaluateIncomingKnockback(const FVector& Impulse)
{
	if (!IsStanceActive()) return EImmovableKnockbackDecision::Passthrough;

	// Airborne / knock-up is a hard CC at every active stage (Stage 1 converts, Stage 2+ refuses).
	const bool bAirborne = Impulse.Z >= AirborneImpulseThreshold;
	const bool bHeavy = Impulse.Size() >= HeavyKnockbackThreshold;
	if (bAirborne || bHeavy)
	{
		HandleHardCC();
		return EImmovableKnockbackDecision::Blocked; // no launch — absorbed or converted to a slow
	}

	// Light knockback still launches, scaled down by the stage's resistance.
	return EImmovableKnockbackDecision::Reduced;
}
