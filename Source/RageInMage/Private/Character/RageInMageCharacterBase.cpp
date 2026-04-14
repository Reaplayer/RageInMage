// Copyright Reaplays


#include "Character/RageInMageCharacterBase.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "RageInMageGameplayTag.h"
#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "Components/CapsuleComponent.h"
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
