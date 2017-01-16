// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "TowerSensingComponent.h"

UTowerSensingComponent::UTowerSensingComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SensingInterval = 0.2f;
	SightRadius = 1000.0f;
	SetPeripheralVisionAngle(180.0f);
	bOnlySensePlayers = false;
	bHearNoises = false;
	bSeePawns = true;
	SightDistance = 500.0f;
}

void UTowerSensingComponent::InitializeComponent()
{
	Super::InitializeComponent();
	// set custom data from config file
	//SightRadius = SightDistance;
}

bool UTowerSensingComponent::ShouldCheckVisibilityOf(APawn *Pawn) const
{
	AMobaAICharacter* const TestChar = Cast<AMobaAICharacter>(Pawn);
	return TestChar != nullptr && !TestChar->bHidden && TestChar->CombatComponent->Health > 0 && AStrategyGameMode::OnEnemyTeam(Pawn, GetOwner());
}

bool UTowerSensingComponent::CanSenseAnything() const
{
	return SightRadius > 0.0f;
}

void UTowerSensingComponent::UpdateAISensing()
{
	const AActor* const Owner = GetOwner();
	if (!IsValid(Owner) || (Owner->GetWorld() == NULL))
	{
		// Cannot sense without a valid owner in the world.
		return;
	}

	for (FConstPawnIterator Iterator = Owner->GetWorld()->GetPawnIterator(); Iterator; ++Iterator)
	{
		AMobaTestCharacter * const Player = Cast<AMobaTestCharacter>(*Iterator);
		if (Player) //its a player
		{
			if (AStrategyGameMode::OnEnemyTeam(Player, GetOwner()))
			{
				if (CouldSeePawn(Player, true))
				{
					KnownTargets.AddUnique(Player);
				}
				else
					KnownTargets.Remove(Player);
			}
		}
		else
		{
			AMobaAICharacter* const TestChar = Cast<AMobaAICharacter>(*Iterator);
			if (TestChar)
			{
				if (!IsSensorActor(TestChar) && ShouldCheckVisibilityOf(TestChar))
				{
					if (CouldSeePawn(TestChar, true))
					{
						KnownTargets.AddUnique(TestChar);
					}
				}
			}
				
		}


	}

	for (int32 i = KnownTargets.Num() - 1; i >= 0; i--)
	{
		const AMobaAICharacter* TestChar = Cast<AMobaAICharacter>(KnownTargets[i].Get());
		const AMobaTestCharacter* TestPlayer = Cast<AMobaTestCharacter>(KnownTargets[i].Get());
		if (TestChar == NULL && TestPlayer == NULL)
		{
			KnownTargets.RemoveAt(i);
		}
	}
}


