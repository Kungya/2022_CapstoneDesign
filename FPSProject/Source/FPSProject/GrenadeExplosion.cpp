// Fill out your copyright notice in the Description page of Project Settings.


#include "GrenadeExplosion.h"

// Sets default values
AGrenadeExplosion::AGrenadeExplosion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RadialForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComponent"));
	Particle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
	if (PS.Succeeded())
	{
		Particle->Template = PS.Object;
	}

	RadialForce->Radius = 1000.f;
	RadialForce->ImpulseStrength = 300000.f;
	RadialForce->ForceStrength = 1000.f;

	//InitialLifeSpan = 3.f;
}

// Called when the game starts or when spawned
void AGrenadeExplosion::BeginPlay()
{
	Super::BeginPlay(); 

	RadialForce->FireImpulse();
	SetLifeSpan(3.f);
}