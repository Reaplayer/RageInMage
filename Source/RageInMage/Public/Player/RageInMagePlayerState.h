// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "RageInMagePlayerState.generated.h"

class ULevelUpInfo;
class UAbilitySystemComponent;
class UAttributeSet;
class URageInMageInventoryComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32 NewStat);

/**
 * 
 */
UCLASS()
class RAGEINMAGE_API ARageInMagePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	ARageInMagePlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; };

	UFUNCTION(BlueprintPure, Category = "Inventory")
	URageInMageInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WidgetData")
	TObjectPtr<ULevelUpInfo> LevelUpInfo;
	
	FOnPlayerStatChanged OnXPChangedDelegate;
	FOnPlayerStatChanged OnLevelUpDelegate;
	FOnPlayerStatChanged OnAttributePointsChangedDelegate;
	FOnPlayerStatChanged OnSpellPointsChangedDelegate;

	FORCEINLINE int32 GetPlayerLevel() const { return Level; };
	FORCEINLINE int32 GetPlayerXP() const { return XP; };
	FORCEINLINE int32 GetAttributePoints() const { return AttributePoints; };
	FORCEINLINE int32 GetSpellPoints() const { return SpellPoints; };
	
	UFUNCTION(BlueprintCallable)
	void AddToXP(int32 XPToAdd);
	UFUNCTION(BlueprintCallable)
	void AddToLevel(int32 LevelToAdd);
	UFUNCTION(BlueprintCallable)
	void AddToAttributePoints(int32 AttributePointsToAdd);
	UFUNCTION(BlueprintCallable)
	void AddToSpellPoints(int32 SpellPointsToAdd);
	
	UFUNCTION(BlueprintCallable)
	void SetXP(int32 NewXP);
	UFUNCTION(BlueprintCallable)
	void SetLevel(int32 NewLevel);
	UFUNCTION(BlueprintCallable)
	void SetAttributePoints(int32 NewAttributePoints);
	UFUNCTION(BlueprintCallable)
	void SetSpellPoints(int32 NewSpellPoints);

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<URageInMageInventoryComponent> InventoryComponent;

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level)
	int32 Level = 1;
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_XP)
	int32 XP = 0;
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AttributePoints)
	int32 AttributePoints = 0;
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_SpellPoints)
	int32 SpellPoints = 0;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UFUNCTION()
	void OnRep_XP(int32 OldXP);
	
	UFUNCTION()
	void OnRep_AttributePoints(int32 OldAttributePoints);
	
	UFUNCTION()
	void OnRep_SpellPoints(int32 OldSpellPoints);
};
