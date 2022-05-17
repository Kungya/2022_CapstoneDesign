// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
/// raycast header
#include "GameFrameWork/Actor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
///
#include "Camera/CameraComponent.h"
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
#include "FPSCharacter.generated.h" // �� header�� �׻� �������� include �ؾ���

UCLASS()
class FPSPROJECT_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSCharacter();

	///*UFPSRecoil* RecoilSys;*/
	//UPROPERTY()
	//UCurveVector* RecoilCurve;

	//UPROPERTY()
	//UFPSRecoil* Recoil;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	// Projectile class to spawn.
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AFPSProjectile> ProjectileClass;

	//UPROPERTY()
	//TSubclassOf<class UFPSRecoil> RecoilClass;

private:
	UPROPERTY()
	bool SlidingTime = true;

	UPROPERTY()
	class UFPSCharacterAnimInstance* AnimInstance;

	UPROPERTY()
	class UFPSCharacterAnimInstance* AnimInstanceFPP;

	UPROPERTY()
	class UFPSWeaponAnimInstance* WeaponAnimInstance;

	/*UPROPERTY()
	class UAnimSequence* AnimReloading;*/

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ���� �̵� ó��
	UFUNCTION()
	void MoveForward(float Value);
	
	// �¿� �̵� ó��
	UFUNCTION()
	void MoveRight(float Value);

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

	UFUNCTION()
	void ReloadingCheck();

	UFUNCTION()
	void DecreaseHp();

	UFUNCTION()
	void OnReloadingMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnFiringMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// FPS Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* FPSCameraComponent;

	// First-person mesh (arms), visible only to the owning player.
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FPSMesh;

	UPROPERTY()
	class USoundCue* PistolFireWave;

	UPROPERTY(VisibleAnywhere, Category = Mesh)
	USkeletalMeshComponent* FPSWeapon;

	UPROPERTY(VisibleAnywhere)
	class UFPSCharacterStatComponent* Stat;

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* HpBar;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	UPROPERTY()
	FTimerHandle Timer;

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