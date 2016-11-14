// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "TowerAIController.h"

DEFINE_LOG_CATEGORY(LogTowerAI);

ATowerAIController::ATowerAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SensingComponent = CreateDefaultSubobject<UTowerSensingComponent>(TEXT("SensingComp"));

	// add default action for most units
	AllowedActions.Add(UTowerAIAction_AttackTarget::StaticClass());
}

void ATowerAIController::Possess(APawn* inPawn)
{
	Super::Possess(inPawn);

	/** Create instances of our possible actions */
	AllActions.Reset();
	for (int32 Idx = 0; Idx < AllowedActions.Num(); Idx++)
	{
		UTowerAIAction* Action = NewObject<UTowerAIAction>(this, AllowedActions[Idx]);
		check(Action);
		Action->SetController(this);
		AllActions.Add(Action);
	}

	SetActorTickEnabled(true);
}

void ATowerAIController::UnPossess()
{
	if (CurrentTarget != NULL)
	{
		const APawn* TargetPawn = Cast<APawn>(CurrentTarget);
		if (TargetPawn != NULL)
		{
			ATowerAIController* const AITarget = Cast<ATowerAIController>(TargetPawn->Controller);
			if (AITarget != NULL)
			{
				AITarget->UnClaimAsTarget(this);
			}
		}
	}

	SetActorTickEnabled(false);
	Super::UnPossess();
}

uint8 ATowerAIController::GetTeamNum() const
{
	AMobaTower* const MyChar = Cast<AMobaTower>(GetPawn());
	return (MyChar != NULL) ? MyChar->GetTeamNum() : EStrategyTeam::Unknown;
}

bool ATowerAIController::IsActionAllowed(TSubclassOf<UTowerAIAction> inClass) const
{
	for (int32 Idx = 0; Idx < AllowedActions.Num(); Idx++)
	{
		if (AllowedActions[Idx] == inClass)
		{
			return true;
		}
	}

	return false;
}

UTowerAIAction* ATowerAIController::GetInstanceOfAction(TSubclassOf<UTowerAIAction> inClass) const
{
	int32 Idx = 0;
	for (Idx = 0; Idx < AllowedActions.Num(); Idx++)
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

bool ATowerAIController::IsTargetValid(AActor* InActor) const
{
	// try to find a character
	const AStrategyChar* TestChar = Cast<AStrategyChar>(InActor);
	const AMobaTestCharacter* TestPlayer = Cast<AMobaTestCharacter>(InActor);

	if ((TestChar && (TestChar->CombatComponent->Health > 0) && AStrategyGameMode::OnEnemyTeam(TestChar, this)) || (TestPlayer && TestPlayer->CombatCharacterComponent->Health > 0 && AStrategyGameMode::OnEnemyTeam(TestPlayer, this)))
	{
		return true;
	}

	return false;
}

void ATowerAIController::SelectTarget()
{
	if (GetPawn() == NULL)
	{
		return;
	}
	const FVector PawnLocation = GetPawn()->GetActorLocation();
	AActor* BestUnit = NULL;
	float BestUnitScore = 10000;

	for (int32 Idx = 0; Idx < SensingComponent->KnownTargets.Num(); Idx++)
	{
		AActor* const TestTarget = SensingComponent->KnownTargets[Idx].Get();
		if (TestTarget == NULL || !IsTargetValid(TestTarget))
		{
			continue;
		}

		/** don't care about targets with disabled logic */
		const APawn* TestPawn = Cast<APawn>(TestTarget);
		const ATowerAIController *AITarget = (TestPawn ? Cast<ATowerAIController>(TestPawn->Controller) : NULL);
		if (AITarget != NULL)
		{
			continue;
		}

		float TargetScore = (PawnLocation - TestTarget->GetActorLocation()).SizeSquared();
		if (CurrentTarget == TestTarget && TestTarget->IsA(AStrategyChar::StaticClass()))
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
	CurrentTarget = Cast<APawn>(BestUnit);
	if (CurrentTarget != NULL && OldTarget != CurrentTarget)
	{
		const APawn* OldTargetPawn = Cast<const APawn>(OldTarget);
		ATowerAIController* AITarget = OldTargetPawn != NULL ? Cast<ATowerAIController>(OldTargetPawn->Controller) : NULL;
		if (AITarget != NULL)
		{
			AITarget->UnClaimAsTarget(this);
		}

		APawn* const CurrentTargetPawn = Cast<APawn>(CurrentTarget);
		AITarget = (CurrentTargetPawn ? Cast<ATowerAIController>(CurrentTargetPawn->Controller) : NULL);
		if (AITarget != NULL)
		{
			AITarget->ClaimAsTarget(this);
		}
	}

	UE_VLOG(this, LogTowerAI, Log, TEXT("Selected target: %s"), CurrentTarget != NULL ? *CurrentTarget->GetName() : TEXT("NONE"));
}

void ATowerAIController::ClaimAsTarget(TWeakObjectPtr<ATowerAIController> InController)
{
	ClaimedBy.AddUnique(InController);
}

void ATowerAIController::UnClaimAsTarget(TWeakObjectPtr<ATowerAIController> InController)
{
	for (int32 Idx = ClaimedBy.Num() - 1; Idx >= 0; Idx--)
	{
		const ATowerAIController* TestC = Cast<ATowerAIController>(ClaimedBy[Idx].Get());
		if (TestC != NULL && TestC == InController.Get())
		{
			ClaimedBy.RemoveAt(Idx);
		}
	}
}

bool ATowerAIController::IsClaimedBy(TWeakObjectPtr<ATowerAIController> InController) const
{
	for (int32 Idx = ClaimedBy.Num() - 1; Idx >= 0; Idx--)
	{
		const ATowerAIController* TestC = Cast<ATowerAIController>(ClaimedBy[Idx].Get());
		if (TestC != NULL && TestC == InController.Get())
		{
			return true;
		}
	}

	return false;
}

int32 ATowerAIController::GetNumberOfAttackers() const
{
	return ClaimedBy.Num();
}

void ATowerAIController::Tick(float DeltaTime)
{
	const AMobaTower* MyChar = Cast<AMobaTower>(GetPawn());
	if (MyChar == NULL || (MyChar != NULL && MyChar->CombatComponent->Health <= 0))
	{
		
		return;
	}
	Super::Tick(DeltaTime);

	if (CurrentAction != NULL && !CurrentAction->Tick(DeltaTime) && CurrentAction->IsSafeToAbort())
	{
		UE_VLOG(this, LogTowerAI, Log, TEXT("Break on '%s' action after Update"), *CurrentAction->GetName());
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
					UE_VLOG(this, LogTowerAI, Log, TEXT("Break on '%s' action, found better one '%s'"), *CurrentAction->GetName(), *AllActions[Idx]->GetName());
					CurrentAction->Abort();
				}

				CurrentAction = AllActions[Idx];
				if (CurrentAction != NULL)
				{
					UE_VLOG(this, LogTowerAI, Log, TEXT("Execute on '%s' action"), *CurrentAction->GetName(), *AllActions[Idx]->GetName());
					CurrentAction->Activate();
					break;
				}
			}
		}
	}

	SelectTarget();
}


