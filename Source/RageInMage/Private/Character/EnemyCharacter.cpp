// Copyright Reaplays


#include "Character/EnemyCharacter.h"

#include "RageInMageGameplayTags.h"
#include "AI/RageInMageAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystem/RageInMageAbilitySystemComponent.h"
#include  "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RageInMage/RageInMage.h"
#include "UI/Widget/MageUserWidget.h"

AEnemyCharacter::AEnemyCharacter()
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

void AEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority()) return;
	AIController = Cast<ARageInMageAIController>(NewController);
	AIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviourTree->BlackboardAsset);
	AIController->RunBehaviorTree(BehaviourTree);
	AIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
	AIController->GetBlackboardComponent()->SetValueAsBool(FName("IsDead"), bIsDead);
	if (CharacterClass != ECharacterClass::Warrior)
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), true);
	}
}

void AEnemyCharacter::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AEnemyCharacter::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

void AEnemyCharacter::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AEnemyCharacter::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

int32 AEnemyCharacter::GetPlayerLevel()
{
	return Level;
}

void AEnemyCharacter::Die()
{
	SetLifeSpan(LifeSpan);
	Super::Die();
	bIsDead = true;
	if (HasAuthority())
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("IsDead"), bIsDead);
	}
}

void AEnemyCharacter::HitReactionTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed + GetRageInMageAttributeSet()->GetMovementSpeed();
	if (HasAuthority())
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
	}
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	InitPlayerAbilityActorInfo();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed + GetRageInMageAttributeSet()->GetMovementSpeed();
	if (HasAuthority())
	{
		URageInMageAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);
	}

	if (UMageUserWidget* EnemyUserWidget = Cast<UMageUserWidget>(HealthBar->GetUserWidgetObject()))
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
		AbilitySystemComponent->RegisterGameplayTagEvent(FRageInMageGameplayTags::Get().Effects_HitReaction, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&AEnemyCharacter::HitReactionTagChanged
			);

		OnHealthChanged.Broadcast(EnemyAS->GetHealth());
		OnMaxHealthChanged.Broadcast(EnemyAS->GetMaxHealth());
	}
}

void AEnemyCharacter::InitPlayerAbilityActorInfo()
{
	checkf(AbilitySystemComponent, TEXT("ASC is null on %s. Ensure BP parent is AEnemyCharacter and ASC is inherited."), *GetName());
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<URageInMageAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	if (HasAuthority())
	{
		InitializeDefaultAttributes();
	}
}

void AEnemyCharacter::InitializeDefaultAttributes() const
{
	URageInMageAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}