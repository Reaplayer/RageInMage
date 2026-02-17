// Copyright Reaplays


#include "Player/RageInMagePlayerState.h"

#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "Inventory/RageInMageInventoryComponent.h"
#include "Net/UnrealNetwork.h"

ARageInMagePlayerState::ARageInMagePlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<URageInMageAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<URageInMageAttributeSet>("AttributeSet");

	// CRITICAL: Register AttributeSet with ASC so GameplayEffects can find it
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AddAttributeSetSubobject(AttributeSet.Get());
		UE_LOG(LogTemp, Warning, TEXT("PlayerState Constructor: Registered AttributeSet %p with ASC"), AttributeSet.Get());
	}

	SetNetUpdateFrequency(100.f);

	InventoryComponent = CreateDefaultSubobject<URageInMageInventoryComponent>("InventoryComponent");
}

void ARageInMagePlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARageInMagePlayerState, Level);
	DOREPLIFETIME(ARageInMagePlayerState, XP);
	DOREPLIFETIME(ARageInMagePlayerState, AttributePoints);
	DOREPLIFETIME(ARageInMagePlayerState, SpellPoints);
}

UAbilitySystemComponent* ARageInMagePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ARageInMagePlayerState::AddToXP(int32 XPToAdd)
{
	XP += XPToAdd;
	OnXPChangedDelegate.Broadcast(XP);
}

void ARageInMagePlayerState::AddToLevel(int32 LevelToAdd)
{
	Level += LevelToAdd;
	OnLevelUpDelegate.Broadcast(Level);
}

void ARageInMagePlayerState::AddToAttributePoints(int32 AttributePointsToAdd)
{
	AttributePoints += AttributePointsToAdd;
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void ARageInMagePlayerState::AddToSpellPoints(int32 SpellPointsToAdd)
{
	SpellPoints += SpellPointsToAdd;
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

void ARageInMagePlayerState::SetXP(int32 NewXP)
{
	XP = NewXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void ARageInMagePlayerState::SetLevel(int32 NewLevel)
{
	Level = NewLevel;
	OnLevelUpDelegate.Broadcast(Level);
}

void ARageInMagePlayerState::SetAttributePoints(int32 NewAttributePoints)
{
	AttributePoints = NewAttributePoints;
	OnAttributePointsChangedDelegate.Broadcast(NewAttributePoints);
}

void ARageInMagePlayerState::SetSpellPoints(int32 NewSpellPoints)
{
	SpellPoints = NewSpellPoints;
	OnSpellPointsChangedDelegate.Broadcast(NewSpellPoints);
}

void ARageInMagePlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelUpDelegate.Broadcast(Level);
}

void ARageInMagePlayerState::OnRep_XP(int32 OldXP)
{
	OnXPChangedDelegate.Broadcast(XP);
}

void ARageInMagePlayerState::OnRep_AttributePoints(int32 OldAttributePoints)
{
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void ARageInMagePlayerState::OnRep_SpellPoints(int32 OldSpellPoints)
{
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}
