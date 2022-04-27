// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/DataTable.h"
#include "FPSCharacterGameInstance.generated.h"

USTRUCT()
struct FFPSCharacterData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHp;
};

/**
 * 
 */
UCLASS()
class FPSPROJECT_API UFPSCharacterGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFPSCharacterGameInstance();

	virtual void Init() override;

	FFPSCharacterData* GetStatData(int32 Level);

private:
	UPROPERTY()
	class UDataTable* MyStats;
};


