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
#include "FPSCharacter.generated.h" // 이 header는 항상 마지막에 include 해야함

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

	// 전후 이동 처리
	UFUNCTION()
	void MoveForward(float Value);
	
	// 좌우 이동 처리
	UFUNCTION()
	void MoveRight(float Value);

	UFUNCTION()
	void Sliding();

	UFUNCTION()
	void SlidingTimer();

	// Function that handles firing projectiles. 발사
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
// FPSProejctile Actor 스폰시, OnFire 함수 구현에 있어 두 가지 고려 사항이 있음.
	// 1) 발사체 스폰 위치
	// 2) Projectile class (FPSCharacter와 derived Blueprint가 스폰할 발사체를 알게 하기위해)
	// Gun muzzle offset from the camera location.
	// 카메라 위치로부터 총구가 얼마나 떨어져 있는지 ? (offset)
	/*
	"EditAnywhere" enables you to change the value of the muzzle offset within the Defaults mode
	of the Blueprint Editor or within the Details tab for any instance of the character.

	The "BlueprintReadWrite" specifier enables you to get and
	set the value of the muzzle offset within a Blueprint.
	*/