// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPSCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSPROJECT_API UFPSCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFPSCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;

public:	
	void SetLevel(int32 NewLevel);
	/*������Ʈ �Ը� Ŀ���� HP�� ����Ǵ� ���� ��찡 ����� ������ 
	SetHp�θ� Hp�� ����ǵ��� �������*/
	void SetHp(int32 NewHp);
	void OnAttacked(float DamageAmount);

	int32 GetLevel() { return Level; }
	int32 GetHp() { return Hp; }
	int32 GetMaxHp() { return MaxHp; }
	float GetHpRatio() { return Hp / (float)MaxHp; }
	int32 GetAttack() { return Attack; }

private:
	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 Level;

	// ���� ü��
	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 Hp;
	// �ִ� ü��
	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 MaxHp;
	// ���ݷ� -> TODO : ���⿡ ���� ����,,,,
	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 Attack;

public:
	FOnHpChanged OnHpChanged;
};
