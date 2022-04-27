// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacterStatComponent.h"

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
	
}

void UFPSCharacterStatComponent::SetHp(int32 NewHp)
{
}

void UFPSCharacterStatComponent::OnAttacked(float DamageAmount)
{
}