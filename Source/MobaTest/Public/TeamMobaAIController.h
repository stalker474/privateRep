// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AI/MobaAIController.h"
#include "TeamMobaAIController.generated.h"

/**
 * 
 */
UCLASS()
class MOBATEST_API ATeamMobaAIController : public AMobaAIController, public IStrategyTeamInterface
{
	GENERATED_UCLASS_BODY()

public:

	// Begin StrategyTeamInterface Interface
	UFUNCTION(BlueprintCallable, Category = "Team")
		virtual uint8 GetTeamNum() const override;
	// End StrategyTeamInterface Interface

	UPROPERTY(Replicated, EditInstanceOnly, Category = Building)
		TEnumAsByte<EStrategyTeam::Type> TeamNum;

	bool IsTargetValid(AActor* InActor) const;

	void Possess(APawn* inPawn) override;
};
