// Copyright Reaplays


#include "Character/RageInMageCharacterBase.h"

#include "RageInMageGameplayTag.h"
#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

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

void ARageInMageCharacterBase::InitPlayerAbilityActorInfo()
{
}

void ARageInMageCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check (IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void ARageInMageCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1);
	ApplyEffectToSelf(DefaultVitalAttributes, 1);
	ApplyEffectToSelf(DefaultResistanceAttributes, 1);
	ApplyEffectToSelf(DefaultItemSpecificAttributes, 1);
}

void ARageInMageCharacterBase::AddCharacterAbilities()
{
	URageInMageAbilitySystemComponent* MageASC = CastChecked<URageInMageAbilitySystemComponent>(AbilitySystemComponent);
	if (!HasAuthority()) return;

	MageASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
	MageASC->AddCharacterAbilities(StartupAbilities);
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
