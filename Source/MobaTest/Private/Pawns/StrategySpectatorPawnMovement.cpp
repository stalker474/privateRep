// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.


#include "MobaTest.h"
#include "StrategySpectatorPawnMovement.h"
#include "StrategyCameraComponent.h"
#include "StrategySpectatorPawn.h"


UStrategySpectatorPawnMovement::UStrategySpectatorPawnMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), bInitialLocationSet(false)
{
	MaxSpeed  = 16000.f;
	Acceleration = 5000.f;
	Deceleration = 4000.f;
}


void UStrategySpectatorPawnMovement::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PawnOwner || !UpdatedComponent)
	{
		return;
	}
}

