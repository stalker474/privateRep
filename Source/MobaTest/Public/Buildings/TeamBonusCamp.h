// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TeamBonusCampAIDirector.h"
#include "Buildings/BonusCamp.h"
#include "TeamBonusCamp.generated.h"

/**
 * 
 */
UCLASS()
class MOBATEST_API ATeamBonusCamp : public ABonusCamp, public IStrategyTeamInterface
{
	GENERATED_UCLASS_BODY()

public:
	
	// Begin StrategyTeamInterface Interface
	UFUNCTION(BlueprintCallable, Category = "Team")
	virtual uint8 GetTeamNum() const override;
	// End StrategyTeamInterface Interface

	UPROPERTY(Replicated, EditInstanceOnly, Category = Building)
	TEnumAsByte<EStrategyTeam::Type> TeamNum;

};
