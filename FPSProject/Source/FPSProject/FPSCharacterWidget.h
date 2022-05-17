// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSCharacterWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPSPROJECT_API UFPSCharacterWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void BindHp(class UFPSCharacterStatComponent* StatComp);

	void UpdateHp();

private:
	TWeakObjectPtr<class UFPSCharacterStatComponent> CurrentStatComp;

	UPROPERTY(meta=(BindWidget))
	// meta=(BindWidget) ������ �˾Ƽ� ���� �̸��� ã����
	class UProgressBar* PB_HpBar;
};