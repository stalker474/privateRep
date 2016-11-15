// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MobaTest.h"
#include "StrategyAIController.h"
#include "StrategyAIAction.h"
#include "StrategyAISensingComponent.h"
#include "StrategyAIAction_AttackTarget.h"
#include "StrategyAIAction_MoveToBrewery.h"
#include "MobaTestCharacter.h"
#include "MobaTower.h"

#include "VisualLogger/VisualLogger.h"


DEFINE_LOG_CATEGORY(LogStrategyAI);

/*
 * Main AI Controller class 
 */
AStrategyAIController::AStrategyAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bLogicEnabled(true)
{
	SensingComponent = CreateDefaultSubobject<UStrategyAISensingComponent>(TEXT("SensingComp"));

	// add default action for most units
	AllowedActions.Add(UStrategyAIAction_AttackTarget::StaticClass());
	AllowedActions.Add(UStrategyAIAction_MoveToBrewery::StaticClass());
}

struct FPlayerData* AStrategyAIController::GetTeamData() const
{
	return GetWorld()->GetGameState<AStrategyGameState>()->GetPlayerData(GetTeamNum());
}

void AStrategyAIController::Possess(APawn* inPawn)
{
	Super::Possess(inPawn);
	
	/** Create instances of our possible actions */
	AllActions.Reset();
	for(int32 Idx=0; Idx < AllowedActions.Num(); Idx++ )
	{
		UStrategyAIAction* Action = NewObject<UStrategyAIAction>(this, AllowedActions[Idx]);
		check(Action);
		Action->SetController(this);
		AllActions.Add(Action);
	}

	AStrategyChar* const MyChar = Cast<AStrategyChar>(GetPawn());
	if (MyChar != NULL && MyChar->GetCharacterMovement() != NULL)
	{
		MyChar->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	SetActorTickEnabled(true);
	EnableLogic(true);
}

void AStrategyAIController::UnPossess()
{
	if (CurrentTarget != NULL)
	{
		const APawn* TargetPawn = Cast<APawn>(CurrentTarget);
		if (TargetPawn != NULL)
		{
			AStrategyAIController* const AITarget = Cast<AStrategyAIController>(TargetPawn->Controller);
			if (AITarget != NULL)
			{
				AITarget->UnClaimAsTarget(this);
			}
		}
	}

	SetActorTickEnabled(false);
	EnableLogic(false);
	Super::UnPossess();
}

uint8 AStrategyAIController::GetTeamNum() const
{
	AStrategyChar* const MyChar = Cast<AStrategyChar>(GetPawn());
	return (MyChar != NULL) ? MyChar->GetTeamNum() : EStrategyTeam::Unknown;
}

void AStrategyAIController::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (CurrentAction != NULL && Result != EPathFollowingResult::Skipped)
	{
		OnMoveCompletedDelegate.ExecuteIfBound();
	}
}

/** Pawn has hit something */
void AStrategyAIController::NotifyBump(FHitResult const& Hit)
{
	if (CurrentAction != NULL && OnNotifyBumpDelegate.IsBound())
	{
		OnNotifyBumpDelegate.Execute(Hit);
	}
}

bool AStrategyAIController::IsActionAllowed(TSubclassOf<UStrategyAIAction> inClass) const
{
	for (int32 Idx=0; Idx < AllowedActions.Num(); Idx++)
	{
		if (AllowedActions[Idx] == inClass)
		{
			return true;
		}
	}

	return false;
}

UStrategyAIAction* AStrategyAIController::GetInstanceOfAction(TSubclassOf<UStrategyAIAction> inClass) const
{
	int32 Idx = 0;
	for (Idx=0; Idx < AllowedActions.Num(); Idx++)
	{
		if (AllowedActions[Idx] == inClass)
		{
			break;
		}
	}

	if (Idx < AllActions.Num() && AllActions[Idx]->GetClass() == inClass)
	{
		return AllActions[Idx];
	}

	return NULL;
}

