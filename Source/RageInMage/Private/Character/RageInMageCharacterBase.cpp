// Copyright Reaplays


#include "Character/RageInMageCharacterBase.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "RageInMageGameplayTag.h"
#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "AbilitySystem/Data/ConditionInfo.h"
#include "AbilitySystem/Components/ImmovableMassComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "RageInMage/RageInMageLogChannels.h"

ARageInMageCharacterBase::ARageInMageCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Weapon->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Weapon->PrimaryComponentTick.bCanEverTick = true;
}

URageInMageAttributeSet* ARageInMageCharacterBase::GetRageInMageAttributeSet() const
{
	return CastChecked<URageInMageAttributeSet>(AttributeSet.Get());
}

void ARageInMageCharacterBase::ApplyKnockbackImpulse(FVector Impulse, bool bXYOverride, bool bZOverride)
{
	// Immovable Mass stance (Earth mage): heavy knockback / airborne is absorbed or converted to a slow
	// (no launch); light knockback still launches but scaled down by the current stage's resistance.
	// The component only exists (and only reports active) on the stanced Earth mage — everyone else
	// falls straight through to the normal Poise-scaled launch below.
	if (UImmovableMassComponent* ImmovableMass = FindComponentByClass<UImmovableMassComponent>())
	{
		if (ImmovableMass->IsStanceActive())
		{
			const EImmovableKnockbackDecision Decision = ImmovableMass->EvaluateIncomingKnockback(Impulse);
			if (Decision == EImmovableKnockbackDecision::Blocked)
			{
				return;
			}
			if (Decision == EImmovableKnockbackDecision::Reduced)
			{
				Impulse *= ImmovableMass->GetKnockbackMultiplier();
			}
		}
	}

	const URageInMageAttributeSet* AS = GetRageInMageAttributeSet();
	const float MyPoise = AS ? FMath::Max(0.f, AS->GetPoise()) : 0.f;
	const float PoiseMultiplier = 100.f / (100.f + MyPoise);

	// Cut any in-flight AI move synchronously. The Behavior Tree normally stops chasing via the
	// Effects.HitReaction tag -> "HitReacting" Blackboard key -> decorator abort, but that chain
	// resolves on the BT's next tick. If the target is mid-MoveTo right now, its movement
	// component still has lateral Acceleration pointed at the old goal, which fights this same
	// tick's launch (even at reduced AirControl while airborne) before the BT catches up.
	if (AController* MyController = GetController())
	{
		MyController->StopMovement();
	}

	LaunchCharacter(Impulse * PoiseMultiplier, bXYOverride, bZOverride);
}

