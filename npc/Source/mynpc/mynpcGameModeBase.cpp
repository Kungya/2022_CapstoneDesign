// Copyright Epic Games, Inc. All Rights Reserved.


#include "mynpcGameModeBase.h"
#include "MyCharacter.h"

AmynpcGameModeBase::AmynpcGameModeBase()
{
	DefaultPawnClass = AMyCharacter::StaticClass();

	static ConstructorHelpers::FClassFinder<ACharacter> BP_Char(TEXT("Blueprint'/Game/Blueprints/BP_MyCharacter.BP_MyCharacter_C'"));

	if (BP_Char.Succeeded())
	{
		DefaultPawnClass = BP_Char.Class;
	}
}