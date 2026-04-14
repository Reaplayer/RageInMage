// Copyright Reaplays


#include "AbilitySystem/RageInMageAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Abilities/RageInMageGameplayAbility.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "GameFramework/PlayerState.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Inventory/RageInMageInventoryComponent.h"
#include "Inventory/RageInMageInventoryTypes.h"
#include "Net/UnrealNetwork.h"
#include "RageInMage/RageInMageLogChannels.h"
#include "RageInMageGameplayTag.h"

namespace
{
	URageInMageInventoryComponent* GetInventoryComponentFromASC(UAbilitySystemComponent* ASC)
	{
		if (!ASC || !ASC->GetAvatarActor()) return nullptr;
		if (const APawn* Pawn = Cast<APawn>(ASC->GetAvatarActor()))
		{
			if (APlayerState* PS = Pawn->GetPlayerState())
			{
				return PS->FindComponentByClass<URageInMageInventoryComponent>();
			}
		}
		return nullptr;
	}
}

void URageInMageAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &URageInMageAbilitySystemComponent::ClientEffectApplied);
}

void URageInMageAbilitySystemComponent::AddCharacterAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities, ECharacterClass CharacterClass)
{
	const FRageInMageGameplayTag& GameplayTag = FRageInMageGameplayTag::Get();
	const TMap<FGameplayTag, FGameplayTag>* TypeMap = GameplayTag.GetAbilityTypeMapForClass(CharacterClass);

	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		// Check the Ability Class is Valid
		if (!AbilityClass) continue;
		// Create the Ability Spec
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const URageInMageGameplayAbility* MageAbility = Cast<URageInMageGameplayAbility>(AbilitySpec.Ability))
		{
			// Add Input and Ability Tags
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(MageAbility->StartupInputTag);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(MageAbility->StartupAbilityTag);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(FRageInMageGameplayTag::Get().Ability_Progression_Unlocked);

			// Override AbilityTypeTag from the class's school map (single source of truth)
			FGameplayTag AbilityTypeTag = MageAbility->StartupAbilityTypeTag; // fallback to BP default
			if (TypeMap)
			{
				for (const auto& [TypeTag, AbilityTag] : *TypeMap)
				{
					if (AbilityTag.MatchesTagExact(MageAbility->StartupAbilityTag))
					{
						AbilityTypeTag = TypeTag;
						break;
					}
				}
			}
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityTypeTag);

			// Give Ability
			GiveAbility(AbilitySpec);
		}
	}
	bStartupAbilitiesGiven = true;

	// Broadcast Ability Given Event
	AbilitiesGivenDelegate.Broadcast();
}

void URageInMageAbilitySystemComponent::AddCharacterPassiveAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void URageInMageAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	// Return if Input Tag is not valid
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void URageInMageAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

void URageInMageAbilitySystemComponent::ForEachAbilitySpec(const FForEachAbilitySpec& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogRageInMage, Error, TEXT("Failed to execute delegate in %s"), *FString(__FUNCTION__));
		}
	}
}

void URageInMageAbilitySystemComponent::OnAbilitiesGiven(
	URageInMageAbilitySystemComponent* RageInMageAbilitySystemComponent)
{
}

FGameplayTag URageInMageAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags())
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Ability"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag URageInMageAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayTag URageInMageAbilitySystemComponent::GetAbilityTypeTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Ability.Type"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayTag URageInMageAbilitySystemComponent::GetAbilityProgressionTagFromSpec(
	const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Ability.Progression"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

void URageInMageAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			ServerUpgradeAttribute(AttributeTag);
		}
	}
}

void URageInMageAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.0f;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}
}

void URageInMageAbilitySystemComponent::SpendSpellPoint(const FGameplayTag& AbilityTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetSpellPoints(GetAvatarActor()) > 0)
		{
			ServerSpendSpellPoint(AbilityTag);
		}
	}
}

void URageInMageAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetSpellPoints(GetAvatarActor()) > 0)
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}
	}
}

// ── Inventory RPCs ──

void URageInMageAbilitySystemComponent::InventoryAddItem(const FRageInMageInventoryItem& Item)
{
	ServerInventoryAddItem(Item);
}

void URageInMageAbilitySystemComponent::ServerInventoryAddItem_Implementation(const FRageInMageInventoryItem& Item)
{
	URageInMageInventoryComponent* InvComp = GetInventoryComponentFromASC(this);
	if (!InvComp)
	{
		UE_LOG(LogRageInMage, Error, TEXT("ServerInventoryAddItem: No inventory component"));
		return;
	}
	InvComp->ServerAddItem(Item);
}

