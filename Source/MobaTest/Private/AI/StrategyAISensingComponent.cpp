// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MobaTest.h"
#include "StrategyAISensingComponent.h"
#include "MobaTestCharacter.h"
#include "MobaTower.h"

UStrategyAISensingComponent::UStrategyAISensingComponent(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	SensingInterval = 0.2f;
	SightRadius = 2200.0f;
	SetPeripheralVisionAngle(180.0f);
	bOnlySensePlayers = false;
	bHearNoises = false;
	bSeePawns = true;
}

void UStrategyAISensingComponent::InitializeComponent()
{
	Super::InitializeComponent();
	// set custom data from config file
	//SightRadius = SightDistance;
}

bool UStrategyAISensingComponent::ShouldCheckVisibilityOf(APawn *Pawn) const
{
	AStrategyChar* const testChar = Cast<AStrategyChar>(Pawn);
	AMobaTower* const mobaTower = Cast<AMobaTower>(Pawn);
	if (testChar)
		return !testChar->bHidden && testChar->CombatComponent->Health > 0 && AStrategyGameMode::OnEnemyTeam(Pawn, GetOwner());
	else if (mobaTower)
	{
		return !mobaTower->bHidden && mobaTower->CombatComponent->Health > 0 && AStrategyGameMode::OnEnemyTeam(Pawn, GetOwner());
	}
	else
		return false;
}

bool UStrategyAISensingComponent::CanSenseAnything() const
{
	return SightRadius > 0.0f;
}

void UStrategyAISensingComponent::UpdateAISensing()
{
	const AActor* const Owner = GetOwner();
	if (!IsValid(Owner) || (Owner->GetWorld() == NULL))
	{
		// Cannot sense without a valid owner in the world.
		return;
	}

	for (FConstPawnIterator Iterator = Owner->GetWorld()->GetPawnIterator(); Iterator; ++Iterator)
	{
		AMobaTestCharacter * const player = Cast<AMobaTestCharacter>(*Iterator);
		AStrategyChar* const charHolop = Cast<AStrategyChar>(*Iterator);
		AMobaTower* const mobaTower = Cast<AMobaTower>(*Iterator);
		if (player) //its a player
		{
			if (AStrategyGameMode::OnEnemyTeam(player, GetOwner()))
			{
				if (CouldSeePawn(player, true))
				{
					KnownTargets.AddUnique(player);
				}
			}
		}
		else if(charHolop)
		{
			if (!IsSensorActor(charHolop) && ShouldCheckVisibilityOf(charHolop))
			{
				if (CouldSeePawn(charHolop, true))
				{
					KnownTargets.AddUnique(charHolop);
				}
			}
		}
		else if (mobaTower)
		{
			if (!IsSensorActor(mobaTower) && ShouldCheckVisibilityOf(mobaTower))
			{
				if (CouldSeePawn(mobaTower, true))
				{
					KnownTargets.AddUnique(mobaTower);
				}
			}
		}

		
	}

	for (int32 i = KnownTargets.Num() - 1; i >= 0; i--)
	{
		const AStrategyChar* charPawn = Cast<AStrategyChar>(KnownTargets[i].Get());
		const AMobaTestCharacter* player = Cast<AMobaTestCharacter>(KnownTargets[i].Get());
		const AMobaTower* tower = Cast<AMobaTower>(KnownTargets[i].Get());
		if (charPawn == NULL && player == NULL && tower == NULL)
		{
			KnownTargets.RemoveAt(i);
		}
	}
}
