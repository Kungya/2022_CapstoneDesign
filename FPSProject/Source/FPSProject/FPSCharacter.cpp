// Fill out your copyright notice in the Description page of Project Settings.
#include "FPSCharacter.h"
#include "FPSProject.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "FPSCharacterAnimInstance.h"
#include "FPSWeaponAnimInstance.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "FPSProjectGameModeBase.h"
#include "FPSCharacterHUD.h"
#include "Components/TextBlock.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FPSCharacterStatComponent.h"
#include "Components/WidgetComponent.h"
#include "FPSCharacterWidget.h"
#include "Components/ProgressBar.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
AFPSCharacter::AFPSCharacter() :
	// Base rates turn/look up
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	HipTurnRate(90.f), // while not aiming
	HipLookUpRate(90.f), // while not aiming
	AimingTurnRate(20.f), // while aiming
	AimingLookUpRate(20.f), // while aiming
	// Mouse sensitivity scale factors
	MouseHipTurnRate(1.0f),
	MouseHipLookUpRate(1.0f),
	MouseAimingTurnRate(0.2f),
	MouseAimingLookUpRate(0.2f),
	// when is aiming, true (bool)
	bAiming(false),
	// Camera FOV vaules
	CameraDefaultFOV(0.f), // not good vaule, but set in BeginPlay
	CameraZoomedFOV(35.f),
	CameraCurrentFOV(0.f),
	ZoomInterpSpeed(30.f),
	// Crosshair spread factors
	CrosshairSpreadMultiplier(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimFactor(0.f),
	CrosshairShootingFactor(0.f),
	// Bullet fire timer vaiables
	ShootTimeDuration(0.05f),
	bFiringBullet(false),
	// Automatic fire variables
	// ����ӵ�, 0.1f = �ʴ� 10ȸ, �׸��� CrosshairSpread timer rate���� ������ Ŀ���Ѵ�(0.05 �����)
	AutomaticFireRate(0.1f),
	bShouldFire(true),
	bFireButtonPressed(false)

{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom (pulls in towards the charcater if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 200.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach camera to end of boom
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Don't rotate when the controller rotates. Let the controller only affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Confgure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // ... at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxAcceleration = 1024.f; // �ִ� ����?
	GetCharacterMovement()->BrakingDecelerationWalking = 85.f; // �ް���
	GetCharacterMovement()->GroundFriction = 4.f; // ������� ���ӿ� ����

	//// Create a first person mesh component for the owning player.
	//FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	//check(FPSMesh != nullptr);

	//// Only the owning player sees this mesh.
	//FPSMesh->SetOnlyOwnerSee(true);

	//// Attach the FPS mesh to the FPS Camera.
	//FPSMesh->SetupAttachment(FollowCamera);

	//// Disable some environmental shadows to preserve the ilusion of having a sisngle mesh.
	//FPSMesh->bCastDynamicShadow = false;
	//FPSMesh->CastShadow = false;

	//// 1��Ī SkeletalMesh �߰�
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> FPPSM(TEXT("SkeletalMesh'/Game/Assets/HeroFPP.HeroFPP'"));
	//if (FPPSM.Succeeded())
	//{
	//	FPSMesh->SetSkeletalMesh(FPPSM.Object);
	//}

	// 3��Ī SkeletalMesh �߰�
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM(TEXT("SkeletalMesh'/Game/ParagonLtBelica/Characters/Heroes/Belica/Meshes/Belica.Belica'"));
	if (SM.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SM.Object);
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	}
	
	// FPSMesh�� ���Ͽ� ���� ����
	/*FName WeaponSocket(TEXT("b_RightWeapon_Socket"));
	if (FPSMesh->DoesSocketExist(WeaponSocket))
	{
		FPSWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));

		static ConstructorHelpers::FObjectFinder<USkeletalMesh> SW(TEXT("SkeletalMesh'/Game/MilitaryWeapSilver/Weapons/Assault_Rifle_A.Assault_Rifle_A'"));

		if (SW.Succeeded())
		{
			FPSWeapon->SetSkeletalMesh(SW.Object);
			FPSWeapon->SetupAttachment(FPSMesh, WeaponSocket);
			FPSWeapon->SetOnlyOwnerSee(true);
			FPSWeapon->SetRelativeLocation(FVector(2.f, 10.f, -4.f));
			FPSWeapon->SetRelativeRotation(FRotator(12.f, 0.f, 10.f));
			FPSWeapon->bCastDynamicShadow = false;
			FPSWeapon->CastShadow = false;
		}
	}*/

	Stat = CreateDefaultSubobject<UFPSCharacterStatComponent>(TEXT("STAT"));

	//HpBar Component ����
	HpBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBAR"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
	HpBar->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FClassFinder<UUserWidget> UW(TEXT("WidgetBlueprint'/Game/UI/WBP_HpBar.WBP_HpBar_C'"));
	if (UW.Succeeded())
	{
		HpBar->SetWidgetClass(UW.Class);
		HpBar->SetDrawSize(FVector2D(200.f, 50.f));
	}
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	// BeginPlay ���� FollowCamera�� �����ǹǷ� ���⼭ ����
	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}

	RefreshAmmoUI();
	RefreshStatUI();
}

void AFPSCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//AnimInstanceFPP = Cast<UFPSCharacterAnimInstance>(FPSMesh->GetAnimInstance());
	//if (AnimInstanceFPP)
	//{// ��������Ʈ ���ε� - Reloading
	//	AnimInstanceFPP->OnMontageEnded.AddDynamic(this, &AFPSCharacter::OnReloadingMontageEnded);
	//	//AnimInstanceFPP->OnReloading.AddUObject(this, &AFPSCharacter::ReloadingCheck);
	//	/*-> źâ�� �ٽ� ������ ������ ������ ����� �����ϴ°� �����ϴٰ� �����Ͽ�
	//	Weapon �� Notify�� ������ ����� �����߱� ������ ���� ��� ����,*/
	//}

	/*WeaponAnimInstance = Cast<UFPSWeaponAnimInstance>(FPSWeapon->GetAnimInstance());
	if (WeaponAnimInstance)
	{
		WeaponAnimInstance->OnMontageEnded.AddDynamic(this, &AFPSCharacter::OnFiringMontageEnded);
		WeaponAnimInstance->OnFiring.AddUObject(this, &AFPSCharacter::Raycast);
		WeaponAnimInstance->OnWeaponReloading.AddUObject(this, &AFPSCharacter::ReloadingCheck);
	}*/

	HpBar->InitWidget();

	// TODO : HpBar Binding, Delegate
	auto HpWidget = Cast<UFPSCharacterWidget>(HpBar->GetUserWidgetObject());
	if (HpWidget)
		HpWidget->BindHp(Stat);
	// TODO : 2DHpBar, 2DMpBar Binding, Delegate
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// set FOV with Interpolation when aiming
	CameraInterpZoom(DeltaTime);

	// Tick ���� bAiming ���� ���� Base Rate ���� �ٲ��ִ� ���� (����)
	SetLookRates();
	
	// Calculate crosshair spread multiplier
	CalculateCrosshairSpread(DeltaTime);
}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFPSCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFPSCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("Turn", this, &AFPSCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AFPSCharacter::LookUp);

	// "action" ���ε� ����
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::StartJump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPSCharacter::StopJump);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	PlayerInputComponent->BindAction("Sliding", IE_Pressed, this, &AFPSCharacter::Sliding);

	// "Fire" ���ε� ����
	PlayerInputComponent->BindAction("FireProjectile", IE_Pressed, this, &AFPSCharacter::Fire);
	// "RayCast" ���ε� ����
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AFPSCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AFPSCharacter::FireButtonReleased);
	// Aiming
	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AFPSCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AFPSCharacter::AimingButtonReleased);

	//PlayerInputComponent->BindAction("Raycast", IE_Pressed, this, &AFPSCharacter::StartRaycast);
	//PlayerInputComponent->BindAction("Raycast", IE_Released, this, &AFPSCharacter::StopRaycast);
	// "Relodaing" ���ε�
	//PlayerInputComponent->BindAction("Reloading", IE_Pressed, this, &AFPSCharacter::Reloading);
	// �����, HP����
	PlayerInputComponent->BindAction("DecreaseHp", IE_Pressed, this, &AFPSCharacter::DecreaseHp);

}

void AFPSCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.f))
	{
		// find out which way is forward
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
	else
	{
		return;
	}
}

void AFPSCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.f))
	{
		// find out which way is right
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value);
	}
	else
	{
		return;
	}
}

void AFPSCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds()); // deg/sec * sec/frame
}

void AFPSCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AFPSCharacter::Turn(float Value)
{
	float TurnScaleFactor{};
	if (bAiming)
	{
		TurnScaleFactor = MouseAimingTurnRate;
	}
	else
	{
		TurnScaleFactor = MouseHipTurnRate;
	}
	AddControllerYawInput(Value * TurnScaleFactor);
}

void AFPSCharacter::LookUp(float Value)
{
	float LookUpScaleFactor{};
	if (bAiming)
	{
		LookUpScaleFactor = MouseAimingLookUpRate;
	}
	else
	{
		LookUpScaleFactor = MouseHipLookUpRate;
	}
	AddControllerPitchInput(Value * LookUpScaleFactor);
}

void AFPSCharacter::StartRaycast()
{
	IsRaycasting = true;

	Raycast();
}

void AFPSCharacter::Raycast()
{
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");

	if (BarrelSocket)
	{
		// �ѱ� ��ġ
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(
			SocketTransform.GetLocation(), BeamEnd);

		FPointDamageEvent PointDamageEvent;
		if (HitActorInfo.IsValid())
		{
			HitActorInfo->TakeDamage(Stat->GetAttack(), PointDamageEvent, GetController(), this);
		}

		if (bBeamEnd)
		{
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					BeamEnd);
			}

			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				BeamParticles,
				SocketTransform);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}

	// Start bullet fire timer for crosshairs
	StartCrosshairBulletFire();

	/*if (CurrAmmo <= 0 || !IsRaycasting || IsReloading)
		return;

	WeaponAnimInstance->PlayFiringMontage();

	--CurrAmmo;
	RefreshAmmoUI();


	FVector start = FollowCamera->GetComponentLocation();
	FVector forward = FollowCamera->GetForwardVector();
	FVector end = start + forward * 3000;
	FHitResult HitResult;*/
	//
	//if (GetWorld())
	//{
	//	bool RayCastResult = GetWorld()->LineTraceSingleByChannel(
	//		OUT HitResult,
	//		start,
	//		end,
	//		ECollisionChannel::ECC_Visibility, // TODO : ä���� ���Ŀ� Visibility �̿ܿ� �ٸ� ������ ����
	//		FCollisionQueryParams(),
	//		FCollisionResponseParams()
	//	);
	//	//DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 2.f, 0.f, 1.f);
	//	
	//	if (RayCastResult && HitResult.Actor.IsValid())
	//	{
	//		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, HitResult.GetActor()->GetFName().ToString());
	//		FPointDamageEvent PointDamageEvent;
	//		// this : ���� �����ϴ°Ŵϱ�
	//		HitResult.Actor->TakeDamage(Stat->GetAttack(), PointDamageEvent, GetController(), this);
	//	}
	//}

	//AddControllerPitchInput(FMath::RandRange(0.3f, 0.6f) * -1);
	//AddControllerYawInput(FMath::RandRange(-0.1f, 0.1f));

	//GetWorldTimerManager().SetTimer(AutoModeTimer, this, &AFPSCharacter::Raycast, .12f, false);
}

void AFPSCharacter::StopRaycast()
{
	IsRaycasting = false;
}

