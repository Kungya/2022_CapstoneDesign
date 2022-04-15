// Fill out your copyright notice in the Description page of Project Settings.
#include "FPSProject.h"
#include "FPSCharacter.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// FPS Camera Component 생성
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	// null check
	check(FPSCameraComponent != nullptr);

	// Camera Component를 Capsule Component에 붙임
	FPSCameraComponent->SetupAttachment(GetCapsuleComponent());

	// 카메라 위치를 눈 위쪽으로
	FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));
	// Pawn의 카메라 Rotation 제어 허용
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
		// 5초간 디버그 메시지 표시
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

	// "movement" 바인딩 구성
	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);

	// "look" 바인딩 구성
	PlayerInputComponent->BindAxis("Yaw", this, &AFPSCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Pitch", this, &AFPSCharacter::AddControllerPitchInput);

	// "action" 바인딩 구성
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPSCharacter::StopJump);
	PlayerInputComponent->BindAction("Sliding", IE_Pressed, this, &AFPSCharacter::Sliding);

	// "Fire" 바인딩 구성
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::Fire);
	// "RayCast" 바인딩 구성
	PlayerInputComponent->BindAction("Raycast", IE_Pressed, this, &AFPSCharacter::Raycast);
}

void AFPSCharacter::MoveForward(float Value)
{
	if (Value == 0.f)
		return;

	AddMovementInput(GetActorForwardVector(), Value);
}

void AFPSCharacter::MoveRight(float Value)
{
	if (Value == 0.f)
		return;

	AddMovementInput(GetActorRightVector(), Value);
}

void AFPSCharacter::StartJump()
{
	bPressedJump = true;
}

void AFPSCharacter::StopJump()
{
	bPressedJump = false;
}

void AFPSCharacter::Sliding()
{	
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Sliding"));

	float SlidingSpeed = 5000.f;
	LaunchCharacter(GetActorForwardVector() * SlidingSpeed, true, true);
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
		// 머즐 위치를 조금 위로 올려줌 -> 에임보다 높게
		FRotator MuzzleRotation = CameraRotation;
		MuzzleRotation.Pitch += 10.0f;

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this; // 이 액터를 스폰한 액터(주인) : this (character)
			SpawnParams.Instigator = GetInstigator(); // 이 발사체를 스폰시켜서 시행된 damage의 책임이 있는 액터

			// Spawn the projectile at the muzzle.
			// 총구에서 발사체 스폰
			AFPSProjectile* Projectile = World->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);

			if (Projectile)
			{
				// Set the projectile's inital trajectory. 최초 발사 방향 (궤적) 지정
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
