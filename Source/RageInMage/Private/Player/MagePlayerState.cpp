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
}

UAbilitySystemComponent* AMagePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMagePlayerState::AddXP(int32 XPToAdd)
{
	XP += XPToAdd;
	OnXPChangedDelegate.Broadcast(XP);
}

void AMagePlayerState::AddLevel(int32 LevelToAdd)
{
	Level += LevelToAdd;
	OnLevelUpDelegate.Broadcast(Level);
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

void AMagePlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelUpDelegate.Broadcast(Level);
}

void AMagePlayerState::OnRep_XP(int32 OldXP)
{
	OnXPChangedDelegate.Broadcast(XP);
}
