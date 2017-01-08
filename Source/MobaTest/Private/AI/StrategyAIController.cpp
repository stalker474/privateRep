// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MobaTest.h"
#include "StrategyAIController.h"
#include "StrategyAIAction.h"
#include "StrategyAISensingComponent.h"
#include "StrategyAIAction_AttackTarget.h"
#include "StrategyAIAction_MoveToBrewery.h"
#include "MobaTestCharacter.h"
#include "MobaTower.h"

#include "VisualLogger/VisualLogger.h"

/*
 * Main AI Controller class 
 */
AStrategyAIController::AStrategyAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// add default action for most units
	AllowedActions.Add(UStrategyAIAction_AttackTarget::StaticClass());
	AllowedActions.Add(UStrategyAIAction_MoveToBrewery::StaticClass());
}

struct FPlayerData* AStrategyAIController::GetTeamData() const
{
	return GetWorld()->GetGameState<AStrategyGameState>()->GetPlayerData(GetTeamNum());
}

uint8 AStrategyAIController::GetTeamNum() const
{
	AStrategyChar* const MyChar = Cast<AStrategyChar>(GetPawn());
	return (MyChar != NULL) ? MyChar->GetTeamNum() : EStrategyTeam::Spectator;
}

bool AStrategyAIController::IsTargetValid(AActor* InActor) const
{
	// try to find a character
	const AStrategyChar* testChar = Cast<AStrategyChar>(InActor);
	const AMobaTestCharacter* testPlayer = Cast<AMobaTestCharacter>(InActor);
	const AMobaTower* testTower = Cast<AMobaTower>(InActor);
	if (testChar == NULL)
	{
		// if Actor is a Controller, try to use its pawn
		const AStrategyAIController* const Controller = Cast<AStrategyAIController>(InActor);
		if (Controller != NULL)
		{
			testChar = Cast<AStrategyChar>(Controller->GetPawn());
		}
	}
	if (testChar)
		return  (testChar->CombatComponent->Health > 0) && AStrategyGameMode::OnEnemyTeam(testChar, this);
	else if (testPlayer)
		return ((testPlayer->CombatCharacterComponent->Health > 0) && AStrategyGameMode::OnEnemyTeam(testPlayer, this));
	else if (testTower)
		return ((testTower->CombatComponent->Health > 0) && AStrategyGameMode::OnEnemyTeam(testTower, this));

	return false;
}

float AStrategyAIController::ComputeScore(const APawn * TestPawn)
{
	const FVector PawnLocation = TestPawn->GetActorLocation();
	float TargetScore = (PawnLocation - TestPawn->GetActorLocation()).SizeSquared();
	if (CurrentTarget == TestPawn && TestPawn->IsA(AStrategyChar::StaticClass()))
	{
		TargetScore -= FMath::Square(300.0f);
	}
	const ABaseAIController *AITarget = (TestPawn ? Cast<ABaseAIController>(TestPawn->Controller) : NULL);
	if (AITarget != NULL)
	{
		if (AITarget->IsClaimedBy(this))
		{
			TargetScore -= FMath::Square(300.0f);
		}
		else
		{
			TargetScore += AITarget->GetNumberOfAttackers() * FMath::Square(900.0f);
		}
	}
	return TargetScore;
}


