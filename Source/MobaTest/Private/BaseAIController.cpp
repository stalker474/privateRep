// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "BaseAIController.h"
#include "MobaAICharacter.h"
#include "StrategyAIAction.h"
#include "StrategyAISensingComponent.h"
#include "MobaTestCharacter.h"

DEFINE_LOG_CATEGORY(LogMobaAI);

/*
* Main AI Controller class
*/
ABaseAIController::ABaseAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bLogicEnabled(true)
{
	SensingComponent = CreateDefaultSubobject<UStrategyAISensingComponent>(TEXT("SensingComp"));
}

void ABaseAIController::Possess(APawn* inPawn)
{
	Super::Possess(inPawn);

	/** Create instances of our possible actions */
	AllActions.Reset();
	for (int32 Idx = 0; Idx < AllowedActions.Num(); Idx++)
	{
		UStrategyAIAction* Action = NewObject<UStrategyAIAction>(this, AllowedActions[Idx]);
		check(Action);
		Action->SetController(this);
		AllActions.Add(Action);
	}

	/*AStrategyChar* const MyChar = Cast<AStrategyChar>(GetPawn());
	if (MyChar != NULL && MyChar->GetCharacterMovement() != NULL)
	{
		MyChar->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}*/

	SetActorTickEnabled(true);
	EnableLogic(true);
}

void ABaseAIController::UnPossess()
{
	if (CurrentTarget != NULL)
	{
		const APawn* TargetPawn = Cast<APawn>(CurrentTarget);
		if (TargetPawn != NULL)
		{
			ABaseAIController* const AITarget = Cast<ABaseAIController>(TargetPawn->Controller);
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

void ABaseAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (CurrentAction != NULL && Result.Code != EPathFollowingResult::Aborted)
	{
		OnMoveCompletedDelegate.ExecuteIfBound();
	}
}

/** Pawn has hit something */
void ABaseAIController::NotifyBump(FHitResult const& Hit)
{
	if (CurrentAction != NULL && OnNotifyBumpDelegate.IsBound())
	{
		OnNotifyBumpDelegate.Execute(Hit);
	}
}

bool ABaseAIController::IsActionAllowed(TSubclassOf<UStrategyAIAction> inClass) const
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

UStrategyAIAction* ABaseAIController::GetInstanceOfAction(TSubclassOf<UStrategyAIAction> inClass) const
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

void ABaseAIController::SelectTarget()
{
	if (GetPawn() == NULL)
	{
		return;
	}

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
		const ABaseAIController *AITarget = (TestPawn ? Cast<ABaseAIController>(TestPawn->Controller) : NULL);
		if (AITarget != NULL && !AITarget->IsLogicEnabled())
		{
			continue;
		}

		float TargetScore = ComputeScore(TestPawn);

		if (BestUnit == NULL || BestUnitScore > TargetScore)
		{
			BestUnitScore = TargetScore;
			BestUnit = TestTarget;
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
		ABaseAIController* AITarget = OldTargetPawn != NULL ? Cast<ABaseAIController>(OldTargetPawn->Controller) : NULL;
		if (AITarget != NULL)
		{
			AITarget->UnClaimAsTarget(this);
		}

		APawn* const CurrentTargetPawn = Cast<APawn>(CurrentTarget);
		AITarget = (CurrentTargetPawn ? Cast<ABaseAIController>(CurrentTargetPawn->Controller) : NULL);
		if (AITarget != NULL)
		{
			AITarget->ClaimAsTarget(this);
		}
	}

	UE_VLOG(this, LogMobaAI, Log, TEXT("Selected target: %s"), CurrentTarget != NULL ? *CurrentTarget->GetName() : TEXT("NONE"));
}

void ABaseAIController::ClaimAsTarget(TWeakObjectPtr<AController> InController)
{
	ClaimedBy.AddUnique(InController);
}

void ABaseAIController::UnClaimAsTarget(TWeakObjectPtr<AController> InController)
{
	for (int32 Idx = ClaimedBy.Num() - 1; Idx >= 0; Idx--)
	{
		const ABaseAIController* TestC = Cast<ABaseAIController>(ClaimedBy[Idx].Get());
		if (TestC != NULL && TestC == InController.Get())
		{
			ClaimedBy.RemoveAt(Idx);
		}
	}
}

bool ABaseAIController::IsClaimedBy(TWeakObjectPtr<AController> InController) const
{
	for (int32 Idx = ClaimedBy.Num() - 1; Idx >= 0; Idx--)
	{
		const ABaseAIController* TestC = Cast<ABaseAIController>(ClaimedBy[Idx].Get());
		if (TestC != NULL && TestC == InController.Get())
		{
			return true;
		}
	}

	return false;
}

int32 ABaseAIController::GetNumberOfAttackers() const
{
	return ClaimedBy.Num();
}

void ABaseAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	const AStrategyChar* pHolop = Cast<AStrategyChar>(GetPawn());
	const AMobaAICharacter* pMobaAI = Cast<AMobaAICharacter>(GetPawn());
	if (!IsLogicEnabled() || (GetPawn() == NULL) || (pHolop != NULL && pHolop->CombatComponent->Health <= 0) || (pMobaAI != NULL && pMobaAI->CombatComponent->Health <= 0))
	{
		if (!IsLogicEnabled())
		{
			if (CurrentAction != NULL)
			{
				CurrentAction->Abort();
				CurrentAction = NULL;
			}
		}
		return;
	}

	if (CurrentAction != NULL && !CurrentAction->Tick(DeltaTime) && CurrentAction->IsSafeToAbort())
	{
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
					CurrentAction->Abort();
				}

				CurrentAction = AllActions[Idx];
				if (CurrentAction != NULL)
				{
					CurrentAction->Activate();
					break;
				}
			}
		}
	}

	SelectTarget();
}

void ABaseAIController::EnableLogic(bool bEnable)
{
	bLogicEnabled = bEnable;
}

bool ABaseAIController::IsLogicEnabled() const
{
	return bLogicEnabled;
}

FVector ABaseAIController::GetAdjustLocation()
{
	return GetPawn() ? GetPawn()->GetActorLocation() : (RootComponent ? RootComponent->GetComponentLocation() : FVector::ZeroVector);
}

void ABaseAIController::RegisterMovementEventDelegate(FOnMovementEvent InDelegate)
{
	OnMoveCompletedDelegate = InDelegate;
}

void ABaseAIController::RegisterBumpEventDelegate(FOnBumpEvent InDelegate)
{
	OnNotifyBumpDelegate = InDelegate;
}

void ABaseAIController::UnregisterMovementEventDelegate()
{
	OnMoveCompletedDelegate.Unbind();
}

void ABaseAIController::UnregisterBumpEventDelegate()
{
	OnNotifyBumpDelegate.Unbind();
}

