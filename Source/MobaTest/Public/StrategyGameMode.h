// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyTypes.h"
#include "StrategyGameMode.generated.h"

class AController;
class AStrategyBuilding;
class SStrategyTitle;

UCLASS(config=Game)
class AStrategyGameMode : public AGameMode
{
	GENERATED_UCLASS_BODY()
	
	// Begin GameMode interface

	/** Initialize the GameState actor. */
	virtual void InitGameState() override;
	
	/** 
	 * Modify the damage we want to apply to an actor.
	 * 
	  * @param Damage			The damage
	  * @param DamagedActor		The actor we wish to damage
	  * @param DamageEvent		The event that caused the damage
	  * @param EventInstigator	
	  * @param DamageCauser
	  *
	  * @returns The adjusted damage amount
	  */
	virtual float ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;

	// End GameMode interface

	/** 
	 * Helper function to test teams (null = not in friendly team). 
	 *
	 * @param ActorA		First actor to test against
	 * @param ActorB		Second actor to test against
	 *
	 * @return true if the two actors are on the same team.
	 */
	static bool OnFriendlyTeam(const AActor* ActorA, const AActor* ActorB);

	/** 
	 * Helper function to test teams (null = not in friendly team). 
	 *
	 * @param ActorA		First actor to test against
	 * @param ActorB		Second actor to test against
	 *
	 * @return true if the two actors are on opposing teams.
	 */	
	static bool OnEnemyTeam(const AActor* ActorA, const AActor* ActorB);

	/** Pointer to title text widget. */
	TSharedPtr<SStrategyTitle> StrategyTitle;

	/** select best spawn point for player */
	virtual class AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	/** check if player can use spawnpoint */
	virtual bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const;

	/** check if player should use spawnpoint */
	virtual bool IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Player) const;

protected:
	/* Helper to return the current gameplay state. */
	EGameplayState::Type GetGameplayState() const;

	/** Handle for efficient management of UpdateHealth timer */
	FTimerHandle TimerHandle_ReturnToMenu;
};

