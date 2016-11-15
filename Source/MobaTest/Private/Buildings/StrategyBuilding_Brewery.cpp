// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MobaTest.h"
#include "StrategyBuilding_Brewery.h"
#include "StrategyAIDirector.h"
#include "StrategyBuilding.h"

AStrategyBuilding_Brewery::AStrategyBuilding_Brewery(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	AIDirector = CreateDefaultSubobject<UStrategyAIDirector>(TEXT("AIDirectorComp"));
}

void AStrategyBuilding_Brewery::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Role == ROLE_Authority)
	{
		FPlayerData* const MyData = GetTeamData();
		if (MyData)
		{
			AIDirector->LanePosition = LanePosition;
			AIDirector->SetTeamNum(GetTeamNum());

			if (LanePosition == ELaneEnum::VE_Top)
				MyData->BreweryLaneTop = this;
			else if (LanePosition == ELaneEnum::VE_Mid)
				MyData->BreweryLaneMid = this;
			else if (LanePosition == ELaneEnum::VE_Short)
				MyData->BreweryLaneShort = this;
		}
	}
}

void AStrategyBuilding_Brewery::OnGameplayStateChange(EGameplayState::Type NewState)
{
	if (Role == ROLE_Authority)
	{
		if (AIDirector != nullptr)
		{
			AIDirector->OnGameplayStateChange(NewState);
		}
	}
}

FText AStrategyBuilding_Brewery::GetSpawnQueueLength() const
{
	return AIDirector->WaveSize > 0 ? FText::AsNumber(AIDirector->WaveSize) : FText::GetEmpty();
}

bool AStrategyBuilding_Brewery::SpawnDwarf()
{
	if (Role == ROLE_Authority)
	{
		FPlayerData* const MyData = GetTeamData();
		if (MyData && AIDirector != nullptr)
		{
			AIDirector->RequestSpawn();
		}
	}
	return false;
}