void URageInMageAbilitySystemComponent::InventoryRemoveItem(const FGuid& ItemID)
{
	ServerInventoryRemoveItem(ItemID);
}

void URageInMageAbilitySystemComponent::ServerInventoryRemoveItem_Implementation(const FGuid& ItemID)
{
	URageInMageInventoryComponent* InvComp = GetInventoryComponentFromASC(this);
	if (!InvComp)
	{
		UE_LOG(LogRageInMage, Error, TEXT("ServerInventoryRemoveItem: No inventory component"));
		return;
	}
	InvComp->ServerRemoveItem(ItemID);
}

void URageInMageAbilitySystemComponent::InventoryEquipItem(const FGuid& ItemID)
{
	ServerInventoryEquipItem(ItemID);
}

void URageInMageAbilitySystemComponent::ServerInventoryEquipItem_Implementation(const FGuid& ItemID)
{
	URageInMageInventoryComponent* InvComp = GetInventoryComponentFromASC(this);
	if (!InvComp)
	{
		UE_LOG(LogRageInMage, Error, TEXT("ServerInventoryEquipItem: No inventory component"));
		return;
	}
	const FRageInMageInventoryItem* Item = InvComp->FindItemByID(ItemID);
	if (!Item || !Item->IsEquippable() || Item->bIsEquipped)
	{
		return;
	}
	InvComp->ServerEquipItem(ItemID, this);
}

void URageInMageAbilitySystemComponent::InventoryUnequipItem(const FGuid& ItemID)
{
	ServerInventoryUnequipItem(ItemID);
}

void URageInMageAbilitySystemComponent::ServerInventoryUnequipItem_Implementation(const FGuid& ItemID)
{
	URageInMageInventoryComponent* InvComp = GetInventoryComponentFromASC(this);
	if (!InvComp)
	{
		UE_LOG(LogRageInMage, Error, TEXT("ServerInventoryUnequipItem: No inventory component"));
		return;
	}
	const FRageInMageInventoryItem* Item = InvComp->FindItemByID(ItemID);
	if (!Item || !Item->bIsEquipped)
	{
		return;
	}
	InvComp->ServerUnequipItem(ItemID, this);
}

void URageInMageAbilitySystemComponent::InventoryDropItem(const FGuid& ItemID)
{
	ServerInventoryDropItem(ItemID);
}

void URageInMageAbilitySystemComponent::ServerInventoryDropItem_Implementation(const FGuid& ItemID)
{
	URageInMageInventoryComponent* InvComp = GetInventoryComponentFromASC(this);
	if (!InvComp)
	{
		UE_LOG(LogRageInMage, Error, TEXT("ServerInventoryDropItem: No inventory component"));
		return;
	}
	if (!InvComp->HasItem(ItemID))
	{
		return;
	}
	InvComp->ServerDropItem(ItemID, this);
}

void URageInMageAbilitySystemComponent::InventoryUseItem(const FGuid& ItemID)
{
	ServerInventoryUseItem(ItemID);
}

void URageInMageAbilitySystemComponent::ServerInventoryUseItem_Implementation(const FGuid& ItemID)
{
	URageInMageInventoryComponent* InvComp = GetInventoryComponentFromASC(this);
	if (!InvComp)
	{
		UE_LOG(LogRageInMage, Error, TEXT("ServerInventoryUseItem: No inventory component"));
		return;
	}
	const FRageInMageInventoryItem* Item = InvComp->FindItemByID(ItemID);
	if (!Item || !Item->IsConsumable())
	{
		return;
	}
	InvComp->ServerUseItem(ItemID, this);
}

void URageInMageAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	
	if (!bStartupAbilitiesGiven)
	{
		AbilitiesGivenDelegate.Broadcast();
		bStartupAbilitiesGiven = true;
	}
}

void URageInMageAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                           const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);
}

void URageInMageAbilitySystemComponent::StartHeatDecay()
{
	if (bHeatDecayActive) return;
	bHeatDecayActive = true;

	if (const AActor* LocalAvatarActor = GetAvatarActor())
	{
		if (UWorld* World = LocalAvatarActor->GetWorld())
		{
			// Tick every 0.25 seconds -> 1 point per tick at 4/sec rate
			World->GetTimerManager().SetTimer(
				HeatDecayTimerHandle,
				this,
				&URageInMageAbilitySystemComponent::TickHeatDecay,
				0.25f,
				true
			);
		}
	}
}

