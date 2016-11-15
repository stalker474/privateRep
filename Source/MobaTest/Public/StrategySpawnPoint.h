// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerStart.h"
#include "StrategySelectionInterface.h"
#include "StrategySpawnPoint.generated.h"

/**
 * 
 */
UCLASS()
class MOBATEST_API AStrategySpawnPoint : public APlayerStart, public IStrategyTeamInterface
{
	GENERATED_BODY()
	
		/** team */
	UPROPERTY(EditInstanceOnly, Category = Building)
	TEnumAsByte<EStrategyTeam::Type> SpawnTeamNum;
	
	// Begin StrategyTeamInterface interface

	/** [IStrategyTeamInterface] get team number */
	virtual uint8 GetTeamNum() const override;
	// End StrategyTeamInterface interface
};
