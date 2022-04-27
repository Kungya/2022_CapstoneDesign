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

	// 현재 체력
	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 Hp;
	// 최대 체력
	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 MaxHp;
	// 공격력 -> TODO : 무기에 따라 변동,,,,
	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 Attack;

public:
	FOnHpChanged OnHpChanged;
};
