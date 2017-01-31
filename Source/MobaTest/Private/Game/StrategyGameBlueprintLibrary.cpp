// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MobaTest.h"
#include "StrategyGameBlueprintLibrary.h"
#include "StrategyAttachment.h"

UStrategyGameBlueprintLibrary::UStrategyGameBlueprintLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

AStrategyGameMode* GetGameFromContextObject(UObject* WorldContextObject)
{
	UWorld* const MyWorld = GEngine->GetWorldFromContextObject(WorldContextObject);
	check(MyWorld);

	AStrategyGameMode* const MyGame = MyWorld->GetAuthGameMode<AStrategyGameMode>();
	return MyGame;
}

AStrategyGameState* GetGameStateFromContextObject(UObject* WorldContextObject)
{
	UWorld* const MyWorld = GEngine->GetWorldFromContextObject(WorldContextObject);
	check(MyWorld);

	AStrategyGameState* const MyGameState = MyWorld->GetGameState<AStrategyGameState>();
	return MyGameState;
}

bool UStrategyGameBlueprintLibrary::AreFriends(AActor* Actor1, AActor* Actor2)
{
	return AStrategyGameMode::OnFriendlyTeam(Actor1, Actor2);
}

bool UStrategyGameBlueprintLibrary::AreEnemies(AActor* Actor1, AActor* Actor2)
{
	return AStrategyGameMode::OnEnemyTeam(Actor1, Actor2);
}

void UStrategyGameBlueprintLibrary::ShowWaveTitle(UObject* WorldContextObject, int32 WaveNumber)
{
}
