// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo.h"
#include "Components/BoxComponent.h"
#include "FPSCharacter.h"

// Sets default values
AAmmo::AAmmo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AMMO"));
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> AP(TEXT("StaticMesh'/Game/MilitaryWeapSilver/Pickups/Shotgun_Pickup.Shotgun_Pickup'"));
	if (AP.Succeeded())
	{
		AmmoMesh->SetStaticMesh(AP.Object);
	}

	AmmoMesh->SetupAttachment(RootComponent);
	Trigger->SetupAttachment(AmmoMesh);
	// Trigger�� AmmoMesh�� ���̴� ������ �̷��� ���� ������ Mesh�� Trigger�� �پ �������� ����

	AmmoMesh->SetCollisionProfileName(TEXT("Collectible"));
	Trigger->SetCollisionProfileName(TEXT("Collectible"));
	Trigger->SetBoxExtent(FVector(25.f, 25.f, 20.f));
}

// Called when the game starts or when spawned
void AAmmo::BeginPlay()
{
	Super::BeginPlay();

}

void AAmmo::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AAmmo::OnCharacterOverlap);

	AddActorLocalRotation(FRotator(-15.f, 0.f, 0.f));
	// ������ ���·� ����
}

void AAmmo::OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(OtherActor);
	if (FPSCharacter)
	{
		FPSCharacter->AddAmmo();

		Destroy(); // ź�� �����
	}
}

// Called every frame
void AAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorWorldRotation(FRotator(0.f, RotateSpeed * DeltaTime, 0.f));
}