void URageInMageAbilitySystemComponent::StopHeatDecay()
{
	if (!bHeatDecayActive) return;
	bHeatDecayActive = false;

	if (const AActor* LocalAvatarActor = GetAvatarActor())
	{
		if (UWorld* World = LocalAvatarActor->GetWorld())
		{
			World->GetTimerManager().ClearTimer(HeatDecayTimerHandle);
		}
	}
}

void URageInMageAbilitySystemComponent::TickHeatDecay()
{
	const URageInMageAttributeSet* AS = GetSet<URageInMageAttributeSet>();
	if (!AS) return;

	const float CurrentHeat = AS->GetHeat();
	if (FMath::IsNearlyZero(CurrentHeat, 0.5f))
	{
		// Close enough to 0, snap to 0 and stop
		const_cast<URageInMageAttributeSet*>(AS)->SetHeat(0.f);
		StopHeatDecay();
		return;
	}

	// Decay: HeatDecayRate per second, tick every 0.25s = Rate * 0.25 per tick
	const float DecayAmount = HeatDecayRate * 0.25f;
	float NewHeat;
	if (CurrentHeat > 0.f)
	{
		NewHeat = FMath::Max(CurrentHeat - DecayAmount, 0.f);
	}
	else
	{
		NewHeat = FMath::Min(CurrentHeat + DecayAmount, 0.f);
	}

	const_cast<URageInMageAttributeSet*>(AS)->SetHeat(NewHeat);
}

// ── Replication ──

void URageInMageAbilitySystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(URageInMageAbilitySystemComponent, ReplicatedIgniteStackCount);
}

// ── Ignite Stack System ──

void URageInMageAbilitySystemComponent::AddIgniteStack(float DamagePerSecond, AActor* InInstigator)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	if (IgniteStacks.Num() < MaxIgniteStacks)
	{
		// Add a new stack
		FIgniteStack NewStack;
		NewStack.DecayTimeRemaining = IgniteStackDuration;
		NewStack.DamageTickTimer = IgniteDamageTickInterval; // First tick fires after one full interval
		NewStack.DamagePerTick = DamagePerSecond * IgniteDamageTickInterval;
		NewStack.Instigator = InInstigator;
		IgniteStacks.Add(NewStack);
	}
	else
	{
		// At max stacks: refresh the stack with the LOWEST remaining decay time
		// IMPORTANT: Only refresh the decay timer, NOT the damage tick timer
		int32 LowestIndex = 0;
		float LowestTime = IgniteStacks[0].DecayTimeRemaining;
		for (int32 i = 1; i < IgniteStacks.Num(); ++i)
		{
			if (IgniteStacks[i].DecayTimeRemaining < LowestTime)
			{
				LowestTime = IgniteStacks[i].DecayTimeRemaining;
				LowestIndex = i;
			}
		}
		IgniteStacks[LowestIndex].DecayTimeRemaining = IgniteStackDuration;
		// DamageTickTimer is intentionally NOT reset — prevents extending time between ticks
		IgniteStacks[LowestIndex].DamagePerTick = DamagePerSecond * IgniteDamageTickInterval;
		IgniteStacks[LowestIndex].Instigator = InInstigator;
	}

	StartIgniteTick();

	// Update replicated count and broadcast
	ReplicatedIgniteStackCount = IgniteStacks.Num();
	OnIgniteStackCountChanged.Broadcast(ReplicatedIgniteStackCount);

	// Ensure the Condition.Burning tag is on the target
	const FRageInMageGameplayTag& Tags = FRageInMageGameplayTag::Get();
	if (!HasMatchingGameplayTag(Tags.Condition_Burning))
	{
		AddLooseGameplayTag(Tags.Condition_Burning);
	}
}

void URageInMageAbilitySystemComponent::RemoveAllIgniteStacks()
{
	IgniteStacks.Empty();
	StopIgniteTick();

	ReplicatedIgniteStackCount = 0;
	OnIgniteStackCountChanged.Broadcast(0);

	// Remove the Condition.Burning loose tag
	const FRageInMageGameplayTag& Tags = FRageInMageGameplayTag::Get();
	RemoveLooseGameplayTag(Tags.Condition_Burning);

	// Also remove any Burning GEs that may have been applied via ConditionInfo
	FGameplayTagContainer BurningTag;
	BurningTag.AddTag(Tags.Condition_Burning);
	RemoveActiveEffectsWithGrantedTags(BurningTag);
}

