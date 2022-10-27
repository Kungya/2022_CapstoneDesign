// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTEnemy_Patrol.generated.h"

/**
 * 
 */
UCLASS()
class FPSPROJECT_API UBTEnemy_Patrol : public UBTTaskNode
{
	GENERATED_BODY()
	

public:
	UBTEnemy_Patrol();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
