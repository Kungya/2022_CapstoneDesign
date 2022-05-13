// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "Curves/CurveVector.h"
#include "GameFramework/Controller.h"
#include "FPSRecoil.generated.h"

/**
 * 
 */
//class FPSPROJECT_API FPSRecoil
//{
//public:
//	FPSRecoil();
//	~FPSRecoil();
//};

UCLASS(Blueprintable)
class UFPSRecoil : public UObject
{
	GENERATED_BODY()

public:
	UFPSRecoil();

public:
	UPROPERTY(BlueprintReadWrite)
	UCurveVector* RecoilCurve;

	UPROPERTY(BlueprintReadWrite)
	bool bRecoil;

	UPROPERTY(BlueprintReadWrite)
	bool IsFiring;

	UPROPERTY(BlueprintReadWrite)
	bool bRecoilRecovery;

	// Timer Handle
	UPROPERTY(BlueprintReadWrite)
	FTimerHandle FireTimer;
	UPROPERTY(BlueprintReadWrite)
	FTimerHandle RecoveryTimer;

	/* customize how fast recoil resets and what is the max time upto which the recovery can last */
	UPROPERTY(BlueprintReadWrite)
	float RecoveryTime = 1.f;
	UPROPERTY(BlueprintReadWrite)
	float RecoverySpeed = 10.f;

// Rotator
	
	// Control rotation at the start of the recoil
	UPROPERTY()
	FRotator RecoilStartRot;

	// Control rotation change due to recoil
	UPROPERTY()
	FRotator RecoilDeltaRot;

	// Control rotation change due to player moving the mouse **
	UPROPERTY()
	FRotator PlayerDeltaRot;

	// Temporary variable used in tick
	UPROPERTY(BlueprintReadWrite)
	FRotator Del;

// Player controller reference (caching)
	UPROPERTY(BlueprintReadWrite)
	APlayerController* PCRef;

public:
	// Call this function when the firing begins, the recoil starts here
	UFUNCTION(BlueprintCallable)
	void RecoilStart();

	// Function in RecoilStart()
	UFUNCTION()
	void RecoilTimerFunction();

	// Called when firing stops
	UFUNCTION(BlueprintCallable)
	void RecoilStop();

	UPROPERTY(BlueprintReadWrite)
	float FireRate;

	// Function in
	UFUNCTION()
	void RecoveryStart();

	// 
	UFUNCTION()
	void RecoveryTimerFunction();

	UFUNCTION(BlueprintCallable)
	void RecoilTick(float DeltaTime);
};