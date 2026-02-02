// Copyright Reaplays


#include "Character/RageInMagePlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "NiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/RageInMagePlayerController.h"
#include "Player/RageInMagePlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "UI/HUD/RageInMageHUD.h"

ARageInMagePlayerCharacter::ARageInMagePlayerCharacter()
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

	AbilitySystemComponent = CreateDefaultSubobject<URageInMageAbilitySystemComponent>("RageInMageAbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

void ARageInMagePlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!RagePlayerState)
	{
		RagePlayerState = GetPlayerState<ARageInMagePlayerState>();
	}
	
	// Init ability actor info for the server
	InitPlayerAbilityActorInfo();
	AddCharacterAbilities();
}

void ARageInMagePlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init Ability actor info for the client
	InitPlayerAbilityActorInfo();
}

int32 ARageInMagePlayerCharacter::GetCharacterLevel_Implementation()
{
	check(RagePlayerState);
	return RagePlayerState->GetPlayerLevel();
}

void ARageInMagePlayerCharacter::AddToXP_Implementation(int32 InXP)
{
	check(RagePlayerState);
	RagePlayerState->AddToXP(InXP);
}

void ARageInMagePlayerCharacter::LevelUp_Implementation()
{
	MultiCastLevelUpParticleEffect();
}

void ARageInMagePlayerCharacter::MultiCastLevelUpParticleEffect_Implementation()
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

int32 ARageInMagePlayerCharacter::GetXP_Implementation() const
{
	check(RagePlayerState);
	return RagePlayerState->GetPlayerXP();
	
}

int32 ARageInMagePlayerCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
	check(RagePlayerState);
	return RagePlayerState->LevelUpInfo->FindLevelForXP(InXP);
}

int32 ARageInMagePlayerCharacter::GetAttributePointsReward_Implementation(int32 InLevel) const
{
	check(RagePlayerState);
	return RagePlayerState->LevelUpInfo->LevelUpInfos[InLevel].AttributePointsReward;
}

int32 ARageInMagePlayerCharacter::GetSpellPointsReward_Implementation(int32 InLevel) const
{
	check(RagePlayerState);
	return RagePlayerState->LevelUpInfo->LevelUpInfos[InLevel].SpellPointsReward;
}

void ARageInMagePlayerCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints) const
{
	check(RagePlayerState);
	RagePlayerState->AddToAttributePoints(InAttributePoints);
}

void ARageInMagePlayerCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints) const
{
	check(RagePlayerState);
	RagePlayerState->AddToSpellPoints(InSpellPoints);
}

void ARageInMagePlayerCharacter::AddToPlayerLevel_Implementation(int32 InLevel) const
{
	check(RagePlayerState);
	RagePlayerState->AddToLevel(InLevel);
}

int32 ARageInMagePlayerCharacter::GetAttributePoints_Implementation() const
{
	check(RagePlayerState);
	return RagePlayerState->GetAttributePoints();
}

int32 ARageInMagePlayerCharacter::GetSpellPoints_Implementation() const
{
	check(RagePlayerState);
	return RagePlayerState->GetSpellPoints();
}

void ARageInMagePlayerCharacter::InitPlayerAbilityActorInfo()
{
	check(RagePlayerState);
	RagePlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(RagePlayerState, this);
	AbilitySystemComponent = RagePlayerState->GetAbilitySystemComponent();
	AttributeSet = RagePlayerState->GetAttributeSet();

	ARageInMagePlayerController* MagePlayerController = Cast<ARageInMagePlayerController>(GetController());
	if (MagePlayerController)
	{
		ARageInMageHUD* RageHUD = Cast<ARageInMageHUD>(MagePlayerController->GetHUD());
		if (RageHUD)
		{
			RageHUD->InitOverlay(MagePlayerController, RagePlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
	InitializeDefaultAttributes();
}
