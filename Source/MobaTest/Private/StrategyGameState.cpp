// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MobaTest.h"
#include "StrategyBuilding_Brewery.h"
#include "StrategyTypes.h"

AStrategyGameState::AStrategyGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// team data for: unknown, player, enemy
	PlayersData.AddZeroed(EStrategyTeam::MAX);
	MiniMapCamera = nullptr;
	GameFinishedTime = 0;
}

void AStrategyGameState::OnActorDamaged(AActor* InActor, float Damage, AController* EventInstigator)
{
	IStrategyTeamInterface* const InstigatorTeam = Cast<IStrategyTeamInterface>(EventInstigator);

	// track damage done
	// @todo, this is def not the place for this
	if (InstigatorTeam)
	{
		FPlayerData& TeamData = PlayersData[InstigatorTeam->GetTeamNum()];
	}
}

FPlayerData* AStrategyGameState::GetPlayerData(uint8 TeamNum) const
{
	if (TeamNum != EStrategyTeam::Unknown)
	{
		return &PlayersData[TeamNum];
	}

	return nullptr;
}

void AStrategyGameState::SetGameplayState(EGameplayState::Type NewState)
{
	GameplayState = NewState;

	// notify the breweries of the state change
	for (int32 i = 0; i < PlayersData.Num(); i++)
	{
		if (PlayersData[i].BreweryLaneMid.IsValid())
		{
			PlayersData[i].BreweryLaneMid->OnGameplayStateChange(NewState);
		}
		if (PlayersData[i].BreweryLaneTop.IsValid())
		{
			PlayersData[i].BreweryLaneTop->OnGameplayStateChange(NewState);
		}
		if (PlayersData[i].BreweryLaneShort.IsValid())
		{
			PlayersData[i].BreweryLaneShort->OnGameplayStateChange(NewState);
		}
	}
}

bool AStrategyGameState::IsGameActive() const
{
	return GameplayState == EGameplayState::Playing;
}

float AStrategyGameState::GetRemainingWaitTime() const
{
	if (GameplayState == EGameplayState::Waiting)
	{
		return GetWorldTimerManager().GetTimerRemaining(TimerHandle_OnGameStart);
	}

	return 0.f;
}

void AStrategyGameState::OnGameStart()
{
	SetGameplayState(EGameplayState::Playing);
	GameFinishedTime = 0.0f;
}


void AStrategyGameState::FinishGame()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);

	SetGameplayState(EGameplayState::Finished);
	GameFinishedTime = GetWorld()->GetRealTimeSeconds();
}

void AStrategyGameState::StartGameplayStateMachine()
{
	if (WarmupTime > 0.f)
	{
		SetGameplayState(EGameplayState::Waiting);
		GetWorldTimerManager().SetTimer(TimerHandle_OnGameStart, this, &AStrategyGameState::OnGameStart, WarmupTime, false);
	}
	else
	{
		OnGameStart();
	}
}

void AStrategyGameState::SetTimersPause(bool bIsPaused)
{
	if (GameplayState == EGameplayState::Waiting )
	{
		bIsPaused ? GetWorldTimerManager().PauseTimer(TimerHandle_OnGameStart) : GetWorldTimerManager().UnPauseTimer(TimerHandle_OnGameStart);
	}
}
void AStrategyGameState::SetGamePaused(bool bIsPaused)
{

}

float AStrategyGameState::GetGameFinishedTime() const
{
	return GameFinishedTime;
}
