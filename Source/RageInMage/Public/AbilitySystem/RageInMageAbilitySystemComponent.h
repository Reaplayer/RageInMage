// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "RageInMageAbilitySystemComponent.generated.h"

enum class ECharacterClass : uint8;
struct FRageInMageInventoryItem;
class URageInMageInventoryComponent;
class URageInMageAbilitySystemComponent;
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/)
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven)
DECLARE_DELEGATE_OneParam(FForEachAbilitySpec, FGameplayAbilitySpec& /*AbilitySpec*/)

/** A single ignite stack with independent decay and damage tick timers. */
USTRUCT()
struct FIgniteStack
{
	GENERATED_BODY()

	/** Time remaining before this stack expires and is removed. Counts down from StackDuration. */
	float DecayTimeRemaining = 4.0f;

	/** Timer tracking when the next damage tick fires. Counts down, resets to TickInterval on each tick. */
	float DamageTickTimer = 0.0f;

	/** Damage dealt per tick by this stack. Pre-calculated as DamagePerSecond * DamageTickInterval. */
	float DamagePerTick = 0.0f;

	/** The instigator actor who caused this stack (for damage attribution). */
	TWeakObjectPtr<AActor> Instigator = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIgniteStackCountChanged, int32, NewStackCount);

/**
 * @class URageInMageAbilitySystemComponent
 * @brief A specialized extension of UAbilitySystemComponent, customized for the Rage in Mage game,
 *        which manages abilities and gameplay effect processing for actors.
 *
 * This class adds functionality for handling character abilities, Ability Input tags,
 * and various Gameplay Effects and Ability-related callbacks.
 */
UCLASS()
class RAGEINMAGE_API URageInMageAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AbilityActorInfoSet();

	FEffectAssetTags EffectAssetTags;
	FAbilitiesGiven AbilitiesGivenDelegate;

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities, ECharacterClass CharacterClass);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);
	bool bStartupAbilitiesGiven = false;

	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void ForEachAbilitySpec(const FForEachAbilitySpec& Delegate);
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetAbilityTypeTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetAbilityProgressionTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);

	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);

	void SpendSpellPoint(const FGameplayTag& AbilityTag);

	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag);

	// ── Inventory Server RPCs ──

	void InventoryAddItem(const FRageInMageInventoryItem& Item);
	UFUNCTION(Server, Reliable)
	void ServerInventoryAddItem(const FRageInMageInventoryItem& Item);

	void InventoryRemoveItem(const FGuid& ItemID);
	UFUNCTION(Server, Reliable)
	void ServerInventoryRemoveItem(const FGuid& ItemID);

	void InventoryEquipItem(const FGuid& ItemID);
	UFUNCTION(Server, Reliable)
	void ServerInventoryEquipItem(const FGuid& ItemID);

	void InventoryUnequipItem(const FGuid& ItemID);
	UFUNCTION(Server, Reliable)
	void ServerInventoryUnequipItem(const FGuid& ItemID);

	void InventoryDropItem(const FGuid& ItemID);
	UFUNCTION(Server, Reliable)
	void ServerInventoryDropItem(const FGuid& ItemID);

	void InventoryUseItem(const FGuid& ItemID);
	UFUNCTION(Server, Reliable)
	void ServerInventoryUseItem(const FGuid& ItemID);

	// ── Heat Decay ──

	void StartHeatDecay();
	void StopHeatDecay();

	UPROPERTY(EditDefaultsOnly, Category = "Mechanics|Heat")
	float HeatDecayRate = 4.f;

	// ── Ignite Stack System ──

	/** Add an ignite stack. If at max stacks, refreshes the stack with the lowest remaining decay time.
	 *  @param DamagePerSecond How much fire DPS this single stack deals.
	 *  @param InInstigator The actor who caused ignition (for damage attribution). */
	void AddIgniteStack(float DamagePerSecond, AActor* InInstigator);

	/** Remove all ignite stacks immediately (ice extinguish, death, etc.). */
	void RemoveAllIgniteStacks();

	/** Get current ignite stack count (for VFX scaling). */
	UFUNCTION(BlueprintPure, Category = "Mechanics|Ignite")
	int32 GetIgniteStackCount() const { return IgniteStacks.Num(); }

	/** Broadcast when stack count changes (for VFX). */
	UPROPERTY(BlueprintAssignable, Category = "Mechanics|Ignite")
	FOnIgniteStackCountChanged OnIgniteStackCountChanged;

	/** Max number of ignite stacks a target can have. */
	UPROPERTY(EditDefaultsOnly, Category = "Mechanics|Ignite")
	int32 MaxIgniteStacks = 10;

	/** Duration in seconds for each ignite stack before it expires. */
	UPROPERTY(EditDefaultsOnly, Category = "Mechanics|Ignite")
	float IgniteStackDuration = 4.0f;

	/** Interval in seconds between ignite damage ticks per stack. */
	UPROPERTY(EditDefaultsOnly, Category = "Mechanics|Ignite")
	float IgniteDamageTickInterval = 0.2f;

	/** GE class used for ignite tick damage. Modifier: IncomingDamage, Additive, SetByCaller. */
	UPROPERTY(EditDefaultsOnly, Category = "Mechanics|Ignite")
	TSubclassOf<UGameplayEffect> IgniteDamageEffect;

	/** Default damage per second per ignite stack (used when ignition is triggered by heat threshold). */
	UPROPERTY(EditDefaultsOnly, Category = "Mechanics|Ignite")
	float DefaultIgniteDPS = 5.0f;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	virtual void OnRep_ActivateAbilities() override;

	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec,
		FActiveGameplayEffectHandle ActiveGameplayEffectHandle);

	UFUNCTION()
	void OnAbilitiesGiven(URageInMageAbilitySystemComponent* RageInMageAbilitySystemComponent);

private:
	// ── Heat Decay Internals ──
	FTimerHandle HeatDecayTimerHandle;
	void TickHeatDecay();
	bool bHeatDecayActive = false;

	// ── Ignite Stack Internals ──
	TArray<FIgniteStack> IgniteStacks;

	FTimerHandle IgniteTickTimerHandle;
	bool bIgniteTickActive = false;

	void StartIgniteTick();
	void StopIgniteTick();
	void TickIgniteStacks();

	/** Apply a single damage tick from an ignite stack to self. */
	void ApplyIgniteDamage(float DamageAmount, AActor* InInstigator);

	/** Replicated stack count for clients (VFX). */
	UPROPERTY(ReplicatedUsing = OnRep_IgniteStackCount)
	int32 ReplicatedIgniteStackCount = 0;

	UFUNCTION()
	void OnRep_IgniteStackCount();
};
