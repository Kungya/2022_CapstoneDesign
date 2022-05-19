// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSWeaponAnimInstance.h"
#include "FPSCharacter.h"

UFPSWeaponAnimInstance::UFPSWeaponAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AFM(TEXT("AnimMontage'/Game/Assets/Animations/Rifle_Fire_Montage.Rifle_Fire_Montage'"));
	if (AFM.Succeeded())
	{
		FiringMontage = AFM.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ARM(TEXT("AnimMontage'/Game/Assets/Animations/Rifle_Reload_Montage.Rifle_Reload_Montage'"));
	if (ARM.Succeeded())
	{
		WeaponReloadingMontage = ARM.Object;
	}
}

void UFPSWeaponAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	auto Pawn = TryGetPawnOwner();

	if (IsValid(Pawn))
	{
		
		auto Character = Cast<AFPSCharacter>(Pawn);
		if (Character)
		{
			IsFiring = Character->GetIsRaycasting();
		}
	}
}

void UFPSWeaponAnimInstance::PlayFiringMontage()
{
		
	Montage_Play(FiringMontage, 1.f);
}

void UFPSWeaponAnimInstance::PlayWeaponReloadingMontage()
{
	Montage_Play(WeaponReloadingMontage , 1.f);
}

void UFPSWeaponAnimInstance::AnimNotify_Firing()
{
	OnFiring.Broadcast();
}

void UFPSWeaponAnimInstance::AnimNotify_ReloadingSucceeded()
{
	OnWeaponReloading.Broadcast();
}