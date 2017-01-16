// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "TeamBonusCamp.h"

ATeamBonusCamp::ATeamBonusCamp(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TeamNum(EStrategyTeam::Spectator)
{
}

void ATeamBonusCamp::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATeamBonusCamp, TeamNum);
}

uint8 ATeamBonusCamp::GetTeamNum() const
{
	return TeamNum;
}
