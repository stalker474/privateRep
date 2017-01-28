// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pawns/MobaAICharacter.h"
#include "TeamMobaAICharacter.generated.h"

/**
 * 
 */
UCLASS()
class MOBATEST_API ATeamMobaAICharacter : public AMobaAICharacter, public IStrategyTeamInterface
{
	GENERATED_UCLASS_BODY()

public:

	// Begin StrategyTeamInterface Interface
	UFUNCTION(BlueprintCallable, Category = "Team")
		virtual uint8 GetTeamNum() const override;
	// End StrategyTeamInterface Interface
};