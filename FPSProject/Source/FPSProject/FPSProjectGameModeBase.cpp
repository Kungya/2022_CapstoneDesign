// Copyright Epic Games, Inc. All Rights Reserved.


#include "FPSProjectGameModeBase.h"
#include "FPSCharacterHUD.h"

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
			CurrentWidget->AddToViewport(); // 인자로 순서를 정해주는 Zorder도 있음
			//CurrentWidget->RemoveFromViewport();
		}
	}
}
