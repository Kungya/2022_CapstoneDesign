// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacterStatComponent.h"
#include "FPSCharacterGameInstance.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UFPSCharacterStatComponent::UFPSCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	bWantsInitializeComponent = true;

	Level = 1;
}


// Called when the game starts
void UFPSCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UFPSCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetLevel(Level);
}

void UFPSCharacterStatComponent::SetLevel(int32 NewLevel)
{
	auto FPSCharacterGameInstance = Cast<UFPSCharacterGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (FPSCharacterGameInstance)
	{
		auto StatData = FPSCharacterGameInstance->GetStatData(NewLevel);
		if (StatData)
		{
			NewLevel = StatData->Level;
			SetHp(StatData->MaxHp); // 이렇게 해줘야 델리게이트 호출을 하기 때문에 UI 변경이 됨
			MaxHp = StatData->MaxHp;
			Attack = StatData->Attack;
		}
	}
}

void UFPSCharacterStatComponent::SetHp(int32 NewHp)
{
	Hp = NewHp;
	if (Hp < 0)
		Hp = 0;

	// 1) Character에서 추가한 UI를 들고와서 설정해줘도 됨
	// 2) 종속성을 끊기 위해 Delegate 사용
	// 전역 싱글톤으로 해도 됨
	OnHpChanged.Broadcast();
}

void UFPSCharacterStatComponent::OnAttacked(float DamageAmount)
{
	int32 NewHp = Hp - DamageAmount;
	SetHp(NewHp);
}