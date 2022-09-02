// Fill out your copyright notice in the Description page of Project Settings.


#include "Potion.h"
#include "Components/BoxComponent.h"
#include "FPSCharacter.h"
#include "FPSCharacterStatComponent.h"

// Sets default values
APotion::APotion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PotionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("POTION"));
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SP(TEXT("StaticMesh'/Game/Assets/Potion/PotionA.PotionA'"));
	// 임시로 체력 포션 (빨간색) Potion의  Static Mesh 사용
	if (SP.Succeeded())
	{
		PotionMesh->SetStaticMesh(SP.Object);
	}

	PotionMesh->SetupAttachment(RootComponent);
	Trigger->SetupAttachment(PotionMesh);
	// Trigger를 PotionMesh에 붙이는 이유는 이렇게 하지 않으면 Mesh와 Trigger가 붙어서 움직이지 않음
	
	PotionMesh->SetCollisionProfileName(TEXT("Collectible"));
	Trigger->SetCollisionProfileName(TEXT("Collectible"));
	Trigger->SetBoxExtent(FVector(25.f, 25.f, 20.f));

}

// Called when the game starts or when spawned
void APotion::BeginPlay()
{
	Super::BeginPlay();

	AddActorLocalRotation(FRotator(-15.f, 0.f, 0.f));
	
}

void APotion::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &APotion::OnCharacterOverlap);
}

void APotion::OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(OtherActor);
	if (FPSCharacter)
	{
		FDamageEvent DamageEvent_Potion;
		FPSCharacter->TakeDamage(-30, DamageEvent_Potion, GetInstigatorController(), this);

		Destroy(); // 포션 사라짐
	}
}

// Called every frame
void APotion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorWorldRotation(FRotator(0.f, RotateSpeed * DeltaTime, 0.f));
}

