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
}

UAbilitySystemComponent* AMagePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMagePlayerState::OnRep_Level(int32 OldLevel)
{
	
}
