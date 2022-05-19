// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "BaseLevel.generated.h"

/**
 * 
 */
UCLASS()
class FPSPROJECT_API ABaseLevel : public ALevelScriptActor
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
};
