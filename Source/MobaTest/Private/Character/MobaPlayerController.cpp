// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "StrategyGameMode.h"
#include "StrategyGameState.h"
#include "StrategySpawnPoint.h"
#include "MobaPlayerState.h"
#include "MobaPlayerController.h"

void AMobaPlayerController::OnKilled()
{
	UnPossess();
	GetWorldTimerManager().SetTimer(TimerHandle_Respawn, this, &AMobaPlayerController::Respawn, 5.f);
}

void AMobaPlayerController::Respawn()
{
	AStrategyGameMode * gameMode = Cast<AStrategyGameMode>(GetWorld()->GetAuthGameMode());
	if (gameMode)
	{
		APawn * newPawn = gameMode->SpawnDefaultPawnFor(this, StartSpot.Get());
		Possess(newPawn);
	}
}

void AMobaPlayerController::SetStrategistMode()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}
