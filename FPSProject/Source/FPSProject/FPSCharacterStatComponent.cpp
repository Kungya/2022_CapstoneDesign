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
			SetHp(StatData->MaxHp); // �̷��� ����� ��������Ʈ ȣ���� �ϱ� ������ UI ������ ��
			MaxHp = StatData->MaxHp;
			SetMp(StatData->MaxMp);
			MaxMp = StatData->MaxMp;
			Attack = StatData->Attack;
		}
	}
}

void UFPSCharacterStatComponent::SetHp(int32 NewHp)
{
	if (NewHp > 50000) // NewHp�� �ִ�ü���� �Ѿ ��� �ִ�ü������ ����
		// TODO : 50000�� MaxHp��... ?
		Hp = MaxHp;
	else
		Hp = NewHp;

	if (Hp < 0)
		Hp = 0;

	// 1) Character���� �߰��� UI�� ���ͼ� �������൵ ��
	// 2) ���Ӽ��� ���� ���� Delegate ���
	// ���� �̱������� �ص� ��
	OnHpChanged.Broadcast();
}

void UFPSCharacterStatComponent::SetMp(int32 NewMp)
{
	Mp = NewMp;
	if (Mp < 0)
		Mp = 0;

}

void UFPSCharacterStatComponent::OnAttacked(float DamageAmount)
{
	int32 NewHp = Hp - DamageAmount;
	// TODO : ������� ó��
	SetHp(NewHp);
}

bool UFPSCharacterStatComponent::OnSkill(float MpAmount)
{
	int32 NewMp = Mp - MpAmount;

	if (NewMp < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mp !"));
		// TODO : Mp ���� �����
		return true;
	}

	SetMp(NewMp);

	return 0;
}