bool AStrategyAIController::IsTargetValid(AActor* InActor) const
{
	// try to find a character
	const AStrategyChar* testChar = Cast<AStrategyChar>(InActor);
	const AMobaTestCharacter* testPlayer = Cast<AMobaTestCharacter>(InActor);
	const AMobaTower* testTower = Cast<AMobaTower>(InActor);
	if (testChar == NULL)
	{
		// if Actor is a Controller, try to use its pawn
		const AStrategyAIController* const Controller = Cast<AStrategyAIController>(InActor);
		if (Controller != NULL)
		{
			testChar = Cast<AStrategyChar>(Controller->GetPawn());
		}
	}
	if (testChar)
		return  (testChar->CombatComponent->Health > 0) && AStrategyGameMode::OnEnemyTeam(testChar, this);
	else if (testPlayer)
		return ((testPlayer->CombatCharacterComponent->Health > 0) && AStrategyGameMode::OnEnemyTeam(testPlayer, this));
	else if (testTower)
		return ((testTower->CombatComponent->Health > 0) && AStrategyGameMode::OnEnemyTeam(testTower, this));

	return false;
}

void AStrategyAIController::SelectTarget()
{
	if( GetPawn() == NULL )
	{
		return;
	}
	const FVector PawnLocation = GetPawn()->GetActorLocation();
	AActor* BestUnit = NULL;
	float BestUnitScore = 10000;

	for (int32 Idx = 0; Idx < SensingComponent->KnownTargets.Num(); Idx++)
	{
		AActor* const TestTarget = SensingComponent->KnownTargets[Idx].Get();
		if (TestTarget == NULL || !IsTargetValid(TestTarget) )
		{
			continue;
		}

		/** don't care about targets with disabled logic */
		const APawn* TestPawn = Cast<APawn>(TestTarget);
		const AStrategyAIController *AITarget = (TestPawn ? Cast<AStrategyAIController>(TestPawn->Controller) : NULL);
		if (AITarget != NULL && !AITarget->IsLogicEnabled())
		{
			continue;
		}

		float TargetScore = (PawnLocation - TestTarget->GetActorLocation()).SizeSquared();
		if (CurrentTarget == TestTarget && TestTarget->IsA(AStrategyChar::StaticClass()) )
		{
			TargetScore -= FMath::Square(300.0f);
		}

		if (AITarget != NULL)
		{
			if (AITarget->IsClaimedBy(this))
			{
				TargetScore -= FMath::Square(300.0f);
			}
			else
			{
				TargetScore += AITarget->GetNumberOfAttackers() * FMath::Square(900.0f);
			}
		}

		if (BestUnit == NULL || BestUnitScore > TargetScore)
		{
			BestUnitScore = TargetScore;
			BestUnit = TestTarget;
		}
	}

	const AActor* OldTarget = CurrentTarget;
	CurrentTarget = BestUnit;
	if (CurrentTarget != NULL && OldTarget != CurrentTarget)
	{
		const APawn* OldTargetPawn = Cast<const APawn>(OldTarget);
		AStrategyAIController* AITarget = OldTargetPawn != NULL ? Cast<AStrategyAIController>(OldTargetPawn->Controller) : NULL;
		if (AITarget != NULL)
		{
			AITarget->UnClaimAsTarget(this);
		}

		APawn* const CurrentTargetPawn = Cast<APawn>(CurrentTarget);
		AITarget = (CurrentTargetPawn ? Cast<AStrategyAIController>(CurrentTargetPawn->Controller) : NULL);
		if (AITarget != NULL)
		{
			AITarget->ClaimAsTarget(this);
		}
	}

	UE_VLOG(this, LogStrategyAI, Log, TEXT("Selected target: %s"), CurrentTarget != NULL ? *CurrentTarget->GetName() : TEXT("NONE") ); 
}

void AStrategyAIController::ClaimAsTarget(TWeakObjectPtr<AStrategyAIController> InController)
{
	ClaimedBy.AddUnique(InController);
}

void AStrategyAIController::UnClaimAsTarget(TWeakObjectPtr<AStrategyAIController> InController)
{
	for (int32 Idx = ClaimedBy.Num() - 1; Idx >= 0; Idx--)
	{
		const AStrategyAIController* TestC = Cast<AStrategyAIController>(ClaimedBy[Idx].Get());
		if (TestC != NULL && TestC == InController.Get())
		{
			ClaimedBy.RemoveAt(Idx);
		}
	}
}

bool AStrategyAIController::IsClaimedBy(TWeakObjectPtr<AStrategyAIController> InController) const
{
	for (int32 Idx = ClaimedBy.Num() - 1; Idx >= 0; Idx--)
	{
		const AStrategyAIController* TestC = Cast<AStrategyAIController>(ClaimedBy[Idx].Get());
		if (TestC != NULL && TestC == InController.Get())
		{
			return true;
		}
	}

	return false;
}

int32 AStrategyAIController::GetNumberOfAttackers() const
{
	return ClaimedBy.Num();
}

