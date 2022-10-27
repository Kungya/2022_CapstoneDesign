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
	// 연사속도, 0.1f = 초당 10회, 그리고 CrosshairSpread timer rate보다 무조건 커야한다(0.05 사용중)
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
	GetCharacterMovement()->MaxAcceleration = 1024.f; // 최대 가속?
	GetCharacterMovement()->BrakingDecelerationWalking = 85.f; // 급감속
	GetCharacterMovement()->GroundFriction = 4.f; // 마찰계수 감속에 영향

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

	//// 1인칭 SkeletalMesh 추가
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> FPPSM(TEXT("SkeletalMesh'/Game/Assets/HeroFPP.HeroFPP'"));
	//if (FPPSM.Succeeded())
	//{
	//	FPSMesh->SetSkeletalMesh(FPPSM.Object);
	//}

	// 3인칭 SkeletalMesh 추가
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM(TEXT("SkeletalMesh'/Game/ParagonLtBelica/Characters/Heroes/Belica/Meshes/Belica.Belica'"));
	if (SM.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SM.Object);
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	}
	
	// FPSMesh의 소켓에 무기 장착
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

	//HpBar Component 생성
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

	// BeginPlay 에서 FollowCamera가 구성되므로 여기서 설정
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
	//{// 델리게이트 바인딩 - Reloading
	//	AnimInstanceFPP->OnMontageEnded.AddDynamic(this, &AFPSCharacter::OnReloadingMontageEnded);
	//	//AnimInstanceFPP->OnReloading.AddUObject(this, &AFPSCharacter::ReloadingCheck);
	//	/*-> 탄창이 다시 장착된 순간에 재장전 기능을 실행하는게 적합하다고 생각하여
	//	Weapon 쪽 Notify로 재장전 기능을 실행했기 때문에 현재 사용 안함,*/
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

	// Tick 마다 bAiming 값에 따라 Base Rate 값을 바꿔주는 형식 (감도)
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

	// "action" 바인딩 구성
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::StartJump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPSCharacter::StopJump);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	PlayerInputComponent->BindAction("Sliding", IE_Pressed, this, &AFPSCharacter::Sliding);

	// "Fire" 바인딩 구성
	PlayerInputComponent->BindAction("FireProjectile", IE_Pressed, this, &AFPSCharacter::Fire);
	// "RayCast" 바인딩 구성
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AFPSCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AFPSCharacter::FireButtonReleased);
	// Aiming
	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AFPSCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AFPSCharacter::AimingButtonReleased);

	//PlayerInputComponent->BindAction("Raycast", IE_Pressed, this, &AFPSCharacter::StartRaycast);
	//PlayerInputComponent->BindAction("Raycast", IE_Released, this, &AFPSCharacter::StopRaycast);
	// "Relodaing" 바인딩
	//PlayerInputComponent->BindAction("Reloading", IE_Pressed, this, &AFPSCharacter::Reloading);
	// 실험용, HP감소
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
		// 총구 위치
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
	//		ECollisionChannel::ECC_Visibility, // TODO : 채널을 추후에 Visibility 이외에 다른 것으로 변경
	//		FCollisionQueryParams(),
	//		FCollisionResponseParams()
	//	);
	//	//DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 2.f, 0.f, 1.f);
	//	
	//	if (RayCastResult && HitResult.Actor.IsValid())
	//	{
	//		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, HitResult.GetActor()->GetFName().ToString());
	//		FPointDamageEvent PointDamageEvent;
	//		// this : 내가 공격하는거니까
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

	if (bScreenToWorld) // deprojection 성공했을경우
	{
		FHitResult ScreenTraceHit;
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ CrosshairWorldPosition + CrosshairWorldDirection * 50'000.f };

		/* BeamEndPoint를 미리 End로 지정해놓음, 도중 충돌한 물체가 있다면 End를 바꾸고, 아니면
		 End까지 Beam 효과를 내겠다는 뜻 */
		OutBeamLocation = End;

		// Trace from corsshairs world location
		GetWorld()->LineTraceSingleByChannel(
			ScreenTraceHit,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);

		if (ScreenTraceHit.bBlockingHit) // if trace hit
		{
			// 충돌한 물체가 있을 경우, BeamEndPoint를 End에서 충돌한 물체의 Location으로 변경
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

	// Mp 소모량 : 20, Mp가 부족하면 1을 리턴하여 스킬 종료
	if (Stat->OnSkill(20))
		return;

	RefreshStatUI();

	SlidingTime = false;

	float SlidingSpeed = 5000.f;

	if (GetMovementComponent()->IsFalling()) // 공중에서는 마찰이 적어 속도 하향
		SlidingSpeed = 2000.f;

	if (GetLastMovementInputVector() == FVector::ZeroVector)
	{
		LaunchCharacter(GetActorForwardVector() * SlidingSpeed, true, true);
	}
	else
	{
		LaunchCharacter(GetLastMovementInputVector() * SlidingSpeed, true, true);
	}
	// 2초 뒤에 사용할 수 있게 SlidingTime을 true로 설정하는 SlidingTimer 호출
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

void AFPSCharacter::RefreshAmmoUI() // 전역으로 땡겨오는 형식, 이렇게 프레임워크 (매니저)를 만들어두고 땡겨쓰는게 낫다
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
{ // TODO : 멀티 환경시 상대의 체력이 Refresh 됨 -> 몹 AI 구현완료되면 디버깅 예정
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
	// TODO : IsReloading을 조절해주는 StartReloading, StopReloaidng... 설정
	IsReloading = true;

	//AnimInstanceFPP->PlayReloadingMontage();
	//WeaponAnimInstance->PlayWeaponReloadingMontage();
	//FPSMesh->PlayAnimation(AnimReloading, false);
}

// 노티파이로 구현된 재장전 기능부분
//void AFPSCharacter::ReloadingCheck()
//{
//	if (SpareAmmo - (MaxAmmo - CurrAmmo) < 0)
//	{
//		// 3발 여유가 남아있는데 2/6 인 상황, 그러면 +3만 해주어야한다
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