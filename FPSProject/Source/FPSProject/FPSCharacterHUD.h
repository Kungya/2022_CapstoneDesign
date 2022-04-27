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
	
public: // set get을 달아주는게 일반적이나 실습에선 우선 public 설정
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AmmoText;

	// UI의 데이터와 실제 로직의 데이터가 헷갈릴 수 있어 여기서 혼용하면 안됨

};