void ARageInMageCharacterBase::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Only the character whose own movement caused the hit resolves the transfer/bounce — the
	// actor on the receiving end gets its own NotifyHit call with bSelfMoved=false, which we ignore.
	if (!bSelfMoved || !HasAuthority() || !Other) return;

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (!MoveComp) return;

	FVector FlatVelocity = MoveComp->Velocity;
	FlatVelocity.Z = 0.f;
	const float Speed = FlatVelocity.Size();
	if (Speed < MinKnockbackTransferSpeed) return;

	// De-dupe: sliding/multi-sweep movement resolution can call NotifyHit several times against
	// the same actor within a single frame (or across consecutive frames while still in contact).
	const float Now = GetWorld()->GetTimeSeconds();
	if (LastKnockbackHitActor.Get() == Other && (Now - LastKnockbackHitTime) < 0.2f) return;
	LastKnockbackHitActor = Other;
	LastKnockbackHitTime = Now;

	ARageInMageCharacterBase* OtherCharacter = Cast<ARageInMageCharacterBase>(Other);

	float TargetResistance = TNumericLimits<float>::Max();
	if (OtherCharacter)
	{
		const URageInMageAttributeSet* OtherAS = OtherCharacter->GetRageInMageAttributeSet();
		const float OtherPoise = OtherAS ? FMath::Max(0.f, OtherAS->GetPoise()) : 0.f;
		TargetResistance = BaseShoveResistance + OtherPoise * PoiseResistancePerPoint;
	}

	if (OtherCharacter && Speed > TargetResistance)
	{
		// Movable: shove the target with a share of our momentum, and keep moving ourselves —
		// slowed by how much resistance we had to push through, but never stopped dead.
		const float OvercomeRatio = FMath::Clamp((Speed - TargetResistance) / Speed, 0.f, 1.f);
		const FVector TransferVelocity = FlatVelocity * KnockbackTransferRatio * OvercomeRatio;
		OtherCharacter->ApplyKnockbackImpulse(TransferVelocity, true, false);

		MoveComp->Velocity *= (1.f - KnockbackAbsorptionRatio * (1.f - OvercomeRatio));
	}
	else
	{
		// Immovable (a heavier character, or static world geometry): bounce off it instead of
		// stopping dead. Reflect the horizontal velocity across the hit normal and lose some energy.
		FVector FlatNormal = HitNormal;
		FlatNormal.Z = 0.f;
		FlatNormal = FlatNormal.GetSafeNormal();
		if (!FlatNormal.IsNearlyZero())
		{
			const FVector Reflected = (FlatVelocity - 2.f * FVector::DotProduct(FlatVelocity, FlatNormal) * FlatNormal) * KnockbackBounceRestitution;
			MoveComp->Velocity.X = Reflected.X;
			MoveComp->Velocity.Y = Reflected.Y;
		}
	}
}

UAnimMontage* ARageInMageCharacterBase::GetHitReactionMontage_Implementation()
{
	return HitReactionMontage;
}

void ARageInMageCharacterBase::Die()
{
	// 1. Perform your death logic
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath();
	bDead = true;
	// 2. Broadcast the delegate to notify listeners
	UE_LOG(LogTemp, Warning, TEXT("Die: %s - OnDeathDelegate.IsBound() = %s"),
		*GetName(), OnDeathDelegate.IsBound() ? TEXT("TRUE") : TEXT("FALSE"));
	if (OnDeathDelegate.IsBound())
	{
		OnDeathDelegate.Broadcast(this);
	}
}

void ARageInMageCharacterBase::MulticastHandleDeath_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());
	
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Dissolve();
}

void ARageInMageCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// TEMP DEBUG: on-screen speed readout (name / MaxWalkSpeed / Slow) — see LogMovementSpeedDebug().
	// Comment this timer out again to silence it.
	GetWorldTimerManager().SetTimer(MovementSpeedDebugTimerHandle, this,
		&ARageInMageCharacterBase::LogMovementSpeedDebug, 0.5f, true);
}

FVector ARageInMageCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& SocketTag)
{
	const FRageInMageGameplayTag& GameplayTags = FRageInMageGameplayTag::Get();
	if (SocketTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon))
	{
		return Weapon->GetSocketLocation(WeaponTipSocketName);
	}
	if (SocketTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	if (SocketTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	if (SocketTag.MatchesTagExact(GameplayTags.CombatSocket_Tail))
	{
		return GetMesh()->GetSocketLocation(TailSocketName);
	}
	return FVector::ZeroVector;
}

bool ARageInMageCharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* ARageInMageCharacterBase::GetAvatar_Implementation()
{
	return this;
}

ECharacterClass ARageInMageCharacterBase::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

TArray<FTaggedMontage> ARageInMageCharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

FTaggedMontage ARageInMageCharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (const FTaggedMontage& TaggedMontage : AttackMontages)
	{
		if (TaggedMontage.MontageEventTag.MatchesTagExact(MontageTag))
		{
			return TaggedMontage;
		}
	}
	return FTaggedMontage();
}

UNiagaraSystem* ARageInMageCharacterBase::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

