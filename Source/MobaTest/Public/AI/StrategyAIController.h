// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "BaseAIController.h"
#include "StrategyTeamInterface.h"
#include "StrategyAIController.generated.h"

UCLASS()
class AStrategyAIController : public ABaseAIController, public IStrategyTeamInterface
{
	GENERATED_UCLASS_BODY()



public:
	// Begin StrategyTeamInterface Interface
	virtual uint8 GetTeamNum() const override;
	// End StrategyTeamInterface Interface

	/** get data for current team */
	struct FPlayerData* GetTeamData() const;

	/** Checks actor and returns true if valid */
	bool IsTargetValid(AActor* InActor) const override;

protected:
	virtual float ComputeScore(const APawn* TestPawn) override;
};