void AStrategyAIController::Tick(float DeltaTime)
{
	const AStrategyChar* MyChar = Cast<AStrategyChar>(GetPawn());
	if (!IsLogicEnabled() || MyChar == NULL || (MyChar != NULL && MyChar->CombatComponent->Health <= 0))
	{
		if (!IsLogicEnabled())
		{
			if (CurrentAction != NULL)
			{
				CurrentAction->Abort();
				CurrentAction= NULL;
			}
		}
		return;
	}
	Super::Tick(DeltaTime);

	if (CurrentAction != NULL && !CurrentAction->Tick(DeltaTime) && CurrentAction->IsSafeToAbort() )
	{
		UE_VLOG(this, LogStrategyAI, Log, TEXT("Break on '%s' action after Update"), *CurrentAction->GetName()); 
		CurrentAction->Abort();
		CurrentAction = NULL;
	}

	// select best action to execute
	const bool bCanBreakCurrentAction = CurrentAction != NULL ? CurrentAction->IsSafeToAbort() : true;
	if (bCanBreakCurrentAction)
	{
		for (int32 Idx = 0; Idx < AllActions.Num(); Idx++)
		{
			if (CurrentAction == AllActions[Idx] && AllActions[Idx]->ShouldActivate())
			{
				break;
			}

			if (CurrentAction != AllActions[Idx] && AllActions[Idx]->ShouldActivate())
			{
				if (CurrentAction != NULL)
				{
					UE_VLOG(this, LogStrategyAI, Log, TEXT("Break on '%s' action, found better one '%s'"), *CurrentAction->GetName(), *AllActions[Idx]->GetName()); 
					CurrentAction->Abort();
				}

				CurrentAction = AllActions[Idx];
				if (CurrentAction != NULL)
				{
					UE_VLOG(this, LogStrategyAI, Log, TEXT("Execute on '%s' action"), *CurrentAction->GetName(), *AllActions[Idx]->GetName()); 
					CurrentAction->Activate();
					break;
				}
			}
		}
	}

	SelectTarget();
}

void AStrategyAIController::EnableLogic(bool bEnable)
{
	bLogicEnabled = bEnable;
}

bool AStrategyAIController::IsLogicEnabled() const		
{ 
	return bLogicEnabled; 
}

FVector AStrategyAIController::GetAdjustLocation()
{
	return GetPawn() ? GetPawn()->GetActorLocation() : (RootComponent ? RootComponent->GetComponentLocation() : FVector::ZeroVector);
}


#if ENABLE_VISUAL_LOG

void AStrategyAIController::GrabDebugSnapshot(FVisualLogEntry* Snapshot) const
{
	Super::GrabDebugSnapshot(Snapshot);

	FVisualLogStatusCategory MyCategory;
	MyCategory.Category = TEXT("StrategyAIController");
	MyCategory.Add(TEXT("CurrentAction"), CurrentAction != NULL ? *CurrentAction->GetName() : TEXT("NONE"));
	MyCategory.Add(TEXT("CurrentTarget"), *GetDebugName(CurrentTarget));

	AStrategyChar* MyChar = Cast<AStrategyChar>(GetPawn());
	if (MyChar)
	{
		MyCategory.Add(TEXT("AttackMax"), FString::FromInt(MyChar->GetPawnData()->AttackMax));
		MyCategory.Add(TEXT("AttackMin"), FString::FromInt(MyChar->GetPawnData()->AttackMin));
		MyCategory.Add(TEXT("MaxHealthBonus"), FString::FromInt(MyChar->GetPawnData()->MaxHealthBonus));
		MyCategory.Add(TEXT("Health"), FString::FromInt(MyChar->CombatComponent->Health));
		MyCategory.Add(TEXT("MaxHealth"), FString::FromInt(MyChar->CombatComponent->MaxHealth));
	}

	Snapshot->Status.Add(MyCategory);
}
#endif // ENABLE_VISUAL_LOG


void AStrategyAIController::RegisterMovementEventDelegate(FOnMovementEvent InDelegate)
{
	OnMoveCompletedDelegate = InDelegate;
}

void AStrategyAIController::RegisterBumpEventDelegate(FOnBumpEvent InDelegate)
{
	OnNotifyBumpDelegate = InDelegate;
}

void AStrategyAIController::UnregisterMovementEventDelegate()
{
	OnMoveCompletedDelegate.Unbind();
}

void AStrategyAIController::UnregisterBumpEventDelegate()
{
	OnNotifyBumpDelegate.Unbind();
}

