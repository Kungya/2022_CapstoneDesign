// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSCharacterAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "FPSCharacter.h"

UFPSCharacterAnimInstance::UFPSCharacterAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM(TEXT("AnimMontage'/Game/Assets/Animations/HeroFPP_Skeleton_Montage.HeroFPP_Skeleton_Montage'"));
	if (AM.Succeeded())
	{
		ReloadingMontage = AM.Object;
	}
}

void UFPSCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();

	if (IsValid(Pawn))
	{
		Speed = Pawn->GetVelocity().Size();
		
		auto Character = Cast<AFPSCharacter>(Pawn);
		if (Character)
		{
			IsFalling = Character->GetMovementComponent()->IsFalling();

			// 블렌드 스페이스 -> 좌우 애니메이션이 현재 없으므로 TODO : 애니메이션 에셋 추가시 추가
			//Vertical = Character->UpDownValue; 
			//Horizontal = Character->LeftRightValue;
		}
	}
}
void UFPSCharacterAnimInstance::PlayReloadingMontage()
{
	Montage_Play(ReloadingMontage, 1.f);
}
// AnimInstance 연동끝, TODO : GameInsatnce 확인
void UFPSCharacterAnimInstance::AnimNotify_Reloading()
{
	UE_LOG(LogTemp, Log, TEXT("AnimNotify_Reloading!!"));
	OnReloading.Broadcast();
}