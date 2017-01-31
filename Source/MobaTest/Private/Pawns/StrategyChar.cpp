// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MobaTest.h"
#include "StrategyAIController.h"
#include "StrategyAttachment.h"

#include "VisualLogger/VisualLogger.h"

AStrategyChar::AStrategyChar(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent>(ACharacter::CharacterMovementComponentName)) 
{
}

void AStrategyChar::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AStrategyChar, MyTeamNum);
}

uint8 AStrategyChar::GetTeamNum() const
{
	return MyTeamNum;
}

void AStrategyChar::SetTeamNum(uint8 NewTeamNum)
{
	MyTeamNum = NewTeamNum;
}
