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

	// FPS Camera Component ����
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	// null check
	check(FPSCameraComponent != nullptr);

	// Camera Component�� Capsule Component�� ����
	FPSCameraComponent->SetupAttachment(GetCapsuleComponent());

	// ī�޶� ��ġ�� �� ��������
	FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 40.0f + BaseEyeHeight));
	// Pawn�� ī�޶� Rotation ���� ���
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

	// 1��Ī SkeletalMesh �߰�
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FPPSM(TEXT("SkeletalMesh'/Game/Assets/HeroFPP.HeroFPP'"));
	if (FPPSM.Succeeded())
	{
		FPSMesh->SetSkeletalMesh(FPPSM.Object);
	}

	// 3��Ī SkeletalMesh �߰�
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM(TEXT("SkeletalMesh'/Game/Assets/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (SM.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SM.Object);
		// The owning player doesn't see the regular (third-person) body mesh.
		GetMesh()->SetOwnerNoSee(true);
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -78.f));
	}
	
	// FPSMesh�� ���Ͽ� ���� ����
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

	//// �̴ϸ� ������ SpringArm + SceneCapture ������ �������Ʈ ���¿��� ������Ʈ �߰��� ��ü��
	//SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	//SpringArm->TargetArmLength = 300.f;
	//SpringArm->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	//
	//MiniMapCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MINIMAPCAPTURE"));
	//MiniMapCapture->SetupAttachment(SpringArm);
	

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

	if (GEngine)
	{
		// 5�ʰ� ����� �޽��� ǥ��
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
	{// ��������Ʈ ���ε� - Reloading
		AnimInstanceFPP->OnMontageEnded.AddDynamic(this, &AFPSCharacter::OnReloadingMontageEnded);
		//AnimInstanceFPP->OnReloading.AddUObject(this, &AFPSCharacter::ReloadingCheck);
		/*-> źâ�� �ٽ� ������ ������ ������ ����� �����ϴ°� �����ϴٰ� �����Ͽ�
		Weapon �� Notify�� ������ ����� �����߱� ������ ���� ��� ����,*/
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

	// "movement" ���ε� ����
	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);

	// "look" ���ε� ����
	PlayerInputComponent->BindAxis("Yaw", this, &AFPSCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Pitch", this, &AFPSCharacter::AddControllerPitchInput);

	// "action" ���ε� ����
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::StartJump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPSCharacter::StopJump);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::Jump);
	
	PlayerInputComponent->BindAction("Sliding", IE_Pressed, this, &AFPSCharacter::Sliding);

	// "Fire" ���ε� ����
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::Fire);
	// "RayCast" ���ε� ����
	PlayerInputComponent->BindAction("Raycast", IE_Pressed, this, &AFPSCharacter::StartRaycast);
	PlayerInputComponent->BindAction("Raycast", IE_Released, this, &AFPSCharacter::StopRaycast);
	// "Relodaing" ���ε�
	PlayerInputComponent->BindAction("Reloading", IE_Pressed, this, &AFPSCharacter::Reloading);
	// �����, HP����
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
			ECollisionChannel::ECC_Visibility, // TODO : ä���� ���Ŀ� Visibility �̿ܿ� �ٸ� ������ ����
			FCollisionQueryParams(),
			FCollisionResponseParams()
		);
		//DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 2.f, 0.f, 1.f);
		
		if (RayCastResult && HitResult.Actor.IsValid())
		{
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, HitResult.GetActor()->GetFName().ToString());

			FDamageEvent DamageEvent;
			// this : ���� �����ϴ°Ŵϱ�
			HitResult.Actor->TakeDamage(Stat->GetAttack(), DamageEvent, GetController(), this);
		}

	}


	// Recoil, TODO : Using Vector Curve, Interp
	AddControllerPitchInput(FMath::RandRange(0.3f, 0.6f) * -1);
	AddControllerYawInput(FMath::RandRange(-0.1f, 0.1f));

	/*
	TODO : �ݵ�... -3x^@
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
	// TODO : IsReloading�� �������ִ� StartReloading, StopReloaidng... ����
	IsReloading = true;

	AnimInstanceFPP->PlayReloadingMontage();
	WeaponAnimInstance->PlayWeaponReloadingMontage();
	//FPSMesh->PlayAnimation(AnimReloading, false);
}

void AFPSCharacter::ReloadingCheck()
{
	if (SpareAmmo - (MaxAmmo - CurrAmmo) < 0)
	{
		// 3�� ������ �����ִµ� 2/6 �� ��Ȳ, �׷��� +3�� ���־���Ѵ�
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