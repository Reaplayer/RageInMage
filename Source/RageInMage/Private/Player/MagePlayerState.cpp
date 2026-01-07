// Copyright Reaplays


#include "Player/MagePlayerState.h"

#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "Net/UnrealNetwork.h"

AMagePlayerState::AMagePlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<URageInMageAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<URageInMageAttributeSet>("AttributeSet");
	
	SetNetUpdateFrequency(100.f);
}

void AMagePlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMagePlayerState, Level);
	DOREPLIFETIME(AMagePlayerState, XP);
	DOREPLIFETIME(AMagePlayerState, AttributePoints);
	DOREPLIFETIME(AMagePlayerState, SpellPoints);
}

UAbilitySystemComponent* AMagePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMagePlayerState::AddToXP(int32 XPToAdd)
{
	XP += XPToAdd;
	OnXPChangedDelegate.Broadcast(XP);
}

void AMagePlayerState::AddToLevel(int32 LevelToAdd)
{
	Level += LevelToAdd;
	OnLevelUpDelegate.Broadcast(Level);
}

void AMagePlayerState::AddToAttributePoints(int32 AttributePointsToAdd)
{
	AttributePoints += AttributePointsToAdd;
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AMagePlayerState::AddToSpellPoints(int32 SpellPointsToAdd)
{
	SpellPoints += SpellPointsToAdd;
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

void AMagePlayerState::SetXP(int32 NewXP)
{
	XP = NewXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void AMagePlayerState::SetLevel(int32 NewLevel)
{
	Level = NewLevel;
	OnLevelUpDelegate.Broadcast(Level);
}

void AMagePlayerState::SetAttributePoints(int32 NewAttributePoints)
{
	AttributePoints = NewAttributePoints;
	OnAttributePointsChangedDelegate.Broadcast(NewAttributePoints);
}

void AMagePlayerState::SetSpellPoints(int32 NewSpellPoints)
{
	SpellPoints = NewSpellPoints;
	OnSpellPointsChangedDelegate.Broadcast(NewSpellPoints);
}

void AMagePlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelUpDelegate.Broadcast(Level);
}

void AMagePlayerState::OnRep_XP(int32 OldXP)
{
	OnXPChangedDelegate.Broadcast(XP);
}

void AMagePlayerState::OnRep_AttributePoints(int32 OldAttributePoints)
{
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AMagePlayerState::OnRep_SpellPoints(int32 OldSpellPoints)
{
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}
