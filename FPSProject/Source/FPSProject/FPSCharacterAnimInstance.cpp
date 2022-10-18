// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSCharacterAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FPSCharacter.h"
#include "Kismet/KismetMathLibrary.h"

UFPSCharacterAnimInstance::UFPSCharacterAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM(TEXT("AnimMontage'/Game/Assets/Animations/HeroFPP_Skeleton_Montage.HeroFPP_Skeleton_Montage'"));
	if (AM.Succeeded())
	{
		ReloadingMontage = AM.Object;
	}
}

void UFPSCharacterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AFPSCharacter>(TryGetPawnOwner());
	}

	if (ShooterCharacter)
	{
		// Get speed of character from velocity except Z axis
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		Velocity.Z = 0;
		Speed = Velocity.Size();

		// Is the character in the air?
		bIsInAir = ShooterCharacter->GetMovementComponent()->IsFalling();
		
		// Is the character accelerating?
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}
		
		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		FRotator MovementRotation =
			UKismetMathLibrary::MakeRotFromX(
				ShooterCharacter->GetVelocity());

		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(
			MovementRotation,
			AimRotation).Yaw;
		
		if (ShooterCharacter->GetVelocity().Size() > 0.f)
		{
			LastMovementOffsetYaw = MovementOffsetYaw;
		}
		/*FString RotationMessage = 
			FString::Printf(
				TEXT("Base Aim Rotation : %f"), 
				AimRotation.Yaw);*/
		/*FString MovmentRotationMessage =
			FString::Printf(
				TEXT("Movmenet Rotation: %f"),
				MovementRotation.Yaw);
		FString OffsetMessage =
			FString::Printf(
				TEXT("Movement Offset Yaw : %f"),
				MovementOffsetYaw);*/

		/*if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Orange, MovementOffsetYaw);
		}*/
	}

}

void UFPSCharacterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AFPSCharacter>(TryGetPawnOwner());
}
//void UFPSCharacterAnimInstance::PlayReloadingMontage()
//{
//	Montage_Play(ReloadingMontage, 1.f);
//}

// AnimInstance 연동끝, TODO : GameInsatnce 확인
//void UFPSCharacterAnimInstance::AnimNotify_Reloading()
//{
//	UE_LOG(LogTemp, Log, TEXT("AnimNotify_Reloading!!"));
//	OnReloading.Broadcast();
//}