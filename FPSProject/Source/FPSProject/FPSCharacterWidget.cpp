// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacterWidget.h"
#include "FPSCharacterStatComponent.h"
#include "Components/ProgressBar.h"

void UFPSCharacterWidget::BindHp(class UFPSCharacterStatComponent* StatComp)
{
	// 이름을 다르게 한걸로 할 때
	//PB_HpBar123 = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HpBar"));
	CurrentStatComp = StatComp;
	// 람다함수를 연결해줄 수도 있고, 내부적으로 정의된 함수를 연동해주고 싶으면 AddUObject
	StatComp->OnHpChanged.AddUObject(this, &UFPSCharacterWidget::UpdateHp);
	/*StatComp->OnMpChanged.AddUObject(this, &UFPSCharacterWidget::UpdateMp);*/
}

void UFPSCharacterWidget::UpdateHp()
{
	//UE_LOG(LogTemp, Warning, TEXT("UpdateHp"));
	if (CurrentStatComp.IsValid())
		PB_HpBar->SetPercent(CurrentStatComp->GetHpRatio());
}