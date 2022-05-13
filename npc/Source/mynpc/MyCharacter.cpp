// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "MyAnimInstance.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);

	SpringArm->TargetArmLength = 500.f;
	SpringArm->SetRelativeRotation(FRotator(-35.f, 0.f, 0.f));

	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM(TEXT("SkeletalMesh'/Game/AnimalVarietyPack/Fox/Meshes/SK_Fox.SK_Fox'"));

	if (SM.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SM.Object);
	}
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	AnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
	AnimInstance->OnMontageEnded.AddDynamic(this, &AMyCharacter::OnAttackMontageEnded);
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AMyCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AMyCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("Yaw"), this, &AMyCharacter::Yaw);
	PlayerInputComponent->BindAction("Jump",IE_Pressed, this, &AMyCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump",IE_Released, this, &AMyCharacter::StopJump);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AMyCharacter::Attack);
}

void AMyCharacter::Attack()
{
	if (IsAttacking)
		return;

	AnimInstance->PlayAttackMontage();

	IsAttacking = true;
}

void AMyCharacter::UpDown(float Value)
{
	if (Value == 0.f)
		return;

	AddMovementInput(GetActorForwardVector(), Value);
}

void AMyCharacter::LeftRight(float Value)
{
	if (Value == 0.f)
		return;

	AddMovementInput(GetActorRightVector(), Value);
}

void AMyCharacter::StartJump()
{
	bPressedJump = true;
}

void AMyCharacter::StopJump()
{
	bPressedJump = false;
}

void AMyCharacter::Yaw(float Value)
{
	AddControllerYawInput(Value);
}

void AMyCharacter::OnAttackMontageEnded(class UAnimMontage* Montage, bool binterrupted)
{
	IsAttacking = false;
}