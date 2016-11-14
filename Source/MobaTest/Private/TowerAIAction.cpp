// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "TowerAIAction.h"
#include "TowerAIController.h"


UTowerAIAction::UTowerAIAction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bIsExecuted(false)
{
}

bool UTowerAIAction::Tick(float DeltaTime)
{
	return false;
}

bool UTowerAIAction::ShouldActivate() const
{
	return false;
}

void UTowerAIAction::Abort()
{
	bIsExecuted = false;
}

bool UTowerAIAction::IsSafeToAbort() const
{
	return true;
}

void UTowerAIAction::Activate()
{
	bIsExecuted = true;
}

void UTowerAIAction::SetController(ATowerAIController* InOwner)
{
	MyAIController = InOwner;
}



