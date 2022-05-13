// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSRecoil.h"
#include "FPSCharacter.h"


UFPSRecoil::UFPSRecoil()
{
	// caching
	PCRef = UGameplayStatics::GetPlayerController(this, 0);
	
	static ConstructorHelpers::FObjectFinder<UCurveVector> CV(TEXT("CurveVector'/Game/Data/AK47_RecoilCurve.AK47_RecoilCurve'"));
	if (CV.Succeeded())
	{
		RecoilCurve = CV.Object;
		UE_LOG(LogTemp, Warning, TEXT("Succeeded!"));
	}
}

void UFPSRecoil::RecoilStart()
{
	if (RecoilCurve)
	{
		// Set rotators to default values
		PlayerDeltaRot = FRotator(0.f, 0.f, 0.f);
		RecoilDeltaRot = FRotator(0.f, 0.f, 0.f);
		Del = FRotator(0.f, 0.f, 0.f);
		RecoilStartRot = PCRef->GetControlRotation();

		IsFiring = true;

		// Timer for the recoil : I have set it to 10s but dependeding how long it takes to empty the gun mag, you can increase the time.
		GetWorld()->GetTimerManager().SetTimer(FireTimer, this, &UFPSRecoil::RecoilTimerFunction, 10.f, false);

		bRecoil = true;
		bRecoilRecovery = false;
	}
}

void UFPSRecoil::RecoilTimerFunction()
{
	bRecoil = false;
	GetWorld()->GetTimerManager().PauseTimer(FireTimer);
}

void UFPSRecoil::RecoilStop()
{
	IsFiring = false;
}

void UFPSRecoil::RecoveryStart()
{
	if (PCRef->GetControlRotation().Pitch > RecoilStartRot.Pitch)
	{
		bRecoilRecovery = true;
		GetWorld()->GetTimerManager().SetTimer(RecoveryTimer, this, &UFPSRecoil::RecoveryTimerFunction, RecoveryTime, false);
	}
}

void UFPSRecoil::RecoveryTimerFunction()
{
	bRecoilRecovery = false;
}

void UFPSRecoil::RecoilTick(float DeltaTime)
{
	float RecoilTime;
	FVector RecoilVec;
	if (bRecoil)
	{
		// Calculation of control rotation to update

		RecoilTime = GetWorld()->GetTimerManager().GetTimerElapsed(FireTimer);
		RecoilVec = RecoilCurve->GetVectorValue(RecoilTime);
		Del.Roll = 0;
		Del.Pitch = (RecoilVec.Y);
		Del.Yaw = (RecoilVec.Z);
		PlayerDeltaRot = PCRef->GetControlRotation() - RecoilStartRot - RecoilDeltaRot;
		PCRef->SetControlRotation(RecoilStartRot + PlayerDeltaRot + Del);
		RecoilDeltaRot = Del;

		// Con

		if (!IsFiring)
		{
			if (RecoilTime > FireRate)
			{
				GetWorld()->GetTimerManager().ClearTimer(FireTimer);
				RecoilStop();
				bRecoil = false;
				RecoveryStart();
			}
		}

	}
	else if (bRecoilRecovery)
	{
		// Recoil Reset
		FRotator TmpRot = PCRef->GetControlRotation();

		if (TmpRot.Pitch >= RecoilStartRot.Pitch)
		{
			PCRef->SetControlRotation(UKismetMathLibrary::RInterpTo(PCRef->GetControlRotation(), PCRef->GetControlRotation() - RecoilDeltaRot, DeltaTime, 10.f));
			RecoilDeltaRot = RecoilDeltaRot + (PCRef->GetControlRotation() - TmpRot);
		
		}
		else
		{
			RecoveryTimer.Invalidate();
		}
	}
}
