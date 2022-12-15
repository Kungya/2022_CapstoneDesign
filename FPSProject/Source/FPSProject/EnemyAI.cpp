// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAI.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

AEnemyAI::AEnemyAI()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BT(TEXT("BehaviorTree'/Game/AI/BT_Enemy.BT_Enemy'"));
	if (BT.Succeeded())
	{
		BehaviorTree = BT.Object;
	}
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BD(TEXT("BlackboardData'/Game/AI/BB_Enemy.BB_Enemy'"));
	if (BD.Succeeded())
	{
		BlackboardData = BD.Object;
	}
}

void AEnemyAI::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	//GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemyAI::RandomMove, 3.f, true);

	if (UseBlackboard(BlackboardData, Blackboard))
	{
		if (RunBehaviorTree(BehaviorTree))
		{

		}
	}


}

void AEnemyAI::OnUnPossess()
{
	Super::OnUnPossess();

	//GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void AEnemyAI::RandomMove()
{
	
	auto CurrentPawn = GetPawn();

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (NavSystem == nullptr)
		return;

	FNavLocation RandomLocation;

	if (NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 500.f, RandomLocation))
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, RandomLocation);
	}
}