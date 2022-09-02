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
	// �ӽ÷� ü�� ���� (������) Potion��  Static Mesh ���
	if (SP.Succeeded())
	{
		PotionMesh->SetStaticMesh(SP.Object);
	}

	PotionMesh->SetupAttachment(RootComponent);
	Trigger->SetupAttachment(PotionMesh);
	// Trigger�� PotionMesh�� ���̴� ������ �̷��� ���� ������ Mesh�� Trigger�� �پ �������� ����
	
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

		Destroy(); // ���� �����
	}
}

// Called every frame
void APotion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorWorldRotation(FRotator(0.f, RotateSpeed * DeltaTime, 0.f));
}

