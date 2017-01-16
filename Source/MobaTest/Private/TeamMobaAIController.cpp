// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "TeamMobaAIController.h"

ATeamMobaAIController::ATeamMobaAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TeamNum(EStrategyTeam::Spectator)
{
}

void ATeamMobaAIController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATeamMobaAIController, TeamNum);
}

uint8 ATeamMobaAIController::GetTeamNum() const
{
	return TeamNum;
}

bool ATeamMobaAIController::IsTargetValid(AActor* InActor) const
{
	if (!AStrategyGameMode::OnEnemyTeam(GetPawn(),InActor))
		return false;

	return AMobaAIController::IsTargetValid(InActor);
}

void ATeamMobaAIController::Possess(APawn * inPawn)
{
	IStrategyTeamInterface * teamPawn = Cast<IStrategyTeamInterface>(inPawn);
	if (teamPawn)
		TeamNum = TEnumAsByte<EStrategyTeam::Type>(teamPawn->GetTeamNum());
	ABaseAIController::Possess(inPawn);
}


