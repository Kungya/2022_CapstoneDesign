// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FPSWeaponAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnFiring);
DECLARE_MULTICAST_DELEGATE(FOnWeaponReloading)
/**
 * 
 */
UCLASS()
class FPSPROJECT_API UFPSWeaponAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UFPSWeaponAnimInstance();

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
public:
	UFUNCTION()
	void PlayFiringMontage();

	UFUNCTION()
	void PlayWeaponReloadingMontage();
private:
	UFUNCTION()
	void AnimNotify_Firing();

	UFUNCTION()
	void AnimNotify_ReloadingSucceeded();

private:
	bool IsFiring;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	UAnimMontage* FiringMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	UAnimMontage* WeaponReloadingMontage;

public:
	FOnFiring OnFiring;
	FOnWeaponReloading OnWeaponReloading;

};
