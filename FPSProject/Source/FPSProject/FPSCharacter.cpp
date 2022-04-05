// Fill out your copyright notice in the Description page of Project Settings.
#include "FPSProject.h"
#include "FPSCharacter.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// FPS Camera Component ����
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	// null check
	check(FPSCameraComponent != nullptr);

	// Camera Component�� Capsule Component�� ����
	FPSCameraComponent->SetupAttachment(GetCapsuleComponent());

	// ī�޶� ��ġ�� �� ��������
	FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));
	// Pawn�� ī�޶� Rotation ���� ���
	FPSCameraComponent->bUsePawnControlRotation = true;
	
	//-----------------------------------------------------------------------------------------------//
	
	// Create a first person mesh component for the owning player.
	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	check(FPSMesh != nullptr);

	// Only the owning player sees this mesh.
	FPSMesh->SetOnlyOwnerSee(true);

	// Attach the FPS mesh to the FPS Camera.
	FPSMesh->SetupAttachment(FPSCameraComponent);

	// Disable some environmental shadows to preserve the ilusion of having a sisngle mesh.
	FPSMesh->bCastDynamicShadow = false;
	FPSMesh->CastShadow = false;

	// The owning player doesn't see the regular (third-person) body mesh.
	GetMesh()->SetOwnerNoSee(true);
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine)
	{
		// 5�ʰ� ����� �޽��� ǥ��
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using FPSCharacter."));
	}
	
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// "movement" ���ε� ����
	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);

	// "look" ���ε� ����
	PlayerInputComponent->BindAxis("Turn", this, &AFPSCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AFPSCharacter::AddControllerPitchInput);

	// "action" ���ε� ����
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPSCharacter::StopJump);

	// "Fire" ���ε� ����
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::Fire);
	// "RayCast" ���ε� ����
	PlayerInputComponent->BindAction("Raycast", IE_Pressed, this, &AFPSCharacter::Raycast);
}

void AFPSCharacter::MoveForward(float Value)
{
	// ��� ���� 'Forward'���� �˾Ƴ���, �÷��̾ �� �������� �̵��� ���̶�� ���
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void AFPSCharacter::MoveRight(float Value)
{
	// ��� ���� 'Right'���� �˾Ƴ���, �÷��̾ �� �������� �̵��� ���̶�� ���
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void AFPSCharacter::StartJump()
{
	bPressedJump = true;
}

void AFPSCharacter::StopJump()
{
	bPressedJump = false;
}

void AFPSCharacter::Fire()
{
	// Attempt to fire a projectile.
	if (ProjectileClass)
	{
		// Get the camera transform.
		FVector CameraLocation;
		FRotator CameraRotation;
		GetActorEyesViewPoint(CameraLocation, CameraRotation);

		// Set MuzzleOffset to spawn projectiles slightly in front of the camera.
		MuzzleOffset.Set(100.f, 0.f, 0.f);

		// Transform MuzzleOffset from camera space to world space. ****
		FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

		// Skew the aim to be slightly upwards.
		// ������ ���� ���� �÷��� -> �ݵ� ���� ?
		FRotator MuzzleRotation = CameraRotation;
		MuzzleRotation.Pitch += 10.0f;

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this; // �� ���͸� ������ ����(����) : this (character)
			SpawnParams.Instigator = GetInstigator(); // �� �߻�ü�� �������Ѽ� ����� damage�� å���� �ִ� ����?

			// Spawn the projectile at the muzzle.
			// �ѱ����� �߻�ü ����
			AFPSProjectile* Projectile = World->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);

			if (Projectile)
			{
				// Set the projectile's inital trajectory. ���� �߻� ���� (����) ����
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireInDirection(LaunchDirection);
			}
		}
	}
}

void AFPSCharacter::Raycast()
{
	FVector start = FPSCameraComponent->GetComponentLocation();
	FVector forward = FPSCameraComponent->GetForwardVector();
	FVector end = start + forward * 3000;
	FHitResult hit;
	
	if (GetWorld())
	{
		bool actorHit = GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, FCollisionQueryParams(), FCollisionResponseParams());
		DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 2.f, 0.f, 10.f);
		
		if (actorHit && hit.GetActor())
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, hit.GetActor()->GetFName().ToString());
		}

	}
}
