// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Character.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"
#include "RageInMageCharacterBase.generated.h"


class UGameplayEffect;
class UAttributeSet;
class UGameplayAbility;
class UAnimMontage;
class UNiagaraComponent;

UCLASS(Abstract)
class RAGEINMAGE_API ARageInMageCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	// The actual delegate instance. "BlueprintAssignable" allows you to bind to it in BP.
	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnDeathSignature OnDeathDelegate;
	
	ARageInMageCharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
	UAttributeSet* GetAttributeSet() const { return AttributeSet; };
	URageInMageAttributeSet* GetRageInMageAttributeSet() const;

	/** Accessors for combat socket components/names (used by abilities to attach VFX). */
	USkeletalMeshComponent* GetWeapon() const { return Weapon; }
	FName GetWeaponTipSocketName() const { return WeaponTipSocketName; }
	FName GetLeftHandSocketName() const { return LeftHandSocketName; }
	FName GetRightHandSocketName() const { return RightHandSocketName; }
	FName GetTailSocketName() const { return TailSocketName; }

	/* Combat Interface */
	virtual UAnimMontage* GetHitReactionMontage_Implementation() override;
	virtual void Die() override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& SocketTag) override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual ECharacterClass GetCharacterClass_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual FTaggedMontage GetRandomAttackMontage_Implementation(bool bIsRanged, bool bIsSummon) override;
	virtual int32 GetSummonCount_Implementation() override;
	virtual void SetSummonCount_Implementation(int32 NewSummonCount) override;
	virtual int32 GetMaxSummonCount_Implementation() override;
	virtual void SetMaxSummonCount_Implementation(int32 NewMaxSummonCount) override;
	virtual void RegisterSpawnedMinion_Implementation(AActor* Minion) override;
	/* End Combat Interface */

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();

	virtual FOnDeathSignature& GetOnDeathDelegate() override { return OnDeathDelegate; }

	UFUNCTION()
	void OnMinionDeath(AActor* DeadActor);

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UNiagaraSystem* BloodEffect;
	UPROPERTY(EditAnywhere, Category = "Combat")
	USoundBase* DeathSound;
	
	/* Summons */
	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 SummonCount = 0;
	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 MaxSummonCount = 5;
	/* End Summons */

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponTipSocketName;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName LeftHandSocketName;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName RightHandSocketName;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName TailSocketName;

	bool bDead = false;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	virtual void InitPlayerAbilityActorInfo();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultResistanceAttributes;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultItemSpecificAttributes;

	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;
	virtual void InitializeDefaultAttributes() const;

	void AddCharacterAbilities();

	/* Condition VFX */

	/** Niagara system to spawn on the character when burning. Should have a 'StackCount' user param. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions|Burning")
	TObjectPtr<UNiagaraSystem> BurningVFXSystem;

	/** Binds the ignite stack count delegate from the ASC to drive burning VFX. Call after ASC is initialized. */
	void BindIgniteStackDelegate();

	/* Heat Glow VFX — subtle emissive tint driven by Heat attribute */

	/** Emissive intensity at heat near zero. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions|HeatGlow")
	float HeatGlowMinIntensity = 0.f;

	/** Emissive intensity at max heat (±120). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions|HeatGlow")
	float HeatGlowMaxIntensity = 2.0f;

	/** Glow color for positive heat (fire). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions|HeatGlow")
	FLinearColor HeatGlowWarmColor = FLinearColor(1.0f, 0.3f, 0.05f, 1.0f);

	/** Glow color for negative heat (cold). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions|HeatGlow")
	FLinearColor HeatGlowColdColor = FLinearColor(0.1f, 0.4f, 1.0f, 1.0f);

	/** Mesh material slot indices to apply the glow to. Eyes (slot 2) excluded by default. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions|HeatGlow")
	TArray<int32> HeatGlowMaterialSlots = {0, 1, 3, 4, 5};

	/** Binds the Heat attribute change delegate. Call after ASC is initialized. */
	void BindHeatGlowDelegate();

	/* Dissolve Effects */
	void Dissolve();

	UFUNCTION(BlueprintImplementableEvent)
	void StartMeshDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);
	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> MeshDissolveMaterialInstance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

private:
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilities;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactionMontage;

	/* Burning VFX */
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> BurningVFXComponent;

	UFUNCTION()
	void OnIgniteStackCountChanged(int32 NewStackCount);

	/* Heat Glow */
	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> HeatGlowDMIs;
	bool bHeatGlowDMIsCreated = false;
	void CreateHeatGlowDMIs();
	void OnHeatAttributeChanged(const FOnAttributeChangeData& Data);
};