bool AFPSCharacter::GetBeamEndLocation(
	const FVector& MuzzleSocketLocation,
	FVector& OutBeamLocation)
{
	// Gut current size of the viewport
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// Get screen space location of crosshairs
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	//CrosshairLocation.Y -= 50.f;
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld) // deprojection �����������
	{
		FHitResult ScreenTraceHit;
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ CrosshairWorldPosition + CrosshairWorldDirection * 50'000.f };

		/* BeamEndPoint�� �̸� End�� �����س���, ���� �浹�� ��ü�� �ִٸ� End�� �ٲٰ�, �ƴϸ�
		 End���� Beam ȿ���� ���ڴٴ� �� */
		OutBeamLocation = End;

		// Trace from corsshairs world location
		GetWorld()->LineTraceSingleByChannel(
			ScreenTraceHit,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);

		if (ScreenTraceHit.bBlockingHit) // if trace hit
		{
			// �浹�� ��ü�� ���� ���, BeamEndPoint�� End���� �浹�� ��ü�� Location���� ����
			OutBeamLocation = ScreenTraceHit.Location;
			HitActorInfo = ScreenTraceHit.Actor;
		}

		// Second trace, from the [ gun barrel ]
		FHitResult WeaponTraceHit;
		const FVector WeaponTraceStart{ MuzzleSocketLocation };
		const FVector WeaponTraceEnd{ OutBeamLocation };
		GetWorld()->LineTraceSingleByChannel(
			WeaponTraceHit,
			WeaponTraceStart,
			WeaponTraceEnd,
			ECollisionChannel::ECC_Visibility);
		if (WeaponTraceHit.bBlockingHit)
		{
			OutBeamLocation = WeaponTraceHit.Location;
			HitActorInfo = WeaponTraceHit.Actor;
		}
		FPointDamageEvent PointDamageEvent;
		

		return true;
	}

	return false;
}

void AFPSCharacter::AimingButtonPressed()
{
	bAiming = true;
}

void AFPSCharacter::AimingButtonReleased()
{
	bAiming = false;
}

void AFPSCharacter::CameraInterpZoom(float DeltaTime)
{
	// Set FOV
	if (bAiming)
	{
		// Interp FOV current -> zoomed
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraZoomedFOV,
			DeltaTime,
			ZoomInterpSpeed);
	}
	else
	{
		// Interp FOV current -> default
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraDefaultFOV,
			DeltaTime,
			ZoomInterpSpeed);
	}
	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
}

void AFPSCharacter::SetLookRates()
{
	if (bAiming)
	{
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AimingLookUpRate;
	}
	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}
}

void AFPSCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	FVector2D WalkSpreadRange{ 0.f, 600.f };
	FVector2D VelocityMultiplierRange{ 0.f, 1.f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(
		WalkSpreadRange, 
		VelocityMultiplierRange,
		Velocity.Size());

	// Calculate crosshair in air factor
	if (GetCharacterMovement()->IsFalling())
	{
		// Spread the crosshairs slowly while in air
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else
	{
		// Shrink the crosshairs rapidly while on the ground
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}

	// Calcuate crosshair aim factor
	if (bAiming)
	{
		// Shrink
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.6f, DeltaTime, 30.f);
	}
	else
	{
		// Spread
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
	}

	// True during 0.05 second (ShootTimeDuration) after firing
	if (bFiringBullet)
	{
		// Spread
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.3f, DeltaTime, 60.f);
	}
	else
	{
		// Shrink
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 60.f);
	}

	CrosshairSpreadMultiplier = 
		0.5f + 
		CrosshairVelocityFactor + 
		CrosshairInAirFactor - 
		CrosshairAimFactor +
		CrosshairShootingFactor;
}

void AFPSCharacter::StartCrosshairBulletFire()
{
	bFiringBullet = true;

	GetWorldTimerManager().SetTimer(
		CrosshairShootTimer, 
		this, 
		&AFPSCharacter::FinishCrosshairBulletFire, 
		ShootTimeDuration);
}

void AFPSCharacter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
}

void AFPSCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;

	StartFireTimer();
}

void AFPSCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AFPSCharacter::StartFireTimer()
{
	if (bShouldFire)
	{
		Raycast();
		bShouldFire = false;

		GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AFPSCharacter::AutoFireReset, AutomaticFireRate);
	}
}

void AFPSCharacter::AutoFireReset()
{
	bShouldFire = true;

	if (bFireButtonPressed)
	{
		StartFireTimer();
	}
}

float AFPSCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

