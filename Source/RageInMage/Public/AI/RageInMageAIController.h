// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RageInMageAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;

/**
 * 
 */
UCLASS()
class RAGEINMAGE_API ARageInMageAIController : public AAIController
{
	GENERATED_BODY()

public:
	ARageInMageAIController();

protected:
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};
