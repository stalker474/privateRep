// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "StrategyAIAction.h"
#include "StrategyAISensingComponent.h"
#include "StrategyAIAction_AttackTarget.h"
#include "MobaTestCharacter.h"
#include "MobaAIController.h"

/*
* Main AI Controller class
*/
AMobaAIController::AMobaAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// add default action for most units
	AllowedActions.Add(UStrategyAIAction_AttackTarget::StaticClass());
}

bool AMobaAIController::IsTargetValid(AActor* InActor) const
{
	// try to find a character
	const AMobaTestCharacter* testPlayer = Cast<AMobaTestCharacter>(InActor);
	if (testPlayer)
		return ((testPlayer->CombatCharacterComponent->Health > 0) && AStrategyGameMode::OnEnemyTeam(testPlayer, this));
	return false;
}

float AMobaAIController::ComputeScore(const APawn * TestPawn)
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
