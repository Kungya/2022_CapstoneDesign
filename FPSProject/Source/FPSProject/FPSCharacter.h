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
//
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "Curves/CurveVector.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
//
#include "Components/SceneCaptureComponent2D.h"
#include "FPSCharacter.generated.h" // 이 header는 항상 마지막에 include 해야함

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

	/** mosue X 값을 기반으로 회전하는 컨트롤러 */
	void Turn(float Value);

	/** mosue Y 값을 기반으로 회전하는 컨트롤러 */
	void LookUp(float Value);

	void StartRaycast();

	void Raycast();

	void StopRaycast();

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

	/** bAiming을 true, false로 설정해주기만 하는 함수들 */
	void AimingButtonPressed();
	void AimingButtonReleased();

	void CameraInterpZoom(float DeltaTime);

	void SetLookRates();

	void CalculateCrosshairSpread(float DeltaTime);
	
	void StartCrosshairBulletFire();

	UFUNCTION()
	void FinishCrosshairBulletFire();

	void FireButtonPressed();
	void FireButtonReleased();

	void StartFireTimer();

	UFUNCTION()
	void AutoFireReset();

	/** Line trace for items under the crosshairs */
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);

	/** Trace for items if OverlappedItemCount > 0 */
	void TraceForItems();

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

	/** TurnRate while not aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipTurnRate;

	/** LookUpRate while not aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipLookUpRate;

	/** TurnRate while aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingTurnRate;

	/** LookUpRate while aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingLookUpRate;

	/** Scale factor for mouse sensitivity, TurnRate(X) when not aiming  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipTurnRate;

	/** Scale factor for mouse sensitivity, LookUp(Y) when not aiming  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookUpRate;

	/** Scale factor for mouse sensitivity, TurnRate(X) when aiming  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingTurnRate;

	/** Scale factor for mouse sensitivity, LookUp(Y) when aiming  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingLookUpRate;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	/** Default camera FOV :  */
	float CameraDefaultFOV;

	/** FOV when aiming */
	float CameraZoomedFOV;

	/** Current field of view this frame for interp */
	float CameraCurrentFOV;

	/** Interp speed for zooming when aiming */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	/** Determines the spread of the crosshairs  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;

	/** Velocity component for crosshairs spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;

	/** In air component for crosshairs spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;
	
	/** Aim component for crosshairs spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor;
	
	/** Shooting component for crosshairs spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;

	float ShootTimeDuration;

	bool bFiringBullet;

	FTimerHandle CrosshairShootTimer;

	/** Left mouse button or right console trigger pressed */
	bool bFireButtonPressed;

	/** Ture when we can fire / False when waiting for the timer */
	bool bShouldFire;

	/** 연사속도 */
	float AutomaticFireRate;

	/** 연사 타이머, Sets a timer between gunshots */
	FTimerHandle AutoFireTimer;

	/** Ture if we should trace every frame for items  */
	bool bShouldTraceForItems;

	/** Number of overlapped AItems */
	int8 OverlappedItemCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	class AItem* TraceHitItemLastFrame;

	UPROPERTY()
	TWeakObjectPtr<AActor> HitActorInfo;


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
	
	FORCEINLINE bool GetAiming() const { return bAiming; }

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

	/** Adds/subtracts to/from OverlappedItemCount and updates bShouldTraceForItems */
	void IncrementOverlappedItemCount(int8 Amount);

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

public:
	UFUNCTION()
	void AddAmmo();
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