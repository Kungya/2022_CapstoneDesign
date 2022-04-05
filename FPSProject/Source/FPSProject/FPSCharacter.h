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
#include "FPSCharacter.generated.h" // �� header�� �׻� �������� include �ؾ���

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

	// ���� �̵� ó��
	UFUNCTION()
	void MoveForward(float Value);
	
	// �¿� �̵� ó��
	UFUNCTION()
	void MoveRight(float Value);

	// Ű�� ������ ���� �÷��� ����
	UFUNCTION()
	void StartJump();

	// Ű�� ���� ���� �÷��� ����
	UFUNCTION()
	void StopJump();

	// Function that handles firing projectiles. �߻�
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;
};
