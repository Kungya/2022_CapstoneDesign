// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BulletHitInterface.h"
#include "Enemy.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEnd);

UCLASS()
class FPSPROJECT_API AEnemy : public ACharacter, public IBulletHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta =(AllowPrivateAccess="true"))
	class UParticleSystem* Impactparticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* ImpactSound;

	/*적 체력*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float Health;
	/*적 최대 체력*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Attack();
	void AttackCheck();

	FOnAttackEnd OnAttackEnd;

	//virtual void BulletHit_Implementation(FHitResult HitResult) override;

	//virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void UpDown(float Value);
	void LeftRight(float Value);
	void Yaw(float Value);

	UFUNCTION()
	void OnAttackMontageEnded(class UAnimMontage* Montage, bool binterrupted);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
private:
	UPROPERTY(VisibleAnywhere, Category = Pawn)
	bool IsAttacking = false;

	UPROPERTY()
	class UEnemyAnimInstance* AnimInstance;

public:

	UPROPERTY()
	float UpDownValue = 0;

	UPROPERTY()
	float LeftRightValue = 0;

	UPROPERTY(VisibleAnywhere)
	class UFPSCharacterStatComponent* Stat;
};