void URageInMageAbilitySystemComponent::StartIgniteTick()
{
	if (bIgniteTickActive) return;
	bIgniteTickActive = true;

	if (const AActor* LocalAvatarActor = GetAvatarActor())
	{
		if (UWorld* World = LocalAvatarActor->GetWorld())
		{
			// Tick at 0.05s (50ms) for accurate independent stack processing
			World->GetTimerManager().SetTimer(
				IgniteTickTimerHandle,
				this,
				&URageInMageAbilitySystemComponent::TickIgniteStacks,
				0.05f,
				true
			);
		}
	}
}

void URageInMageAbilitySystemComponent::StopIgniteTick()
{
	if (!bIgniteTickActive) return;
	bIgniteTickActive = false;

	if (const AActor* LocalAvatarActor = GetAvatarActor())
	{
		if (UWorld* World = LocalAvatarActor->GetWorld())
		{
			World->GetTimerManager().ClearTimer(IgniteTickTimerHandle);
		}
	}
}

void URageInMageAbilitySystemComponent::TickIgniteStacks()
{
	if (IgniteStacks.Num() == 0)
	{
		StopIgniteTick();
		return;
	}

	// Early out if the target is dead
	if (const AActor* LocalAvatarActor = GetAvatarActor())
	{
		if (ICombatInterface::Execute_IsDead(LocalAvatarActor))
		{
			RemoveAllIgniteStacks();
			return;
		}
	}

	constexpr float DeltaTime = 0.05f; // Matches timer interval
	bool bStacksRemoved = false;

	// Process stacks in reverse so removal doesn't invalidate indices
	for (int32 i = IgniteStacks.Num() - 1; i >= 0; --i)
	{
		FIgniteStack& Stack = IgniteStacks[i];

		// Decrement decay timer
		Stack.DecayTimeRemaining -= DeltaTime;

		// Check for expiry
		if (Stack.DecayTimeRemaining <= 0.f)
		{
			IgniteStacks.RemoveAtSwap(i);
			bStacksRemoved = true;
			continue;
		}

		// Decrement damage tick timer
		Stack.DamageTickTimer -= DeltaTime;
		if (Stack.DamageTickTimer <= 0.f)
		{
			// Apply damage tick
			ApplyIgniteDamage(Stack.DamagePerTick, Stack.Instigator.Get());
			// Reset tick timer — add interval rather than assign to prevent cumulative drift
			Stack.DamageTickTimer += IgniteDamageTickInterval;
		}
	}

	if (bStacksRemoved)
	{
		ReplicatedIgniteStackCount = IgniteStacks.Num();
		OnIgniteStackCountChanged.Broadcast(ReplicatedIgniteStackCount);

		if (IgniteStacks.Num() == 0)
		{
			StopIgniteTick();

			// Remove Condition.Burning when all stacks have expired
			const FRageInMageGameplayTag& Tags = FRageInMageGameplayTag::Get();
			RemoveLooseGameplayTag(Tags.Condition_Burning);
			FGameplayTagContainer BurningTag;
			BurningTag.AddTag(Tags.Condition_Burning);
			RemoveActiveEffectsWithGrantedTags(BurningTag);
		}
	}
}

void URageInMageAbilitySystemComponent::ApplyIgniteDamage(float DamageAmount, AActor* InInstigator)
{
	if (!IgniteDamageEffect) return;

	// Create context with the original instigator for damage attribution
	FGameplayEffectContextHandle Context = MakeEffectContext();
	if (InInstigator)
	{
		Context.AddInstigator(InInstigator, InInstigator);
	}

	const FGameplayEffectSpecHandle Spec = MakeOutgoingSpec(IgniteDamageEffect, 1, Context);
	if (Spec.IsValid())
	{
		// Set damage via SetByCaller — the GE modifier reads this as IncomingDamage
		const FRageInMageGameplayTag& Tags = FRageInMageGameplayTag::Get();
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
			Spec, Tags.DamageType_MagicalDamage_Fire, DamageAmount);

		ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	}
}

void URageInMageAbilitySystemComponent::OnRep_IgniteStackCount()
{
	OnIgniteStackCountChanged.Broadcast(ReplicatedIgniteStackCount);
}
