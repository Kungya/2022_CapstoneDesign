// Copyright Epic Games, Inc. All Rights Reserved.


#include "FPSProjectGameModeBase.h"

void AFPSProjectGameModeBase::StartPlay()
{
	Super::StartPlay();

	if (GEngine)
	{
		// ����� �޽����� 5�ʰ� ǥ��
		// ù��° �μ��� -1�� �ϸ� �� �޽����� ������Ʈ�� �ʿ䰡 ������ �ǹ�
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Hello World, this is FPSGameModeBase !"));
	}


}