// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyTypes.h"
#include "StrategyGameState.generated.h"

class AStrategyChar;
/*class AStrategyMiniMapCapture;*/

UCLASS(config=Game)
class AStrategyGameState : public AGameState
{
	GENERATED_UCLASS_BODY()

public:
	/** Game state. */
	EGameplayState::Type GameplayState;

	/** Warm up time before game starts */
	UPROPERTY(config)
	int32 WarmupTime;

	UFUNCTION()
	void NewPlayerJoined(AController* Player);

	void FinishGame();

	/** 
	 * Notification that an actor was damaged. 
	 * 
	 * @param	InChar	The character that has died.
	 * @param	Damage	The amount of damage inflicted.
	 * @param	InChar	The controller that inflicted the damage.
	 */
	void OnActorDamaged(AActor* InActor, float Damage, AController* EventInstigator);
	
	/** 
	 * Get a team's data. 
	 * 
	 * @param	TeamNum	The team to get the data for
	 * @returns FPlayerData pointer to the data for requested team.
	 */
	FPlayerData* GetPlayerData(uint8 TeamNum) const;

	/** 
	 * Initialize the game-play state machine. 
	 */
	void StartGameplayStateMachine();

	/** 
	 * Change game state and notify observers.
	 * @param	NewState	The required game state.
	 */
	void SetGameplayState(EGameplayState::Type NewState);

	/** Is game currently in state: playing ? */
	bool IsGameActive() const;

	/** Notification that game has started. */
	void OnGameStart();

	/** Get time until game starts, in seconds. */
	float GetRemainingWaitTime() const;

	/** Get time when game finished */
	float GetGameFinishedTime() const;

	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, Category = "Characters")
	TArray<TSubclassOf<class AMobaTestCharacter>> AvailableCharacters;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Blue Players")
	TArray<APlayerState*> BluePlayers;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Red Players")
	TArray<APlayerState*> RedPlayers;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Chat")
	FString ChatText;

protected:
	
	// @todo, get rid of mutable?
	/** Gameplay information about each player. */	
	mutable TArray<FPlayerData> PlayersData;

	/** Time in seconds when the game finished. Set at the end of game. */
	float GameFinishedTime;

	/** Handle for efficient management of UpdateHealth timer */
	FTimerHandle TimerHandle_OnGameStart;

	/** 
	 * Pauses/Unpauses current game timer. 
	 * 
	 * @param	bIsPaused The required pause state.
	 */
	void SetTimersPause(bool bIsPaused);
};


