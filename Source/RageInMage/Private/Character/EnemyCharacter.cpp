// Copyright Reaplays


#include "Character/EnemyCharacter.h"

#include "RageInMageGameplayTags.h"
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
	AttributeSet = CreateDefaultSubobject<URageInMageAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
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

int32 AEnemyCharacter::GetPlayerLevel()
{
	return Level;
}

void AEnemyCharacter::Die()
{
	SetLifeSpan(LifeSpan);
	Super::Die();
}

void AEnemyCharacter::HitReactionTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	InitPlayerAbilityActorInfo();
	URageInMageAttributeSet* RInMAS = Cast<URageInMageAttributeSet>(AttributeSet);
	GetCharacterMovement()->MaxWalkSpeed = RInMAS->GetMovementSpeed();
	URageInMageAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent);

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
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<URageInMageAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	InitializeDefaultAttributes();
}

void AEnemyCharacter::InitializeDefaultAttributes() const
{
	URageInMageAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}
