// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "TeamMobaAICharacter.h"

ATeamMobaAICharacter::ATeamMobaAICharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

uint8 ATeamMobaAICharacter::GetTeamNum() const
{
	IStrategyTeamInterface * owner = Cast<IStrategyTeamInterface>(GetOwner());
	if (owner)
		return owner->GetTeamNum();
	else
		int k = 6;

	return 0;
}


