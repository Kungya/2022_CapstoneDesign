// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSCharacterHUD.generated.h"

/**
 * 
 */
UCLASS()
class FPSPROJECT_API UFPSCharacterHUD : public UUserWidget
{
	GENERATED_BODY()
	
public: // set get�� �޾��ִ°� �Ϲ����̳� �ǽ����� �켱 public ����
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AmmoText;

	// UI�� �����Ϳ� ���� ������ �����Ͱ� �򰥸� �� �־� ���⼭ ȥ���ϸ� �ȵ�

};