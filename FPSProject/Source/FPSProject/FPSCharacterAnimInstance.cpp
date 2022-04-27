// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSCharacterAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "FPSCharacter.h"

UFPSCharacterAnimInstance::UFPSCharacterAnimInstance()
{

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

			// ���� �����̽� -> �¿� �ִϸ��̼��� ���� �����Ƿ� TODO : �ִϸ��̼� ���� �߰��� �߰�
			//Vertical = Character->UpDownValue; 
			//Horizontal = Character->LeftRightValue;
		}
	}
}
// AnimInstance ������, TODO : GameInsatnce Ȯ��
