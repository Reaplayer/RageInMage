// Copyright Reaplays


#include "Player/RageInMagePlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "RageInMageGameplayTag.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Input/RageInMageEInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "GameFramework/Character.h"
#include "Player/RageInMagePlayerState.h"
#include "UI/HUD/RageInMageHUD.h"
#include "UI/Widget/DamageTextComponent.h"

ARageInMagePlayerController::ARageInMagePlayerController()
{
	bReplicates = true;
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void ARageInMagePlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CursorTrace();
	AutoRun();
}

ARageInMageHUD* ARageInMagePlayerController::GetRageHUD()
{
	if (!RageHUD)
	{
		RageHUD = Cast<ARageInMageHUD>(GetHUD());
	}
	return RageHUD;
}

void ARageInMagePlayerController::TryInitOverlay()
{
	if (bOverlayInitialized) return;
	if (!IsLocalController()) return;

	APawn* LocalPawn = GetPawn();
	if (!LocalPawn) return;

	ARageInMagePlayerState* RagePS = GetPlayerState<ARageInMagePlayerState>();
	if (!RagePS) return;

	UAbilitySystemComponent* ASC = RagePS->GetAbilitySystemComponent();
	if (!ASC) return;

	ARageInMageHUD* HUD = Cast<ARageInMageHUD>(GetHUD());
	if (!HUD) return;

	HUD->InitOverlay(this, RagePS, ASC, RagePS->GetAttributeSet());
	bOverlayInitialized = true;
}

void ARageInMagePlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bIsCriticalHit, bool bIsVulnerableHit, bool bIsResistantHit)
{
	if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, bIsCriticalHit, bIsVulnerableHit, bIsResistantHit);
	}
}

void ARageInMagePlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(
			ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

void ARageInMagePlayerController::CursorTrace()
{
	GetHitResultUnderCursor(ECC_Visibility, false, CursorTraceHit);
	if (!CursorTraceHit.bBlockingHit) return;
	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorTraceHit.GetActor());

	if (LastActor != ThisActor)
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (ThisActor) ThisActor->HighlightActor();
	}
}

void ARageInMagePlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(FRageInMageGameplayTag::Get().InputTag_LMB))
	{
		bTargeting = ThisActor ? true : false;
		bAutoRunning = false;
	}
}

void ARageInMagePlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FRageInMageGameplayTag::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
		return;
	}
	if (!bTargeting || !bShiftKeyDown)
	{
		if (const APawn* ControlledPawn = GetPawn())
		{
			if (FollowTime <= ShortPressThreshold && ControlledPawn)
			{
				if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
					this, ControlledPawn->GetActorLocation(), CachedDestination))
				{
					Spline->ClearSplinePoints();
					for (const FVector& PointLoc : NavPath->PathPoints)
					{
						Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
					}
					if (NavPath->PathPoints.Num() > 0)
					{
						CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
						bAutoRunning = true;
					}
				}
			}
	}
		FollowTime = 0.f;
		bTargeting = false;
	}
}

void ARageInMagePlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FRageInMageGameplayTag::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
		return;
	}
	if (bTargeting || bShiftKeyDown)
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
	}
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();
		if (CursorTraceHit.bBlockingHit) CachedDestination = CursorTraceHit.ImpactPoint;
		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}

URageInMageAbilitySystemComponent* ARageInMagePlayerController::GetASC()
{
	if (MageAbilitySystemComponent == nullptr)
	{
		MageAbilitySystemComponent = Cast<URageInMageAbilitySystemComponent>
		(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return MageAbilitySystemComponent;
}


void ARageInMagePlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(MageContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if(Subsystem)
	{
		Subsystem->AddMappingContext(MageContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);

	// Attempt overlay init — may succeed immediately on listen server,
	// or may need to wait for OnRep_PlayerState on clients.
	TryInitOverlay();
}

void ARageInMagePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	URageInMageEInputComponent* MageEInputComponent = CastChecked<URageInMageEInputComponent>(InputComponent);

	MageEInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARageInMagePlayerController::Move);
	MageEInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &ARageInMagePlayerController::ShiftPressed);
	MageEInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &ARageInMagePlayerController::ShiftReleased);
	MageEInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void ARageInMagePlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}