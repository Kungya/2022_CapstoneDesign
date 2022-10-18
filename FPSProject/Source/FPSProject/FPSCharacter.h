// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
/// raycast header
#include "GameFrameWork/Actor.h"
#include "Engine/World.h"
///
#include "Components/CapsuleComponent.h"
#include "FPSProjectile.h"
#include "Engine/EngineTypes.h" // TimerHandle
#include "FPSRecoil.h"
//
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "Curves/CurveVector.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
//
#include "Components/SceneCaptureComponent2D.h"
#include "FPSCharacter.generated.h" // �� header�� �׻� �������� include �ؾ���

UCLASS()
class FPSPROJECT_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSCharacter();

protected:
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	/* 
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/*
	* Called via input to look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired rate
	*/
	void LookUpAtRate(float Rate);

	// Projectile class to spawn.
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AFPSProjectile> ProjectileClass;



private:
	UPROPERTY()
	bool SlidingTime = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/* in deg/sec. Other scaling may affect final turn rate */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	/* in deg/sec. Other scaling may affect final turn rate */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	/** Randomized gunshot sound cue */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	/** Flash spawned at BarrelSocket */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	/** Montage for firing the weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	/** Particles spawned upon bullet impact */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	/** Smoke trail for bullets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FPSWeapon;

	UPROPERTY()
	class UFPSCharacterAnimInstance* AnimInstanceFPP;

	UPROPERTY()
	class UFPSWeaponAnimInstance* WeaponAnimInstance;
public:
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION()
	void Sliding();

	UFUNCTION()
	void SlidingTimer();

	// Function that handles firing projectiles. �߻�
	UFUNCTION()
	void Fire();

	UFUNCTION()
	void RefreshAmmoUI();

	UFUNCTION()
	void RefreshStatUI();

	UFUNCTION()
	void StartRaycast();

	UFUNCTION()
	void Raycast();

	UFUNCTION()
	void StopRaycast();

	UFUNCTION()
	void Reloading();

	/*UFUNCTION()
	void ReloadingCheck();*/

	UFUNCTION()
	void DecreaseHp();

	UFUNCTION()
	void OnReloadingMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnFiringMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditAnywhere)
	USceneCaptureComponent2D* MiniMapCapture;

	UPROPERTY(VisibleAnywhere)
	class UFPSCharacterStatComponent* Stat;

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* HpBar;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	UPROPERTY()
	FTimerHandle SlidingCooldown;

	UPROPERTY()
	FTimerHandle AutoModeTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn)
	bool IsRaycasting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn)
	bool IsReloading;

	UFUNCTION()
	bool GetIsRaycasting();

	UFUNCTION()
	bool GetIsReloading();
private:

	int32 CurrAmmo = 30;
	int32 MaxAmmo = 30;
	int32 SpareAmmo = 120;
};
// FPSProejctile Actor ������, OnFire �Լ� ������ �־� �� ���� ��� ������ ����.
	// 1) �߻�ü ���� ��ġ
	// 2) Projectile class (FPSCharacter�� derived Blueprint�� ������ �߻�ü�� �˰� �ϱ�����)
	// Gun muzzle offset from the camera location.
	// ī�޶� ��ġ�κ��� �ѱ��� �󸶳� ������ �ִ��� ? (offset)
	/*
	"EditAnywhere" enables you to change the value of the muzzle offset within the Defaults mode
	of the Blueprint Editor or within the Details tab for any instance of the character.

	The "BlueprintReadWrite" specifier enables you to get and
	set the value of the muzzle offset within a Blueprint.
	*/