FTaggedMontage ARageInMageCharacterBase::GetRandomAttackMontage_Implementation(bool bIsRanged, bool bIsSummon)
{
	TArray<FTaggedMontage> EligibleMontages;

	for (const FTaggedMontage& Montage : AttackMontages)
	{
		// Determine if this specific montage is Ranged, Melee or Summon based on ProjectileClass or SummonClass
		const bool bIsMontageRanged = Montage.ProjectileClass != nullptr;
		const bool bIsMontageSummon = Montage.SummonClass != nullptr;

		// If we want a Summon, only accept Summons
		if (bIsSummon)
		{
			if (bIsMontageSummon)
			{
				EligibleMontages.Add(Montage);
			}
		}
		// If we want Ranged, only accept Ranged (that are not summons)
		else if (bIsRanged)
		{
			if (bIsMontageRanged)
			{
				EligibleMontages.Add(Montage);
			}
		}
		// If we want Melee (neither ranged nor summon), accept only pure melee
		else 
		{
			if (!bIsMontageRanged && !bIsMontageSummon)
			{
				EligibleMontages.Add(Montage);
			}
		}
	}

	// If we found at least one valid montage, pick a random one
	if (!EligibleMontages.IsEmpty())
	{
		const int32 Selection = FMath::RandRange(0, EligibleMontages.Num() - 1);
		return EligibleMontages[Selection];
	}

	// Return an empty struct if no matching montages were found
	return FTaggedMontage();
}

int32 ARageInMageCharacterBase::GetSummonCount_Implementation()
{
	return SummonCount;
}

void ARageInMageCharacterBase::SetSummonCount_Implementation(int32 NewSummonCount)
{
	SummonCount = NewSummonCount;
}

int32 ARageInMageCharacterBase::GetMaxSummonCount_Implementation()
{
	return MaxSummonCount;
}

void ARageInMageCharacterBase::SetMaxSummonCount_Implementation(int32 NewMaxSummonCount)
{
	MaxSummonCount = NewMaxSummonCount;
}

void ARageInMageCharacterBase::RegisterSpawnedMinion_Implementation(AActor* Minion)
{
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Minion))
	{
		// Guard against double-registration: if already bound, skip
		if (CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &ARageInMageCharacterBase::OnMinionDeath))
		{
			return;
		}

		const int32 OldCount = Execute_GetSummonCount(this);
		Execute_SetSummonCount(this, OldCount + 1);

		CombatInterface->GetOnDeathDelegate().AddDynamic(this, &ARageInMageCharacterBase::OnMinionDeath);
	}
}

void ARageInMageCharacterBase::OnMinionDeath(AActor* DeadActor)
{
	const int32 OldCount = Execute_GetSummonCount(this);
	const int32 NewCount = FMath::Max(0, OldCount - 1);
	Execute_SetSummonCount(this, NewCount);
}

void ARageInMageCharacterBase::InitPlayerAbilityActorInfo()
{
}

void ARageInMageCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check (IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();

	// Debug: Check if AttributeSet is registered
	const UAttributeSet* AS = ASC->GetAttributeSet(URageInMageAttributeSet::StaticClass());
	UE_LOG(LogTemp, Warning, TEXT("ApplyEffectToSelf: ASC has AttributeSet? %s (Pointer: %p)"),
		AS ? TEXT("YES") : TEXT("NO - WILL FAIL!"), AS);

	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);

	UE_LOG(LogTemp, Warning, TEXT("ApplyEffectToSelf: Applying GE %s at Level %f"),
		*GameplayEffectClass->GetName(), Level);

	FActiveGameplayEffectHandle Handle = ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), ASC);

	UE_LOG(LogTemp, Warning, TEXT("ApplyEffectToSelf: GE applied? %s"),
		Handle.IsValid() ? TEXT("YES") : TEXT("NO - FAILED!"));
}

