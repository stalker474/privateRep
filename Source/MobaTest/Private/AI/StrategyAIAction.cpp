// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MobaTest.h"
#include "StrategyAIAction.h"
#include "StrategyAIController.h"


UStrategyAIAction::UStrategyAIAction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bIsExecuted(false)
{
}

bool UStrategyAIAction::Tick(float DeltaTime) 
{ 
	return false; 
}

bool UStrategyAIAction::ShouldActivate() const 
{ 
	return false; 
}

void UStrategyAIAction::Abort() 
{
	bIsExecuted = false; 
}

bool UStrategyAIAction::IsSafeToAbort() const 
{ 
	return true; 
}

void UStrategyAIAction::Activate()
{ 		
	bIsExecuted = true; 
}

void UStrategyAIAction::SetController(ABaseAIController* InOwner)
{ 
	MyAIController = InOwner; 
}
