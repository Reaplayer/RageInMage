// Copyright Reaplays


#include "Character/RageInMagePlayerCharacter.h"

#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "AbilitySystem/Components/ImmovableMassComponent.h"
#include "AbilitySystem/Components/MomentumComponent.h"
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

	ImmovableMassComponent = CreateDefaultSubobject<UImmovableMassComponent>("ImmovableMassComponent");
	MomentumComponent = CreateDefaultSubobject<UMomentumComponent>("MomentumComponent");
	
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// NOTE: Do NOT create an ASC here. The authoritative ASC lives on PlayerState.
	// InitPlayerAbilityActorInfo() assigns it from PlayerState at runtime.
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

	// Cache PlayerState for clients (PossessedBy only runs on the server)
	if (!RagePlayerState)
	{
		RagePlayerState = GetPlayerState<ARageInMagePlayerState>();
	}

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
	AbilitySystemComponent = RagePlayerState->GetAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(RagePlayerState, this);
	
	// Wire up the effect-applied callback so UI gets notified of gameplay effects
	CastChecked<URageInMageAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	
	AttributeSet = RagePlayerState->GetAttributeSet();
	// Attribute initialisation is authority-only: the server applies the default-attribute GEs
	// and replicates the resulting values down to clients. Without this guard, OnRep_PlayerState
	// would re-run it on clients (and on any PlayerState re-replication), re-baking the Instant
	// Primary GE and stacking extra copies of the non-stacking Infinite Secondary GE. Mirrors the
	// HasAuthority() gate already used in ARageInMageEnemyCharacter::InitPlayerAbilityActorInfo().
	if (HasAuthority())
	{
		InitializeDefaultAttributes();
	}
	CastChecked<URageInMageAttributeSet>(AttributeSet)->InitialiseTagsToAttributes();

	// Bind ignite stack delegate for burning VFX
	BindIgniteStackDelegate();
	// Bind heat glow delegate for material emissive
	BindHeatGlowDelegate();
	// Bind crowd-control delegates to lock/unlock movement (all incapacitating conditions, not just stun)
	BindCrowdControlDelegates();
	// Bind hit reaction grace delegate to track when reactions end
	BindHitReactionGraceDelegate();
	// Bind movement speed delegate so slow/haste effects update walk speed live
	BindMovementSpeedDelegate();
	UpdateMovementSpeed();

	// Activate the Immovable Mass juggernaut stance for the Earth mage. ActivateStance() is
	// authority-gated internally, so the OnRep_PlayerState path on clients is a safe no-op.
	if (ImmovableMassComponent && CharacterClass == ECharacterClass::EarthMage)
	{
		ImmovableMassComponent->ActivateStance();
	}

	// Activate the Momentum flow-state meter for the Air mage (authority-gated internally).
	if (MomentumComponent && CharacterClass == ECharacterClass::AirMage)
	{
		MomentumComponent->ActivateMomentum();
	}

	// Try to initialize the overlay UI. TryInitOverlay is safe to call repeatedly
	// and handles all cases: server, listen-server, and client (where PC/HUD may not
	// be available yet during OnRep_PlayerState).
	if (ARageInMagePlayerController* MagePlayerController = Cast<ARageInMagePlayerController>(GetController()))
	{
		MagePlayerController->TryInitOverlay();
	}
}
