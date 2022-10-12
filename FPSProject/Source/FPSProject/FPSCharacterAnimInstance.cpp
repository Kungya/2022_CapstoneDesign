// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSCharacterAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FPSCharacter.h"

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

}

void UFPSCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();

	if (IsValid(Pawn))
	{
		FVector Velocity{ Pawn->GetVelocity() };
		Velocity.Z = 0;
		Speed = Velocity.Size();
		
		auto Character = Cast<AFPSCharacter>(Pawn);
		if (Character)
		{
			// Is the character in the air?
			bIsInAir = Character->GetMovementComponent()->IsFalling();

			// Is the character accelerating?
			if (Character->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
			{
				bIsAccelerating = true;
			}
			else
			{
				bIsAccelerating = false;
			}
			//bIsAccelerating = ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
			
			// 블렌드 스페이스 -> 좌우 애니메이션이 현재 없으므로 TODO : 애니메이션 에셋 추가시 추가
			//Vertical = Character->UpDownValue; 
			//Horizontal = Character->LeftRightValue;
		}
	}
}
void UFPSCharacterAnimInstance::NativeInitializeAnimation()
{
	//ShooterCharacter = Cast<AFPSCharacter>(TryGetPawnOwner());
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