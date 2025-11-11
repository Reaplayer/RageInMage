// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Damage.generated.h"

/**
 * 
 */
UCLASS()
class RAGEINMAGE_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UExecCalc_Damage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

	static void SetCriticalHit (const FGameplayEffectSpec& Spec, bool bIsCriticalHit);
	static void SetVulnerableHit (const FGameplayEffectSpec& Spec, bool bIsVulnerableHit);
	static void SetResistantHit (const FGameplayEffectSpec& Spec, bool bIsResistantHit);

private:
};
