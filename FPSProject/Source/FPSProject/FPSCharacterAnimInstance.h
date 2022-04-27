// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FPSCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class FPSPROJECT_API UFPSCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UFPSCharacterAnimInstance();

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsFalling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float Horizontal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float Vertical;
	
public:
};
