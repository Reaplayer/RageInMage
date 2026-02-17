// Copyright Reaplays


#include "Character/RageInMageEnemyCharacter.h"

#include "RageInMageGameplayTag.h"
#include "AI/RageInMageAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include  "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RageInMage/RageInMage.h"
#include "UI/Widget/RageInMageUserWidget.h"

ARageInMageEnemyCharacter::ARageInMageEnemyCharacter()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<URageInMageAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	AttributeSet = CreateDefaultSubobject<URageInMageAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
}

void ARageInMageEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority()) return;
	AIController = Cast<ARageInMageAIController>(NewController);
	
	// Initialize the Blackboard via RunBehaviourTree First
	AIController->RunBehaviorTree(BehaviourTree);
	
	// Initialize Blackboard Values
	AIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
	AIController->GetBlackboardComponent()->SetValueAsBool(FName("IsDead"), bIsDead);
	AIController->GetBlackboardComponent()->SetValueAsInt(FName("SummonCount"), SummonCount);
	AIController->GetBlackboardComponent()->SetValueAsInt(FName("MaxSummonCount"), MaxSummonCount);
	if (bRangedAttacker)
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), true);
	}
	if (bMeleeAndRangedAttacker)
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("MeleeAndRangedAttacker"), true);
	}
}

void ARageInMageEnemyCharacter::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void ARageInMageEnemyCharacter::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

void ARageInMageEnemyCharacter::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* ARageInMageEnemyCharacter::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

int32 ARageInMageEnemyCharacter::GetCharacterLevel_Implementation()
{
	return Level;
}

void ARageInMageEnemyCharacter::Die()
{
	SetLifeSpan(LifeSpan);
	Super::Die();
	bIsDead = true;
	if (HasAuthority())
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("IsDead"), bIsDead);
	}
}

void ARageInMageEnemyCharacter::HitReactionTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed + GetRageInMageAttributeSet()->GetMovementSpeed();
	if (HasAuthority())
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
	}
}

void ARageInMageEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	InitPlayerAbilityActorInfo();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed + GetRageInMageAttributeSet()->GetMovementSpeed();
	if (HasAuthority())
	{
		URageInMageAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);
	}

	if (URageInMageUserWidget* EnemyUserWidget = Cast<URageInMageUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		EnemyUserWidget->SetWidgetController(this);
	}
	
	if (URageInMageAttributeSet* EnemyAS = CastChecked<URageInMageAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(EnemyAS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
			);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(EnemyAS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
			);
		AbilitySystemComponent->RegisterGameplayTagEvent(FRageInMageGameplayTag::Get().Effects_HitReaction, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&ARageInMageEnemyCharacter::HitReactionTagChanged
			);

		OnHealthChanged.Broadcast(EnemyAS->GetHealth());
		OnMaxHealthChanged.Broadcast(EnemyAS->GetMaxHealth());
	}
}

void ARageInMageEnemyCharacter::InitPlayerAbilityActorInfo()
{
	checkf(AbilitySystemComponent, TEXT("ASC is null on %s. Ensure BP parent is ARageInMageEnemyCharacter and ASC is inherited."), *GetName());
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	CastChecked<URageInMageAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	if (HasAuthority())
	{
		InitializeDefaultAttributes();
	}
}

void ARageInMageEnemyCharacter::InitializeDefaultAttributes() const
{
	URageInMageAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}