void ARageInMageCharacterBase::InitializeDefaultAttributes() const
{
	UE_LOG(LogTemp, Warning, TEXT("=== InitializeDefaultAttributes START ==="));

	ApplyEffectToSelf(DefaultPrimaryAttributes, 1);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1);
	ApplyEffectToSelf(DefaultVitalAttributes, 1);
	ApplyEffectToSelf(DefaultResistanceAttributes, 1);
	ApplyEffectToSelf(DefaultItemSpecificAttributes, 1);

	// Debug: Check attribute values after application
	const URageInMageAttributeSet* AS = Cast<URageInMageAttributeSet>(GetAbilitySystemComponent()->GetAttributeSet(URageInMageAttributeSet::StaticClass()));
	if (AS)
	{
		UE_LOG(LogRageInMage, Warning, TEXT("Post-Init Attribute Values:"));
		UE_LOG(LogRageInMage, Warning, TEXT("  Strength: %f"), AS->GetStrength());
		UE_LOG(LogRageInMage, Warning, TEXT("  Dexterity: %f"), AS->GetDexterity());
		UE_LOG(LogRageInMage, Warning, TEXT("  Vigor: %f"), AS->GetVigor());
		UE_LOG(LogRageInMage, Warning, TEXT("  Intelligence: %f"), AS->GetIntelligence());	
		UE_LOG(LogRageInMage, Warning, TEXT("  Endurance: %f"), AS->GetEndurance());
		UE_LOG(LogRageInMage, Warning, TEXT("  Wit: %f"), AS->GetWit());
		UE_LOG(LogRageInMage, Warning, TEXT("  Agility: %f"), AS->GetAgility());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CRITICAL: No AttributeSet found after applying effects!"));
	}

	UE_LOG(LogTemp, Warning, TEXT("=== InitializeDefaultAttributes END ==="));
}

void ARageInMageCharacterBase::AddCharacterAbilities()
{
	URageInMageAbilitySystemComponent* RageASC = CastChecked<URageInMageAbilitySystemComponent>(AbilitySystemComponent);
	if (!HasAuthority()) return;

	RageASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);

	// Build the full ability list: CharacterClassInfo abilities (class-specific + common) + Blueprint overrides
	TArray<TSubclassOf<UGameplayAbility>> AllAbilities;

	if (UCharacterClassInfo* ClassInfo = URageInMageAbilitySystemLibrary::GetCharacterClassInfo(this))
	{
		// Add class-specific abilities from the data asset (e.g. Fire Mage gets 6 fire spells)
		const FCharacterClassDefaultInfo& ClassDefaults = ClassInfo->GetCharacterClassDefaultInfo(CharacterClass);
		AllAbilities.Append(ClassDefaults.StartupAbilities);

		// Add common abilities shared by all characters
		AllAbilities.Append(ClassInfo->CommonAbilities);
	}

	// Append any additional abilities set directly on the Blueprint (for testing/overrides),
	// but skip duplicates that are already in the data asset list
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : StartupAbilities)
	{
		if (AbilityClass && !AllAbilities.Contains(AbilityClass))
		{
			AllAbilities.Add(AbilityClass);
		}
	}

	RageASC->AddCharacterAbilities(AllAbilities, CharacterClass);
}

void ARageInMageCharacterBase::Dissolve()
{
	if (IsValid(MeshDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MeshDissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMaterialInstance);
		StartMeshDissolveTimeline(DynamicMaterialInstance);
	}
	if (IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, DynamicMaterialInstance);
		StartWeaponDissolveTimeline(DynamicMaterialInstance);
	}
}

// ── Burning VFX ──

void ARageInMageCharacterBase::BindIgniteStackDelegate()
{
	if (URageInMageAbilitySystemComponent* RageASC = Cast<URageInMageAbilitySystemComponent>(AbilitySystemComponent))
	{
		// Guard against double-bind (PlayerCharacter calls this from both PossessedBy and OnRep_PlayerState)
		if (!RageASC->OnIgniteStackCountChanged.IsAlreadyBound(this, &ARageInMageCharacterBase::OnIgniteStackCountChanged))
		{
			RageASC->OnIgniteStackCountChanged.AddDynamic(this, &ARageInMageCharacterBase::OnIgniteStackCountChanged);
		}
	}
}

