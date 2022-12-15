// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"
#include "EnemyAI.h"
#include "EnemyAnimInstance.h"
#include "FPSCharacterStatComponent.h"

// Sets default values
AEnemy::AEnemy() :
	Health(100.f),
	MaxHealth(100.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM(TEXT("SkeletalMesh'/Game/AnimalVarietyPack/Fox/Meshes/SK_Fox.SK_Fox'"));

	if (SM.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SM.Object);
	}

	Stat = CreateDefaultSubobject<UFPSCharacterStatComponent>(TEXT("STAT"));

	AIControllerClass = AEnemyAI::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
}

void AEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AnimInstance = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &AEnemy::OnAttackMontageEnded);
		AnimInstance->OnAttackHit.AddUObject(this, &AEnemy::AttackCheck);
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AEnemy::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AEnemy::Attack);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AEnemy::UpDown);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AEnemy::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("Yaw"), this, &AEnemy::Yaw);

}

void AEnemy::Attack()
{
	if (IsAttacking)
		return;

	AnimInstance->PlayAttackMontage();

	IsAttacking = true;
}

void AEnemy::AttackCheck()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);

	float AttackRange = 50.f;
	float AttackRadius = 50.f;

	bool bResult = GetWorld()->SweepSingleByChannel(
		OUT HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(AttackRadius),
		Params);

	FVector Vec = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + Vec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat Rotation = FRotationMatrix::MakeFromZ(Vec).ToQuat();
	FColor DrawColor;
	if (bResult)
		DrawColor = FColor::Green;
	else
		DrawColor = FColor::Red;

	DrawDebugCapsule(GetWorld(), Center, HalfHeight, AttackRadius,
		Rotation, DrawColor, false, 2.f);

	if (bResult && HitResult.Actor.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("Hit Actor : %s"), *HitResult.Actor->GetName());

		FDamageEvent DamageEvent;
		HitResult.Actor->TakeDamage(Stat->GetAttack(), DamageEvent, GetController(), this);
	}
}

/*void AEnemy::BulletHit_Implementation(FHitResult HitResult)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Impactparticles, HitResult.Location, FRotator(0.f), true);
	}
}*/

/*float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
	}
	else
	{
		Health -= DamageAmount;
	}
	return DamageAmount;
}*/

void AEnemy::UpDown(float Value)
{
	UpDownValue = Value;
	AddMovementInput(GetActorForwardVector(), Value);
}

void AEnemy::LeftRight(float Value)
{
	LeftRightValue = Value;
	AddMovementInput(GetActorRightVector(), Value);
}

void AEnemy::Yaw(float Value)
{
	AddControllerYawInput(Value);
}

void AEnemy::OnAttackMontageEnded(class UAnimMontage* Montage, bool binterrupted)
{
	IsAttacking = false;
	OnAttackEnd.Broadcast();
}
float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Stat->OnAttacked(DamageAmount);

	return DamageAmount;
}