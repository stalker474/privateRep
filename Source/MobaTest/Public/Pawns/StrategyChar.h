// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Public/StrategyTypes.h"
#include "Pawns/MobaAICharacter.h"
#include "Public/Interfaces/StrategyTeamInterface.h"
#include "StrategyChar.generated.h"

class UStrategyAttachment;

// Base class for the minions
UCLASS(Abstract)
class AStrategyChar : public AMobaAICharacter, public IStrategyTeamInterface
{
	GENERATED_UCLASS_BODY()

public:

	// Begin StrategyTeamInterface interface
	UFUNCTION(BlueprintCallable, Category = "Team")
	virtual uint8 GetTeamNum() const override;
	// End StrategyTeamInterface interface

	/** set team number */
	void SetTeamNum(uint8 NewTeamNum); 

	ELaneEnum LanePosition;

protected:

	/** team number */
	UPROPERTY(Replicated)
	uint8 MyTeamNum;
};

