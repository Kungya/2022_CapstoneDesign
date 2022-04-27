// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacterGameInstance.h"

UFPSCharacterGameInstance::UFPSCharacterGameInstance()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DATA(TEXT("DataTable'/Game/Data/StatTable.StatTable'"));
	if (DATA.Succeeded())
	{
		MyStats = DATA.Object;
	}
}

void UFPSCharacterGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogTemp, Warning, TEXT("FPSCharacterGameInstance %d"), GetStatData(1)->Attack);
}

FFPSCharacterData* UFPSCharacterGameInstance::GetStatData(int32 Level)
{
	return MyStats->FindRow<FFPSCharacterData>(*FString::FromInt(Level), TEXT(""));
}