void AFPSCharacter::Sliding()
{	
	if (!SlidingTime)
		return;

	// Mp �Ҹ� : 20, Mp�� �����ϸ� 1�� �����Ͽ� ��ų ����
	if (Stat->OnSkill(20))
		return;

	RefreshStatUI();

	SlidingTime = false;

	float SlidingSpeed = 5000.f;

	if (GetMovementComponent()->IsFalling()) // ���߿����� ������ ���� �ӵ� ����
		SlidingSpeed = 2000.f;

	if (GetLastMovementInputVector() == FVector::ZeroVector)
	{
		LaunchCharacter(GetActorForwardVector() * SlidingSpeed, true, true);
	}
	else
	{
		LaunchCharacter(GetLastMovementInputVector() * SlidingSpeed, true, true);
	}
	// 2�� �ڿ� ����� �� �ְ� SlidingTime�� true�� �����ϴ� SlidingTimer ȣ��
	GetWorldTimerManager().SetTimer(SlidingCooldown, this, &AFPSCharacter::SlidingTimer, 2.f, false);
}

void AFPSCharacter::SlidingTimer()
{
	SlidingTime = true;
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
		// ���� ��ġ�� ���� ���� �÷��� -> ���Ӻ��� ����
		FRotator MuzzleRotation = CameraRotation;
		MuzzleRotation.Pitch += 10.0f;

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this; // �� ���͸� ������ ����(����) : this (character)
			SpawnParams.Instigator = GetInstigator(); // �� �߻�ü�� �������Ѽ� ����� damage�� å���� �ִ� ����

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

void AFPSCharacter::RefreshAmmoUI() // �������� ���ܿ��� ����, �̷��� �����ӿ�ũ (�Ŵ���)�� �����ΰ� ���ܾ��°� ����
{
	AFPSProjectGameModeBase* GameMode = Cast<AFPSProjectGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		UFPSCharacterHUD* FPSCharacterHUD = Cast<UFPSCharacterHUD>(GameMode->CurrentWidget);
		if (FPSCharacterHUD)
		{
			const FString AmmoStr = FString::Printf(TEXT("Ammo %01d / %01d"), CurrAmmo, SpareAmmo);
			FPSCharacterHUD->AmmoText->SetText(FText::FromString(AmmoStr));
		}
	}
}

void AFPSCharacter::RefreshStatUI()
{ // TODO : ��Ƽ ȯ��� ����� ü���� Refresh �� -> �� AI �����Ϸ�Ǹ� ����� ����
	AFPSProjectGameModeBase* GameMode = Cast<AFPSProjectGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		UFPSCharacterHUD* FPSCharacterHUD = Cast<UFPSCharacterHUD>(GameMode->CurrentWidget);
		if (FPSCharacterHUD)
		{
			FPSCharacterHUD->PB_2DHpBar->SetPercent(Stat->GetHpRatio());
			FPSCharacterHUD->PB_2DMpBar->SetPercent(Stat->GetMpRatio());
		}
	}
}

void AFPSCharacter::Reloading()
{
	if (SpareAmmo == 0 || CurrAmmo == MaxAmmo || IsReloading || IsRaycasting)
		return;
	// TODO : IsReloading�� �������ִ� StartReloading, StopReloaidng... ����
	IsReloading = true;

	//AnimInstanceFPP->PlayReloadingMontage();
	//WeaponAnimInstance->PlayWeaponReloadingMontage();
	//FPSMesh->PlayAnimation(AnimReloading, false);
}

// ��Ƽ���̷� ������ ������ ��ɺκ�
//void AFPSCharacter::ReloadingCheck()
//{
//	if (SpareAmmo - (MaxAmmo - CurrAmmo) < 0)
//	{
//		// 3�� ������ �����ִµ� 2/6 �� ��Ȳ, �׷��� +3�� ���־���Ѵ�
//		CurrAmmo += SpareAmmo;
//		SpareAmmo = 0;
//	}
//	else
//	{
//		SpareAmmo -= (MaxAmmo - CurrAmmo);
//		CurrAmmo = MaxAmmo;
//	}
//
//	RefreshAmmoUI();
//	IsReloading = false;
//}

void AFPSCharacter::DecreaseHp()
{
	FDamageEvent DamageEvent2;
	this->TakeDamage(100, DamageEvent2, GetController(), this);
}

void AFPSCharacter::OnReloadingMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{

}

void AFPSCharacter::OnFiringMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{

}

float AFPSCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Stat->OnAttacked(DamageAmount);
	RefreshStatUI();

	return DamageAmount;
}

bool AFPSCharacter::GetIsRaycasting()
{
	return IsRaycasting;
}

bool AFPSCharacter::GetIsReloading()
{
	return IsReloading;
}