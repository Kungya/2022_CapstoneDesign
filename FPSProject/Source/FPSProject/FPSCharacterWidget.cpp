// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacterWidget.h"
#include "FPSCharacterStatComponent.h"
#include "Components/ProgressBar.h"

void UFPSCharacterWidget::BindHp(class UFPSCharacterStatComponent* StatComp)
{
	// �̸��� �ٸ��� �Ѱɷ� �� ��
	//PB_HpBar123 = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HpBar"));
	CurrentStatComp = StatComp;
	// �����Լ��� �������� ���� �ְ�, ���������� ���ǵ� �Լ��� �������ְ� ������ AddUObject
	StatComp->OnHpChanged.AddUObject(this, &UFPSCharacterWidget::UpdateHp);
	/*StatComp->OnMpChanged.AddUObject(this, &UFPSCharacterWidget::UpdateMp);*/
}

void UFPSCharacterWidget::UpdateHp()
{
	//UE_LOG(LogTemp, Warning, TEXT("UpdateHp"));
	if (CurrentStatComp.IsValid())
		PB_HpBar->SetPercent(CurrentStatComp->GetHpRatio());
}