// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MobaTest.h"
#include "StrategyBuilding.h"
#include "StrategySpectatorPawn.h"
#include "StrategyTeamInterface.h"
#include "MobaTestCharacter.h"
#include "StrategySpawnPoint.h"


AStrategyGameMode::AStrategyGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultPawnClass = AStrategySpectatorPawn::StaticClass();
	GameStateClass = AStrategyGameState::StaticClass();

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Characters/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	/*static ConstructorHelpers::FClassFinder<AStrategyBuilding> EmptyWallSlotHelper(TEXT("/Game/Buildings/Wall/Wall_EmptySlot"));
	EmptyWallSlotClass = EmptyWallSlotHelper.Class;
	if ((GEngine != nullptr) && (GEngine->GameViewport != nullptr))
	{
		GEngine->GameViewport->SetSuppressTransitionMessage(true);
	}*/
}

// internal
EGameplayState::Type AStrategyGameMode::GetGameplayState() const
{
	return GetGameState<AStrategyGameState>()->GameplayState;
}


void AStrategyGameMode::InitGameState()
{
	Super::InitGameState();

	AStrategyGameState* const GameState = GetGameState<AStrategyGameState>();
	if (GameState)
	{
		// start the game!
		GameState->StartGameplayStateMachine();
	}
}

float AStrategyGameMode::ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	// no health changes after game is finished
	if (GetGameplayState() == EGameplayState::Finished)
	{
		return 0.0f;
	}
	
	if (Damage > 0.f)
	{
		const IStrategyTeamInterface* VictimTeam = Cast<IStrategyTeamInterface>(DamagedActor);
		IStrategyTeamInterface* InstigatorTeam = Cast<IStrategyTeamInterface>(EventInstigator);
		if (InstigatorTeam == nullptr)
		{
			InstigatorTeam = Cast<IStrategyTeamInterface>(DamageCauser);
		}

		// skip friendly fire
		if (InstigatorTeam && VictimTeam && InstigatorTeam->GetTeamNum() == VictimTeam->GetTeamNum())
		{
			return 0.0f;
		}

		// pawn's damage reduction
		const AStrategyChar* DamagedChar = Cast<AStrategyChar>(DamagedActor);
		if (DamagedChar)
		{
			Damage -= DamagedChar->GetPawnData()->DamageReduction;
		}
	}

	return Damage;
}

AActor* AStrategyGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<APlayerStart*> PreferredSpawns;
	
	UWorld* World = GetWorld();


	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		APlayerStart* Start = *It;
		if (IsSpawnpointAllowed(Start, Player))
		{
			if (IsSpawnpointPreferred(Start, Player))
			{
				PreferredSpawns.Add(Start);
			}
		}
	}

	APlayerStart* BestStart = NULL;
	if (PreferredSpawns.Num() > 0)
	{
		BestStart = PreferredSpawns[FMath::RandHelper(PreferredSpawns.Num())];
	}

	return BestStart ? BestStart : Super::ChoosePlayerStart(Player);
}

bool AStrategyGameMode::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const
{
	AStrategySpawnPoint* spawnPoint = Cast<AStrategySpawnPoint>(SpawnPoint);
	if (spawnPoint)
	{
		
		/*if (spawnPoint->GetTeamNum() != )
		{
			return false;
		}*/
	}

	return true;
}

bool AStrategyGameMode::IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Player) const
{
	ACharacter* MyPawn = Player ? Cast<ACharacter>(Player->GetPawn()) : NULL;
	if (MyPawn)
	{
		const FVector SpawnLocation = SpawnPoint->GetActorLocation();
		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
		{
			ACharacter* OtherPawn = Cast<ACharacter>(*It);
			if (OtherPawn && OtherPawn != MyPawn)
			{
				const float CombinedHeight = (MyPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + OtherPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) * 2.0f;
				const float CombinedRadius = MyPawn->GetCapsuleComponent()->GetScaledCapsuleRadius() + OtherPawn->GetCapsuleComponent()->GetScaledCapsuleRadius();
				const FVector OtherLocation = OtherPawn->GetActorLocation();

				// check if player start overlaps this pawn
				if (FMath::Abs(SpawnLocation.Z - OtherLocation.Z) < CombinedHeight && (SpawnLocation - OtherLocation).Size2D() < CombinedRadius)
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool AStrategyGameMode::OnFriendlyTeam(const AActor* ActorA, const AActor* ActorB)
{
	const IStrategyTeamInterface* TeamA = Cast<const IStrategyTeamInterface>(ActorA);
	const IStrategyTeamInterface* TeamB = Cast<const IStrategyTeamInterface>(ActorB);

	if( (TeamA != nullptr && TeamA->GetTeamNum() == EStrategyTeam::Unknown) || (TeamB != nullptr && TeamB->GetTeamNum() == EStrategyTeam::Unknown))
		return true;

	return (TeamA != nullptr) && (TeamB != nullptr) && (TeamA->GetTeamNum() == TeamB->GetTeamNum());
}

bool AStrategyGameMode::OnEnemyTeam(const AActor* ActorA, const AActor* ActorB)
{
	const IStrategyTeamInterface* TeamA = Cast<const IStrategyTeamInterface>(ActorA);
	const IStrategyTeamInterface* TeamB = Cast<const IStrategyTeamInterface>(ActorB);

	if( (TeamA != nullptr && TeamA->GetTeamNum() == EStrategyTeam::Unknown) || (TeamB != nullptr && TeamB->GetTeamNum() == EStrategyTeam::Unknown))
		return false;

	return (TeamA != nullptr) && (TeamB != nullptr) && (TeamA->GetTeamNum() != TeamB->GetTeamNum());
}