// ── Heat Glow ──

void ARageInMageCharacterBase::BindHeatGlowDelegate()
{
	if (!AbilitySystemComponent) return;

	const URageInMageAttributeSet* RageAS = Cast<URageInMageAttributeSet>(AttributeSet);
	if (!RageAS) return;

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		RageAS->GetHeatAttribute()
	).AddUObject(this, &ARageInMageCharacterBase::OnHeatAttributeChanged);
}

// ── Movement Speed ──

void ARageInMageCharacterBase::UpdateMovementSpeed()
{
	const URageInMageAttributeSet* RageAS = GetRageInMageAttributeSet();
	if (!RageAS || !GetCharacterMovement()) return;

	// MovementSpeed is the base/buff multiplier (self-slow, item slows, Momentum live here). Slow is the
	// dedicated "real slow" channel in percent — factored in as Clamp(1 - Slow/100, 0, 1) so Slow=0 leaves
	// speed untouched and Slow>=100 fully stops the character (without ever forcing MaxWalkSpeed negative).
	const float SlowMultiplier = FMath::Clamp(1.f - RageAS->GetSlow() / 100.f, 0.f, 1.f);
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * RageAS->GetMovementSpeed() * SlowMultiplier;
}

void ARageInMageCharacterBase::SetBaseWalkSpeed(float NewBaseWalkSpeed)
{
	BaseWalkSpeed = NewBaseWalkSpeed;
	UpdateMovementSpeed();
}

void ARageInMageCharacterBase::BindMovementSpeedDelegate()
{
	if (!AbilitySystemComponent) return;

	const URageInMageAttributeSet* RageAS = Cast<URageInMageAttributeSet>(AttributeSet);
	if (!RageAS) return;

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		RageAS->GetMovementSpeedAttribute()
	).AddUObject(this, &ARageInMageCharacterBase::OnMovementSpeedAttributeChanged);

	// Slow feeds the same MaxWalkSpeed recompute — reuse the handler (it just calls UpdateMovementSpeed).
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		RageAS->GetSlowAttribute()
	).AddUObject(this, &ARageInMageCharacterBase::OnMovementSpeedAttributeChanged);
}

void ARageInMageCharacterBase::OnMovementSpeedAttributeChanged(const FOnAttributeChangeData& Data)
{
	UpdateMovementSpeed();
}

// TEMP DEBUG: tracking down the "MovementSpeed=0" bug. Shows actual velocity alongside
// MaxWalkSpeed and the raw MovementSpeed GAS attribute, per-actor, so it's possible to see
// whether the attribute itself is zeroing out, whether MaxWalkSpeed isn't following it, or
// whether MaxWalkSpeed is fine but something else (AI/pathing) just isn't moving the actor.
void ARageInMageCharacterBase::LogMovementSpeedDebug()
{
	// Re-enabled to verify the Slow attribute drives MaxWalkSpeed. Comment this body out (and the
	// timer in BeginPlay) to silence it again.
	if (!GEngine) return;

	const URageInMageAttributeSet* RageAS = Cast<URageInMageAttributeSet>(AttributeSet);
	const UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (!RageAS || !MoveComp) return;

	const float CurrentSpeed = GetVelocity().Size();
	const float MaxWalkSpeed = MoveComp->MaxWalkSpeed;
	const float MovementSpeedAttr = RageAS->GetMovementSpeed();
	const float SlowAttr = RageAS->GetSlow();

	// Amber once a slow is actually biting, red if movement has been zeroed outright.
	FColor Color = FColor::Green;
	if (MaxWalkSpeed <= 0.f || MovementSpeedAttr <= 0.f) Color = FColor::Red;
	else if (SlowAttr > 0.f) Color = FColor::Orange;

	const int32 ID = GetUniqueID();
	GEngine->AddOnScreenDebugMessage(ID, 0.6f, Color,
		FString::Printf(TEXT("[Speed] %s | MaxWalkSpeed=%.0f | Vel=%.0f | MoveSpeed=%.2f | Slow=%.0f%%"),
			*GetName(), MaxWalkSpeed, CurrentSpeed, MovementSpeedAttr, SlowAttr));
}

