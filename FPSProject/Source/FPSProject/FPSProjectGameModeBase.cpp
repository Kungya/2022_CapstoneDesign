// Copyright Epic Games, Inc. All Rights Reserved.


#include "FPSProjectGameModeBase.h"
#include "FPSCharacterHUD.h"

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

AFPSProjectGameModeBase::AFPSProjectGameModeBase()
	: Super()
{



	static ConstructorHelpers::FClassFinder<UFPSCharacterHUD> UI_HUD(TEXT("WidgetBlueprint'/Game/UI/WBP_HUD.WBP_HUD_C'"));
	if (UI_HUD.Succeeded())
	{
		HUD_Class = UI_HUD.Class;

		CurrentWidget = CreateWidget(GetWorld(), HUD_Class);
		if (CurrentWidget)
		{
			CurrentWidget->AddToViewport(); // ���ڷ� ������ �����ִ� Zorder�� ����
			//CurrentWidget->RemoveFromViewport();
		}
	}
}
