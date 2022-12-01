// Fill out your copyright notice in the Description page of Project Settings.


#include "Potion_S_MP.h"
#include "Components/BoxComponent.h"
#include "FPSCharacter.h"
#include "FPSCharacterStatComponent.h"

// Sets default values
APotion_S_MP::APotion_S_MP()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PotionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SMANAPOTION"));
	// Convention - S : Small, MANA : Blue (<-> Health)
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SP(TEXT("StaticMesh'/Game/Assets/Potion/Potion_S_2.Potion_S_2'"));

	if (SP.Succeeded())
	{
		PotionMesh->SetStaticMesh(SP.Object);
	}

	PotionMesh->SetupAttachment(RootComponent);
	Trigger->SetupAttachment(PotionMesh);

	PotionMesh->SetCollisionProfileName(TEXT("Collectible"));
	Trigger->SetCollisionProfileName(TEXT("Collectibe"));
	Trigger->SetBoxExtent(FVector(25.f, 25.f, 20.f));

}

// Called when the game starts or when spawned
void APotion_S_MP::BeginPlay()
{
	Super::BeginPlay();

	AddActorLocalRotation(FRotator(-15.f, 0.f, 0.f));
	// 기울어진 상태로 시작
	
}

void APotion_S_MP::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &APotion_S_MP::OnCharacterOverlap);
}

void APotion_S_MP::OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(OtherActor);
	if (FPSCharacter)
	{
		//FDamageEvent DamageEvent_ManaPotion;
		// TODO : 
		FPSCharacter->Stat->OnSkill(-20);
		FPSCharacter->RefreshStatUI();

		Destroy();
	}
}

// Called every frame
void APotion_S_MP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorWorldRotation(FRotator(0.f, RotateSpeed * DeltaTime, 0.f));

}

