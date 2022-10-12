// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FPSCharacterAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnReloading);
/**
 * 
 */
UCLASS()
class FPSPROJECT_API UFPSCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UFPSCharacterAnimInstance();

	void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeInitializeAnimation() override;

	/*UFUNCTION()
	void PlayReloadingMontage();*/
private:
	/*UFUNCTION()
	void AnimNotify_Reloading();*/

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class AFPSCharacter* ShooterCharacter;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;

	/* Whether or not the character is moving */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	/* Wheter or not the character is moving */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadingMontage;
	
public:
	FOnReloading OnReloading;
};