void ARageInMageCharacterBase::CreateHeatGlowDMIs()
{
	if (bHeatGlowDMIsCreated) return;
	bHeatGlowDMIsCreated = true;

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	const int32 NumSlots = MeshComp->GetNumMaterials();
	HeatGlowDMIs.SetNum(NumSlots);

	for (const int32 SlotIndex : HeatGlowMaterialSlots)
	{
		if (SlotIndex < 0 || SlotIndex >= NumSlots) continue;

		UMaterialInterface* BaseMat = MeshComp->GetMaterial(SlotIndex);
		if (!BaseMat) continue;

		UMaterialInstanceDynamic* DMI = UMaterialInstanceDynamic::Create(BaseMat, this);
		if (DMI)
		{
			MeshComp->SetMaterial(SlotIndex, DMI);
			HeatGlowDMIs[SlotIndex] = DMI;
			DMI->SetVectorParameterValue(FName("HeatGlowColor"), FLinearColor::Black);
			DMI->SetScalarParameterValue(FName("HeatGlowIntensity"), 0.f);
		}
	}
}

void ARageInMageCharacterBase::OnHeatAttributeChanged(const FOnAttributeChangeData& Data)
{
	const float HeatNormalized = FMath::Clamp(Data.NewValue / 120.f, -1.f, 1.f);
	const float AbsHeat = FMath::Abs(HeatNormalized);

	FLinearColor GlowColor = FLinearColor::Black;
	float Intensity = 0.f;

	if (!FMath::IsNearlyZero(AbsHeat, 0.01f))
	{
		GlowColor = (HeatNormalized > 0.f) ? HeatGlowWarmColor : HeatGlowColdColor;
		Intensity = FMath::Lerp(HeatGlowMinIntensity, HeatGlowMaxIntensity, AbsHeat);
	}

	if (!bHeatGlowDMIsCreated && !FMath::IsNearlyZero(AbsHeat, 0.01f))
	{
		CreateHeatGlowDMIs();
	}

	for (UMaterialInstanceDynamic* DMI : HeatGlowDMIs)
	{
		if (DMI)
		{
			DMI->SetVectorParameterValue(FName("HeatGlowColor"), GlowColor);
			DMI->SetScalarParameterValue(FName("HeatGlowIntensity"), Intensity);
		}
	}
}

// ── Crowd control ──

FGameplayTagContainer ARageInMageCharacterBase::GetMovementBlockingTags() const
{
	FGameplayTagContainer Blockers = IncapacitationTags;
	Blockers.AppendTags(MovementOnlyBlockTags);
	return Blockers;
}

void ARageInMageCharacterBase::BindCrowdControlDelegates()
{
	URageInMageAbilitySystemComponent* RageASC = Cast<URageInMageAbilitySystemComponent>(AbilitySystemComponent);
	if (!RageASC) return;

	const FRageInMageGameplayTag& ConditionTags = FRageInMageGameplayTag::Get();

	// Lazy defaults straight from the Condition.* tag descriptions. Setting either container in the
	// editor overrides these. (Filled here rather than in the constructor because native tags are not
	// guaranteed registered at CDO construction time - same pattern as UImmovableMassComponent.)
	if (IncapacitationTags.IsEmpty())
	{
		// "unable to Move or make any Action"
		IncapacitationTags.AddTag(ConditionTags.Condition_Stunned);
		IncapacitationTags.AddTag(ConditionTags.Condition_Frozen);
		IncapacitationTags.AddTag(ConditionTags.Condition_Petrified);
		IncapacitationTags.AddTag(ConditionTags.Condition_Grappled);
		IncapacitationTags.AddTag(ConditionTags.Condition_Constricted);
		IncapacitationTags.AddTag(ConditionTags.Condition_Paralysed);
		IncapacitationTags.AddTag(ConditionTags.Condition_Shocked);
	}
	if (MovementOnlyBlockTags.IsEmpty())
	{
		// "unable to Move" - can still act
		MovementOnlyBlockTags.AddTag(ConditionTags.Condition_Rooted);
	}

	for (const FGameplayTag& Tag : GetMovementBlockingTags())
	{
		if (!Tag.IsValid()) continue;
		// Guard against double-bind (PlayerCharacter calls this from both PossessedBy and OnRep_PlayerState)
		if (!RageASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).IsBoundToObject(this))
		{
			RageASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved)
				.AddUObject(this, &ARageInMageCharacterBase::CrowdControlTagChanged);
		}
	}
}

