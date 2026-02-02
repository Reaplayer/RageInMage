// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Character/RageInMageCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "RageInMageEnemyCharacter.generated.h"

class UWidgetComponent;
class UBehaviorTree;
class ARageInMageAIController;
/**
 * 
 */
UCLASS()
class RAGEINMAGE_API ARageInMageEnemyCharacter : public ARageInMageCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
public:
	ARageInMageEnemyCharacter();
	virtual void PossessedBy(AController* NewController) override;

	//** Enemy Interface */
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	//**End Enemy Interface */

	//** Combat Interface */
	virtual int32 GetCharacterLevel_Implementation() override;
	virtual void Die() override;
	//** End Combat Interface */

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

	void HitReactionTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float BaseWalkSpeed = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 5.f;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;

	
protected:
	virtual void BeginPlay() override;
	virtual void InitPlayerAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviourTree;

	UPROPERTY()
	TObjectPtr<ARageInMageAIController> AIController;

private:
	bool bIsDead = false;
	
	/** Do Not Set MeleeAndRangedAttacker true if this is true! */
	UPROPERTY(EditAnywhere, Category = "Combat")
	bool bRangedAttacker = false;
	
	/** Do Not Set RangedAttacker true if this is true! */
	UPROPERTY(EditAnywhere, Category = "Combat")
	bool bMeleeAndRangedAttacker = false;
};
