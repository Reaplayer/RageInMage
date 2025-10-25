// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include  "AbilitySystemComponent.h"
#include "RageInMageAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties(){}

	FGameplayEffectContextHandle EffectContextHandle;

	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;
	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;
	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;
	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;
	UPROPERTY()
	AController* SourceController = nullptr;
	UPROPERTY()
	AController* TargetController = nullptr;
	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;
	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
};

template<class T>
using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;

/*
 * 
 */
UCLASS()
class RAGEINMAGE_API URageInMageAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	URageInMageAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributes;
	
	/*
	 * Primary Attributes
	 */
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = "Primary Attributes")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(URageInMageAttributeSet, Strength);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Dexterity, Category = "Primary Attributes")
	FGameplayAttributeData Dexterity;
	ATTRIBUTE_ACCESSORS(URageInMageAttributeSet, Dexterity);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Agility, Category = "Primary Attributes")
	FGameplayAttributeData Agility;
	ATTRIBUTE_ACCESSORS(URageInMageAttributeSet, Agility);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category = "Primary Attributes")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(URageInMageAttributeSet, Intelligence);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Wit, Category = "Primary Attributes")
	FGameplayAttributeData Wit;
	ATTRIBUTE_ACCESSORS(URageInMageAttributeSet, Wit);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Vigor, Category = "Primary Attributes")
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS(URageInMageAttributeSet, Vigor);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Endurance, Category = "Primary Attributes")
	FGameplayAttributeData Endurance;
	ATTRIBUTE_ACCESSORS(URageInMageAttributeSet, Endurance);

	/*
	 * Secondary Attributes
	 */
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalAttack, Category = "Secondary Attributes")
	FGameplayAttributeData PhysicalAttack;
	ATTRIBUTE_ACCESSORS(URageInMageAttributeSet, PhysicalAttack);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicalAttack, Category = "Secondary Attributes")
	FGameplayAttributeData MagicalAttack;
	ATTRIBUTE_ACCESSORS(URageInMageAttributeSet, MagicalAttack);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalChance, Category = "Secondary Attributes")
	FGameplayAttributeData CriticalChance;
	ATTRIBUTE_ACCESSORS(URageInMageAttributeSet, CriticalChance);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalDamage, Category = "Secondary Attributes")
	FGameplayAttributeData CriticalDamage;
	ATTRIBUTE_ACCESSORS(URageInMageAttributeSet, CriticalDamage);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackSpeed, Category = "Secondary Attributes")
	FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS(URageInMageAttributeSet, AttackSpeed);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MovementSpeed, Category = "Secondary Attributes")
	FGameplayAttributeData MovementSpeed;
	ATTRIBUTE_ACCESSORS(URageInMageAttributeSet, MovementSpeed);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalDefence, Category = "Secondary Attributes")
	FGameplayAttributeData PhysicalDefence;
	ATTRIBUTE_ACCESSORS(URageInMageAttributeSet, PhysicalDefence);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicalDefence, Category = "Secondary Attributes")
	FGameplayAttributeData MagicalDefence;
	ATTRIBUTE_ACCESSORS(URageInMageAttributeSet, MagicalDefence);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalDefencePenetration, Category = "Secondary Attributes")
	FGameplayAttributeData PhysicalDefencePenetration;
	ATTRIBUTE_ACCESSORS(URageInMageAttributeSet, PhysicalDefencePenetration);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicalDefencePenetration, Category = "Secondary Attributes")
	FGameplayAttributeData MagicalDefencePenetration;
	ATTRIBUTE_ACCESSORS(URageInMageAttributeSet, MagicalDefencePenetration);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Secondary Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(URageInMageAttributeSet, MaxHealth);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Secondary Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(URageInMageAttributeSet, MaxMana);

	/*
	 * Vital Attributes
	 */
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(URageInMageAttributeSet, Health);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Vital Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(URageInMageAttributeSet, Mana);

	/*
	 * Meta Attributes
	 */

	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(URageInMageAttributeSet, IncomingDamage);

	/*
	 * Primary Attributes
	 */
	
	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldStrength) const;
	UFUNCTION()
	void OnRep_Dexterity(const FGameplayAttributeData& OldDexterity) const;
	UFUNCTION()
	void OnRep_Agility(const FGameplayAttributeData& OldAgility) const;
	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const;
	UFUNCTION()
	void OnRep_Wit(const FGameplayAttributeData& OldWit) const;
	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldVigor) const;
	UFUNCTION()
	void OnRep_Endurance(const FGameplayAttributeData& OldEndurance) const;

	/*
	 * Secondary Attributes
	 */
	
	UFUNCTION()
	void OnRep_PhysicalAttack(const FGameplayAttributeData& OldPhysicalAttack) const;
	UFUNCTION()
	void OnRep_MagicalAttack(const FGameplayAttributeData& OldMagicalAttack) const;
	UFUNCTION()
	void OnRep_CriticalChance(const FGameplayAttributeData& OldCriticalChance) const;
	UFUNCTION()
	void OnRep_CriticalDamage(const FGameplayAttributeData& OldCriticalDamage) const;
	UFUNCTION()
	void OnRep_AttackSpeed(const FGameplayAttributeData& OldAttackSpeed) const;
	UFUNCTION()
	void OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed) const;
	UFUNCTION()
	void OnRep_PhysicalDefence(const FGameplayAttributeData& OldPhysicalDefence) const;
	UFUNCTION()
	void OnRep_MagicalDefence(const FGameplayAttributeData& OldMagicalDefence) const;
	UFUNCTION()
	void OnRep_PhysicalDefencePenetration(const FGameplayAttributeData& OldPhysicalDefencePenetration) const;
	UFUNCTION()
	void OnRep_MagicalDefencePenetration(const FGameplayAttributeData& OldMagicalDefencePenetration) const;
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;

	/*
	 * Vital Attributes
	 */
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;
	
private:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Properties) const;
	void ShowFloatingText(const FEffectProperties& Properties, float Damage) const;
};