void ARageInMageCharacterBase::CrowdControlTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!Movement || !AbilitySystemComponent) return;

	// Re-evaluate against EVERY movement-blocking tag rather than trusting this one tag's count.
	// Chained CCs deliberately overlap (stun -> petrify -> freeze), so movement must stay locked until
	// the last one clears. The bMovementBlockedByCC latch also means the pre-CC movement mode is
	// captured only on the FIRST lock - otherwise a second CC would save the already-disabled
	// MOVE_None and the character would never regain movement once everything expired.
	const bool bBlocked = AbilitySystemComponent->HasAnyMatchingGameplayTags(GetMovementBlockingTags());

	if (bBlocked && !bMovementBlockedByCC)
	{
		bMovementBlockedByCC = true;
		MovementModeBeforeCC = Movement->MovementMode;
		Movement->DisableMovement();
	}
	else if (!bBlocked && bMovementBlockedByCC)
	{
		bMovementBlockedByCC = false;
		Movement->SetMovementMode(MovementModeBeforeCC);
	}

	// Struggle bookkeeping rides the same tag events (server owns it).
	if (HasAuthority())
	{
		if (NewCount > 0)
		{
			// A newly-applied escapable condition takes over the struggle. Applying a second escapable
			// CC therefore resets progress - chaining Grapple onto a Petrify is a real reset, by design.
			BeginStruggle(CallbackTag);
		}
		else if (CallbackTag == ActiveStruggleTag)
		{
			// The condition we were struggling against ended on its own (expired, or was removed).
			EndStruggle();
		}
	}
}

// ── Struggle (mash to break free) ──

void ARageInMageCharacterBase::BeginStruggle(const FGameplayTag& CondTag)
{
	UConditionInfo* ConditionInfoData = URageInMageAbilitySystemLibrary::GetConditionInfo(this);
	if (!ConditionInfoData) return;

	const FRageInMageConditionInfo Row = ConditionInfoData->FindConditionInfoForTag(CondTag);
	if (!Row.ConditionTag.IsValid() || !Row.bCanStruggleFree) return;

	ActiveStruggleTag = CondTag;
	StruggleProgress = 0.f;
	StruggleRequiredProgress = FMath::Max(Row.StruggleRequiredProgress, 1.f);
	StruggleProgressPerMash = Row.StruggleProgressPerMash;
	StruggleDecayPerSecond = Row.StruggleProgressDecayPerSecond;

	OnStruggleProgressChanged.Broadcast(0.f);

	if (StruggleDecayPerSecond > 0.f && GetWorld())
	{
		constexpr float DecayInterval = 0.1f;
		GetWorldTimerManager().SetTimer(
			StruggleDecayTimerHandle, this, &ARageInMageCharacterBase::TickStruggleDecay, DecayInterval, true);
	}
}

void ARageInMageCharacterBase::EndStruggle()
{
	ActiveStruggleTag = FGameplayTag();
	StruggleProgress = 0.f;

	if (GetWorld())
	{
		GetWorldTimerManager().ClearTimer(StruggleDecayTimerHandle);
	}

	OnStruggleProgressChanged.Broadcast(0.f);
}

