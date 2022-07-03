// Fill out your copyright notice in the Description page of Project Settings.
#include "FPSCharacter.h"
#include "FPSProject.h"
#include "FPSCharacterAnimInstance.h"
#include "FPSWeaponAnimInstance.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
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
	FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 40.0f + BaseEyeHeight));
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

	// 1인칭 SkeletalMesh 추가
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FPPSM(TEXT("SkeletalMesh'/Game/Assets/HeroFPP.HeroFPP'"));
	if (FPPSM.Succeeded())
	{
		FPSMesh->SetSkeletalMesh(FPPSM.Object);
	}

	// 3인칭 SkeletalMesh 추가
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM(TEXT("SkeletalMesh'/Game/Assets/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (SM.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SM.Object);
		// The owning player doesn't see the regular (third-person) body mesh.
		GetMesh()->SetOwnerNoSee(true);
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -78.f));
	}
	
	// FPSMesh의 소켓에 무기 장착
	FName WeaponSocket(TEXT("b_RightWeapon_Socket"));
	if (FPSMesh->DoesSocketExist(WeaponSocket))
	{
		FPSWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));

		static ConstructorHelpers::FObjectFinder<USkeletalMesh> SW(TEXT("SkeletalMesh'/Game/MilitaryWeapSilver/Weapons/Assault_Rifle_A.Assault_Rifle_A'"));
		
		check(FPSCameraComponent != nullptr);

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
	}

	//// 미니맵 추적용 SpringArm + SceneCapture 에러로 블루프린트 상태에서 컴포넌트 추가로 대체중
	//SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	//SpringArm->TargetArmLength = 300.f;
	//SpringArm->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	//
	//MiniMapCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MINIMAPCAPTURE"));
	//MiniMapCapture->SetupAttachment(SpringArm);
	

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

	if (GEngine)
	{
		// 5초간 디버그 메시지 표시
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using FPSCharacter."));
	}

	RefreshAmmoUI();
	RefreshStatUI();

	//UFPSRecoil* Recoil = NewObject<UFPSRecoil>(this, UFPSRecoil::StaticClass());
}

void AFPSCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AnimInstance = Cast<UFPSCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{

	}

	AnimInstanceFPP = Cast<UFPSCharacterAnimInstance>(FPSMesh->GetAnimInstance());
	if (AnimInstanceFPP)
	{// 델리게이트 바인딩 - Reloading
		AnimInstanceFPP->OnMontageEnded.AddDynamic(this, &AFPSCharacter::OnReloadingMontageEnded);
		//AnimInstanceFPP->OnReloading.AddUObject(this, &AFPSCharacter::ReloadingCheck);
		/*-> 탄창이 다시 장착된 순간에 재장전 기능을 실행하는게 적합하다고 생각하여
		Weapon 쪽 Notify로 재장전 기능을 실행했기 때문에 현재 사용 안함,*/
	}

	WeaponAnimInstance = Cast<UFPSWeaponAnimInstance>(FPSWeapon->GetAnimInstance());
	if (WeaponAnimInstance)
	{
		WeaponAnimInstance->OnMontageEnded.AddDynamic(this, &AFPSCharacter::OnFiringMontageEnded);
		WeaponAnimInstance->OnFiring.AddUObject(this, &AFPSCharacter::Raycast);
		WeaponAnimInstance->OnWeaponReloading.AddUObject(this, &AFPSCharacter::ReloadingCheck);
	}

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

	//Recoil->RecoilTick(DeltaTime);
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
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::StartJump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPSCharacter::StopJump);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::Jump);
	
	PlayerInputComponent->BindAction("Sliding", IE_Pressed, this, &AFPSCharacter::Sliding);

	// "Fire" 바인딩 구성
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::Fire);
	// "RayCast" 바인딩 구성
	PlayerInputComponent->BindAction("Raycast", IE_Pressed, this, &AFPSCharacter::StartRaycast);
	PlayerInputComponent->BindAction("Raycast", IE_Released, this, &AFPSCharacter::StopRaycast);
	// "Relodaing" 바인딩
	PlayerInputComponent->BindAction("Reloading", IE_Pressed, this, &AFPSCharacter::Reloading);
	// 실험용, HP감소
	PlayerInputComponent->BindAction("DecreaseHp", IE_Pressed, this, &AFPSCharacter::DecreaseHp);

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
	GetWorldTimerManager().SetTimer(Timer, this, &AFPSCharacter::SlidingTimer, 2.f, false);
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

void AFPSCharacter::StartRaycast()
{
	IsRaycasting = true;

	Raycast();
}

void AFPSCharacter::Raycast()
{
	if (CurrAmmo <= 0 || !IsRaycasting || IsReloading)
		return;

	WeaponAnimInstance->PlayFiringMontage();

	--CurrAmmo;
	RefreshAmmoUI();


	FVector start = FPSCameraComponent->GetComponentLocation();
	FVector forward = FPSCameraComponent->GetForwardVector();
	FVector end = start + forward * 3000;
	FHitResult HitResult;
	
	if (GetWorld())
	{
		bool RayCastResult = GetWorld()->LineTraceSingleByChannel(
			OUT HitResult,
			start,
			end,
			ECollisionChannel::ECC_Visibility, // TODO : 채널을 추후에 Visibility 이외에 다른 것으로 변경
			FCollisionQueryParams(),
			FCollisionResponseParams()
		);
		//DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 2.f, 0.f, 1.f);
		
		if (RayCastResult && HitResult.Actor.IsValid())
		{
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, HitResult.GetActor()->GetFName().ToString());

			FDamageEvent DamageEvent;
			// this : 내가 공격하는거니까
			HitResult.Actor->TakeDamage(Stat->GetAttack(), DamageEvent, GetController(), this);
		}

	}


	// Recoil, TODO : Using Vector Curve, Interp
	AddControllerPitchInput(FMath::RandRange(0.3f, 0.6f) * -1);
	AddControllerYawInput(FMath::RandRange(-0.1f, 0.1f));

	/*
	TODO : 반동... -3x^@
	*/
	// Recursion

	GetWorldTimerManager().SetTimer(AutoModeTimer, this, &AFPSCharacter::Raycast, .12f, false);
}

void AFPSCharacter::StopRaycast()
{
	//Recoil->RecoilStop();
	IsRaycasting = false;
}

void AFPSCharacter::Reloading()
{
	if (SpareAmmo == 0 || CurrAmmo == MaxAmmo || IsReloading || IsRaycasting)
		return;
	// TODO : IsReloading을 조절해주는 StartReloading, StopReloaidng... 설정
	IsReloading = true;

	AnimInstanceFPP->PlayReloadingMontage();
	WeaponAnimInstance->PlayWeaponReloadingMontage();
	//FPSMesh->PlayAnimation(AnimReloading, false);
}

void AFPSCharacter::ReloadingCheck()
{
	if (SpareAmmo - (MaxAmmo - CurrAmmo) < 0)
	{
		// 3발 여유가 남아있는데 2/6 인 상황, 그러면 +3만 해주어야한다
		CurrAmmo += SpareAmmo;
		SpareAmmo = 0;
	}
	else
	{
		SpareAmmo -= (MaxAmmo - CurrAmmo);
		CurrAmmo = MaxAmmo;
	}

	RefreshAmmoUI();
	IsReloading = false;
}

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