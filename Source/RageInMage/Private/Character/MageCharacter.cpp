// Copyright Reaplays


#include "Character/MageCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "NiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/MagePlayerController.h"
#include "Player/MagePlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "UI/HUD/MageHUD.h"

class AMageHUD;

AMageCharacter::AMageCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 3000.0f;
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;
	
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>("TopDownCameraComponent");
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;
	
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate = false;
	
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

int32 AMageCharacter::GetCharacterLevel_Implementation()
{
	const AMagePlayerState* MagePlayerState = GetPlayerState<AMagePlayerState>();
	check(MagePlayerState);
	return MagePlayerState->GetPlayerLevel();
}

void AMageCharacter::AddToXP_Implementation(int32 InXP)
{
	AMagePlayerState* MagePlayerState = GetPlayerState<AMagePlayerState>();
	check(MagePlayerState);
	MagePlayerState->AddToXP(InXP);
}

void AMageCharacter::LevelUp_Implementation()
{
	MultiCastLevelUpParticleEffect();
}

void AMageCharacter::MultiCastLevelUpParticleEffect_Implementation()
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		const FVector CameraLocation = TopDownCameraComponent->GetComponentLocation();
		const FVector NiagaraSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const FRotator ToCameraRotation = (CameraLocation - NiagaraSystemLocation).Rotation();
		LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);
		LevelUpNiagaraComponent->Activate(true);
	}
}

int32 AMageCharacter::GetXP_Implementation() const
{
	const AMagePlayerState* MagePlayerState = GetPlayerState<AMagePlayerState>();
	check(MagePlayerState);
	return MagePlayerState->GetPlayerXP();
	
}

int32 AMageCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
	const AMagePlayerState* MagePlayerState = GetPlayerState<AMagePlayerState>();
	check(MagePlayerState);
	return MagePlayerState->LevelUpInfo->FindLevelForXP(InXP);
}

int32 AMageCharacter::GetAttributePointsReward_Implementation(int32 InLevel) const
{
	const AMagePlayerState* MagePlayerState = GetPlayerState<AMagePlayerState>();
	check(MagePlayerState);
	return MagePlayerState->LevelUpInfo->LevelUpInfos[InLevel].AttributePointsReward;
}

int32 AMageCharacter::GetSpellPointsReward_Implementation(int32 InLevel) const
{
	const AMagePlayerState* MagePlayerState = GetPlayerState<AMagePlayerState>();
	check(MagePlayerState);
	return MagePlayerState->LevelUpInfo->LevelUpInfos[InLevel].SpellPointsReward;
}

void AMageCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints) const
{
	IPlayerInterface::AddToAttributePoints_Implementation(InAttributePoints);
}

void AMageCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints) const
{
	IPlayerInterface::AddToSpellPoints_Implementation(InSpellPoints);
}

void AMageCharacter::AddToPlayerLevel_Implementation(int32 InLevel) const
{
	AMagePlayerState* MagePlayerState = GetPlayerState<AMagePlayerState>();
	check(MagePlayerState);
	MagePlayerState->AddToLevel(InLevel);
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
