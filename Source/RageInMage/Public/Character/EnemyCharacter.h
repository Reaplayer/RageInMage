// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Character/MageCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "EnemyCharacter.generated.h"

class UWidgetComponent;
/**
 * 
 */
UCLASS()
class RAGEINMAGE_API AEnemyCharacter : public AMageCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
public:
	AEnemyCharacter();

	//** Enemy Interface */
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	//**End Enemy Interface */

	//** Combat Interface */
	virtual int32 GetPlayerLevel() override;
	virtual void Die() override;
	//** End Combat Interface */

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

	void HitReactionTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting = false;

	float BaseWalkSpeed = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 5.f;

	
protected:
	virtual void BeginPlay() override;
	virtual void InitPlayerAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	
};
