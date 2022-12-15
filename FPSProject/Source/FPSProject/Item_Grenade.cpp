// Fill out your copyright notice in the Description page of Project Settings.


#include "Item_Grenade.h"
#include "Components/BoxComponent.h"
#include "FPSCharacter.h"

// Sets default values
AItem_Grenade::AItem_Grenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!GrenadeMesh)
	{
		GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grenade"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>GM(TEXT("StaticMesh'/Game/Assets/Grenade/grenadeMesh.grenadeMesh'"));
		if (GM.Succeeded())
		{
			GrenadeMesh->SetStaticMesh(GM.Object);
		}
	}

	GrenadeMesh->SetRelativeScale3D(FVector(1.4f, 1.4f, 1.4f)); //크기 조정
	GrenadeMesh->SetupAttachment(RootComponent);
	
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Trigger->SetupAttachment(GrenadeMesh);
	// Trigger를 AmmoMesh에 붙이는 이유는 이렇게 하지 않으면 Mesh와 Trigger가 붙어서 움직이지 않음

	GrenadeMesh->SetCollisionProfileName(TEXT("Collectible"));
	Trigger->SetCollisionProfileName(TEXT("Collectible"));
	Trigger->SetBoxExtent(FVector(25.f, 25.f, 20.f));
}

// Called when the game starts or when spawned
void AItem_Grenade::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItem_Grenade::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AItem_Grenade::OnCharacterOverlap);

	AddActorLocalRotation(FRotator(-15.f, 0.f, 0.f));
	// 기울어진 상태로 시작
}

void AItem_Grenade::OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(OtherActor);
	if (FPSCharacter)
	{
		FPSCharacter->AddGrenade();

		Destroy(); // 수류탄 사라짐
	}
}

// Called every frame
void AItem_Grenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorWorldRotation(FRotator(0.f, RotateSpeed * DeltaTime, 0.f));
}

