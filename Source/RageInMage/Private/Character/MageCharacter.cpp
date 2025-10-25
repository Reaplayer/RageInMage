// Copyright Reaplays


#include "Character/MageCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/MagePlayerController.h"
#include "Player/MagePlayerState.h"
#include "UI/HUD/MageHUD.h"

class AMageHUD;

AMageCharacter::AMageCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	AbilitySystemComponent = CreateDefaultSubobject<URageInMageAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

void AMageCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init ability actor info for the server
	InitPlayerAbilityActorInfo();
	AddCharacterAbilities();
}

void AMageCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init Ability actor info for the client
	InitPlayerAbilityActorInfo();
}

int32 AMageCharacter::GetPlayerLevel()
{
	const AMagePlayerState* MagePlayerState = GetPlayerState<AMagePlayerState>();
	check(MagePlayerState);
	return MagePlayerState->GetPlayerLevel();
}

void AMageCharacter::InitPlayerAbilityActorInfo()
{
	AMagePlayerState* MagePlayerState = GetPlayerState<AMagePlayerState>();
	check(MagePlayerState);
	MagePlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(MagePlayerState, this);
	Cast<URageInMageAbilitySystemComponent>(MagePlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = MagePlayerState->GetAbilitySystemComponent();
	AttributeSet = MagePlayerState->GetAttributeSet();

	AMagePlayerController* MagePlayerController = Cast<AMagePlayerController>(GetController());
	if (MagePlayerController)
	{
		AMageHUD* MageHUD = Cast<AMageHUD>(MagePlayerController->GetHUD());
		if (MageHUD)
		{
			MageHUD->InitOverlay(MagePlayerController, MagePlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
	InitializeDefaultAttributes();
}
