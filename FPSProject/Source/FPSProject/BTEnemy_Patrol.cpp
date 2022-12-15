// Fill out your copyright notice in the Description page of Project Settings.


#include "BTEnemy_Patrol.h"
#include "EnemyAI.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTEnemy_Patrol::UBTEnemy_Patrol()
{
	NodeName = TEXT("Patrol");
}

EBTNodeResult::Type UBTEnemy_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto CurrentPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (CurrentPawn == nullptr)
		return EBTNodeResult::Failed;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (NavSystem == nullptr)
		return EBTNodeResult::Failed;

	FNavLocation RandomLocation;

	UE_LOG(LogTemp, Log, TEXT("patrol"));
	if (NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 500.f, RandomLocation))
	{
		UE_LOG(LogTemp, Log, TEXT("patrol2"));
		//UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, RandomLocation);
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(FName(TEXT("PatrolPos")), RandomLocation.Location);
		UE_LOG(LogTemp, Log, TEXT("patrol3"));
		return EBTNodeResult::Succeeded;
	}
	UE_LOG(LogTemp, Log, TEXT("patrol4"))
	return EBTNodeResult::Failed;
}