// Fill out your copyright notice in the Description page of Project Settings.


#include "Item_LevelUp.h"
#include "Components/BoxComponent.h"
#include "FPSCharacter.h"

// Sets default values
AItem_LevelUp::AItem_LevelUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LevelUpMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LevelUp"));
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> LM(TEXT("StaticMesh'/Game/LootSet/Meshes/Scrolls_Books/magic_book2.magic_book2'"));
	if (LM.Succeeded())
	{
		LevelUpMesh->SetStaticMesh(LM.Object);
	}

	LevelUpMesh->SetupAttachment(RootComponent);
	Trigger->SetupAttachment(LevelUpMesh);
	// Trigger�� AmmoMesh�� ���̴� ������ �̷��� ���� ������ Mesh�� Trigger�� �پ �������� ����

	LevelUpMesh->SetCollisionProfileName(TEXT("Collectible"));
	Trigger->SetCollisionProfileName(TEXT("Collectible"));
	Trigger->SetBoxExtent(FVector(25.f, 25.f, 20.f));
}

// Called when the game starts or when spawned
void AItem_LevelUp::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItem_LevelUp::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AItem_LevelUp::OnCharacterOverlap);

	AddActorLocalRotation(FRotator(-15.f, 0.f, 0.f));
	// ������ ���·� ����
}

void AItem_LevelUp::OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(OtherActor);
	if (FPSCharacter)
	{
		//FPSCharacter->AddAmmo();
		FPSCharacter->LevelUp();

		Destroy(); // ź�� �����
	}
}

// Called every frame
void AItem_LevelUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorWorldRotation(FRotator(0.f, RotateSpeed * DeltaTime, 0.f));
}