void ARageInMageCharacterBase::TickStruggleDecay()
{
	if (!ActiveStruggleTag.IsValid()) return;

	constexpr float DecayInterval = 0.1f;
	StruggleProgress = FMath::Max(0.f, StruggleProgress - StruggleDecayPerSecond * DecayInterval);
	OnStruggleProgressChanged.Broadcast(GetStruggleProgressPercent());
}

void ARageInMageCharacterBase::AddStruggleProgress()
{
	// Owning client mashes locally; the server is the only place progress actually counts.
	if (HasAuthority())
	{
		ServerAddStruggleProgress_Implementation();
	}
	else
	{
		ServerAddStruggleProgress();
	}
}

void ARageInMageCharacterBase::ServerAddStruggleProgress_Implementation()
{
	if (!ActiveStruggleTag.IsValid()) return;

	StruggleProgress += StruggleProgressPerMash;
	OnStruggleProgressChanged.Broadcast(GetStruggleProgressPercent());

	if (StruggleProgress >= StruggleRequiredProgress)
	{
		BreakFreeFromStruggle();
	}
}

void ARageInMageCharacterBase::BreakFreeFromStruggle()
{
	if (!ActiveStruggleTag.IsValid() || !AbilitySystemComponent) return;

	// Strip the condition. Removing the tag re-fires CrowdControlTagChanged, which releases the
	// movement lock (unless another CC still holds it) and clears struggle state via EndStruggle.
	FGameplayTagContainer ToRemove;
	ToRemove.AddTag(ActiveStruggleTag);
	AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(ToRemove);

	EndStruggle();
}

float ARageInMageCharacterBase::GetStruggleProgressPercent() const
{
	if (!ActiveStruggleTag.IsValid() || StruggleRequiredProgress <= 0.f) return 0.f;
	return FMath::Clamp(StruggleProgress / StruggleRequiredProgress, 0.f, 1.f);
}

void ARageInMageCharacterBase::BindHitReactionGraceDelegate()
{
	if (URageInMageAbilitySystemComponent* RageASC = Cast<URageInMageAbilitySystemComponent>(AbilitySystemComponent))
	{
		const FGameplayTag HitReactionTag = FRageInMageGameplayTag::Get().Effects_HitReaction;
		// Guard against double-bind (PlayerCharacter calls this from both PossessedBy and OnRep_PlayerState)
		if (!RageASC->RegisterGameplayTagEvent(HitReactionTag, EGameplayTagEventType::NewOrRemoved).IsBoundToObject(this))
		{
			RageASC->RegisterGameplayTagEvent(HitReactionTag, EGameplayTagEventType::NewOrRemoved)
				.AddUObject(this, &ARageInMageCharacterBase::HitReactionGraceTagChanged);
		}
	}
}

void ARageInMageCharacterBase::HitReactionGraceTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		LastHitReactionEndTime = GetWorld()->GetTimeSeconds();
	}
}

bool ARageInMageCharacterBase::CanTriggerHitReaction() const
{
	return GetWorld()->GetTimeSeconds() - LastHitReactionEndTime >= HitReactionGraceSeconds;
}

void ARageInMageCharacterBase::OnIgniteStackCountChanged(int32 NewStackCount)
{
	if (NewStackCount > 0)
	{
		// Spawn the burning VFX if not already active
		if (!BurningVFXComponent && BurningVFXSystem)
		{
			BurningVFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
				BurningVFXSystem,
				GetMesh(),
				NAME_None,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::KeepRelativeOffset,
				false // bAutoDestroy — we manage the lifetime
			);
		}
		// Update the stack count user parameter for VFX intensity scaling
		if (BurningVFXComponent)
		{
			BurningVFXComponent->SetVariableFloat(FName("StackCount"), static_cast<float>(NewStackCount));
		}
	}
	else
	{
		// All stacks gone — destroy the burning VFX
		if (BurningVFXComponent)
		{
			BurningVFXComponent->DeactivateImmediate();
			BurningVFXComponent->DestroyComponent();
			BurningVFXComponent = nullptr;
		}
	}
}
