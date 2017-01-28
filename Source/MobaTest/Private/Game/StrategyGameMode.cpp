// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MobaTest.h"
#include "StrategyBuilding.h"
#include "StrategySpectatorPawn.h"
#include "StrategyTeamInterface.h"
#include "MobaTestCharacter.h"
#include "StrategySpawnPoint.h"
#include "MobaPlayerState.h"
#include "StrategyTeamInterface.h"
#include "MobaPlayerController.h"

DEFINE_LOG_CATEGORY(MobaGameModeLog);


AStrategyGameMode::AStrategyGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultPawnClass = AStrategySpectatorPawn::StaticClass();
	GameStateClass = AStrategyGameState::StaticClass();
	DefaultCharacterClass = nullptr;
	InGameHUDClass = nullptr;

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Characters/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultCharacterClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<AHUD> PlayerHUDBPClass(TEXT("/Game/UI/HUD/MobaHUD"));
	if (PlayerHUDBPClass.Class != NULL)
	{
		InGameHUDClass = PlayerHUDBPClass.Class;
	}

	PlayerStateClass = AMobaPlayerState::StaticClass();
	PlayerControllerClass = AMobaPlayerController::StaticClass();

	BlueCount = 0;
	RedCount = 0;
	BlueReady = 0;
	RedReady = 0;

	bUseSeamlessTravel = true;
}

// internal
EGameplayState::Type AStrategyGameMode::GetGameplayState() const
{
	return GetGameState<AStrategyGameState>()->GameplayState;
}


void AStrategyGameMode::InitGameState()
{
	Super::InitGameState();
	AStrategyGameState* const gameState = GetGameState<AStrategyGameState>();
	if (gameState)
	{
		// start the game!
		gameState->StartGameplayStateMachine();
	}
}

UClass* AStrategyGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	AMobaPlayerState * ps = Cast<AMobaPlayerState>(InController->PlayerState);
	if (GetGameplayState() == EGameplayState::ChosingCharacter)
		return DefaultPawnClass;
	else
	{
		APlayerController * pc = Cast<APlayerController>(InController);
		pc->ClientSetHUD(InGameHUDClass);
	
		if (ps->SelectedCharClass)
			return ps->SelectedCharClass;
		if (DefaultCharacterClass)
			return DefaultCharacterClass;
		else return DefaultPawnClass;
	}
	
}

void AStrategyGameMode::Ready(AController* Player)
{
	AMobaPlayerState * ps = Cast<AMobaPlayerState>(Player->PlayerState);
	ps->State = EMobaState::Waiting;
	if (ps->MyTeamNum == EStrategyTeam::Blue)
		BlueReady++;
	else
		RedReady++;

	if (BlueReady == BlueCount && RedReady == RedCount)
	{
		GetGameState<AStrategyGameState>()->GameplayState = EGameplayState::Joining;
		GetWorld()->ServerTravel("/Game/Tower_Defence/Maps/test/demo");
	}
}

AActor* AStrategyGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<APlayerStart*> PreferredSpawns;
	AMobaPlayerState * ps = Cast<AMobaPlayerState>(Player->PlayerState);
	UWorld* World = GetWorld();
	if (ps->State == EMobaState::ChosingCharacter && GetGameplayState() == EGameplayState::ChosingCharacter)
	{
		if (BlueCount < RedCount)
		{
			ps->MyTeamNum = EStrategyTeam::Blue;
			BlueCount++;
		}
		else
		{
			ps->MyTeamNum = EStrategyTeam::Red;
			RedCount++;
		}
	}

	AStrategyGameState * gs = Cast<AStrategyGameState>(GameState);
	gs->NewPlayerJoined(Player);

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
	AStrategySpawnPoint * sp = Cast<AStrategySpawnPoint>(BestStart);
	if(ps->State == EMobaState::ChosingCharacter)
		sp->MULTICAST_PlayerChanged(Player->GetName());
	return BestStart ? BestStart : Super::ChoosePlayerStart(Player);
}

void AStrategyGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

bool AStrategyGameMode::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const
{
	EGameplayState::Type gs = GetGameplayState();
	AStrategySpawnPoint* spawnPoint = Cast<AStrategySpawnPoint>(SpawnPoint);
	if (spawnPoint)
	{
		AMobaPlayerState * ps = Cast<AMobaPlayerState>(Player->PlayerState);
		if (gs == EGameplayState::ChosingCharacter && (spawnPoint->GetTeamNum() == ps->GetTeam()) && !spawnPoint->IsSpawnable)
			return true;
		else if (gs == EGameplayState::Joining && (spawnPoint->GetTeamNum() == ps->GetTeam()) && spawnPoint->IsSpawnable)
			return true;
	}

	return false;
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

	if( (TeamA != nullptr && TeamA->GetTeamNum() == EStrategyTeam::Spectator) || (TeamB != nullptr && TeamB->GetTeamNum() == EStrategyTeam::Spectator))
		return true;

	return (TeamA != nullptr) && (TeamB != nullptr) && (TeamA->GetTeamNum() == TeamB->GetTeamNum());
}

bool AStrategyGameMode::OnEnemyTeam(const AActor* ActorA, const AActor* ActorB)
{
	const IStrategyTeamInterface* TeamA = Cast<const IStrategyTeamInterface>(ActorA);
	const IStrategyTeamInterface* TeamB = Cast<const IStrategyTeamInterface>(ActorB);

	if (TeamA == nullptr || TeamB == nullptr) //if one has no team, enemies
		return true;

	if( (TeamA != nullptr && TeamA->GetTeamNum() == EStrategyTeam::Spectator) || (TeamB != nullptr && TeamB->GetTeamNum() == EStrategyTeam::Spectator))
		return false;

	return (TeamA != nullptr) && (TeamB != nullptr) && (TeamA->GetTeamNum() != TeamB->GetTeamNum());
}





