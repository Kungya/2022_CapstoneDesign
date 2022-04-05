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
#include "FPSCharacter.generated.h" // 이 header는 항상 마지막에 include 해야함

UCLASS()
class FPSPROJECT_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Projectile class to spawn.
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AFPSProjectile> ProjectileClass;

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

	// 키를 누르면 점프 플래그 설정
	UFUNCTION()
	void StartJump();

	// 키를 떼면 점프 플래그 지움
	UFUNCTION()
	void StopJump();

	// Function that handles firing projectiles. 발사
	UFUNCTION()
	void Fire();

	UFUNCTION()
	void Raycast();

	// FPS Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* FPSCameraComponent;

	// First-person mesh (arms), visible only to the owning player.
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FPSMesh;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;
};
