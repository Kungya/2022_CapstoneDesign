// Copyright Epic Games, Inc. All Rights Reserved.


#include "FPSProjectGameModeBase.h"

void AFPSProjectGameModeBase::StartPlay()
{
	Super::StartPlay();

	if (GEngine)
	{
		// 디버그 메시지를 5초간 표시
		// 첫번째 인수를 -1로 하면 이 메시지를 업데이트할 필요가 없음을 의미
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Hello World, this is FPSGameModeBase !"));
	}


}