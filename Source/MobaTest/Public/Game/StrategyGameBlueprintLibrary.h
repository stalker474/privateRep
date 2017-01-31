// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyTypes.h"
#include "StrategyGameBlueprintLibrary.generated.h"

class UObject;

UCLASS()
class UStrategyGameBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/** 
	 * Are the two actors on the same team.
	 *
	 * @param Actor1	First actor to compare.
	 * @param Actor2	Second actor to compare.
	 *
	 * @returns true if the actors are on the same team.
	 */
	 UFUNCTION(BlueprintPure, Category=Game)
	static bool AreFriends(AActor* Actor1, AActor* Actor2);

	/** 
	 * Are the two actors on different teams.
	 *
	 * @param Actor1	First actor to compare.
	 * @param Actor2	Second actor to compare.
	 *
	 * @returns true if the actors are on different teams.
	 */
	UFUNCTION(BlueprintPure, Category=Game)
	static bool AreEnemies(AActor* Actor1, AActor* Actor2);

	/** 
	 * Display the wave title. 
	 * 
	 * @param WaveNumber	The Wave number to show the title for.
	 */
	UFUNCTION(BlueprintCallable, Category=Game, meta=(WorldContext="WorldContextObject"))
	static void ShowWaveTitle(class UObject* WorldContextObject, int32 WaveNumber);
};
