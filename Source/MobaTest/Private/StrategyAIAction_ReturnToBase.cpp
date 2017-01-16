// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "StrategyAIAction_ReturnToBase.h"
#include "AI/Navigation/NavigationPathGenerator.h"
#include "MobaAIController.h"

UStrategyAIAction_ReturnToBase::UStrategyAIAction_ReturnToBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Destination(FVector::ZeroVector)
	, bIsMoving(false)
	, NotMovingFromTime(0)
{
}

bool UStrategyAIAction_ReturnToBase::IsSafeToAbort() const
{
	return true;
}

void UStrategyAIAction_ReturnToBase::Abort()
{
	check(MyAIController.IsValid());
	Super::Abort();

	bIsMoving = false;
	Destination = FVector::ZeroVector;
	if (MyAIController->GetPathFollowingComponent())
	{
		MyAIController->GetPathFollowingComponent()->AbortMove(*MyAIController->GetOwner(), FPathFollowingResultFlags::UserAbort);
	}
	MyAIController->UnregisterMovementEventDelegate();
}

void UStrategyAIAction_ReturnToBase::Activate()
{
	check(MyAIController.IsValid());
	Super::Activate();

	NotMovingFromTime = 0;
	// find brewery base and cache it's destination
	AMobaAIController * pController = Cast<AMobaAIController>(MyAIController.Get());

	if (pController == nullptr)
	{
		UE_VLOG(MyAIController.Get(), LogMobaAI, Log, TEXT("ERROR, ReturnToBase can only be used on AMobaAIController!"));
		return;
	}

	const AActor* Actor = pController->GetPawn();
	if (Actor != NULL)
	{
		bIsMoving = true;
		Destination = Actor->GetOwner()->GetActorLocation();
		MyAIController->MoveToLocation(Destination, 10.0f, true, true, true);
	}

	FOnMovementEvent MovementDelegate;
	MovementDelegate.BindUObject(this, &UStrategyAIAction_ReturnToBase::OnMoveCompleted);
	MyAIController->RegisterMovementEventDelegate(MovementDelegate);
}

bool UStrategyAIAction_ReturnToBase::Tick(float DeltaTime)
{
	if (bIsMoving && MyAIController.IsValid())
	{
		const bool bNoMove = (MyAIController->GetMoveStatus() != EPathFollowingStatus::Moving);
		if (!bNoMove)
		{
			NotMovingFromTime = 0;
		}
		else if (NotMovingFromTime == 0)
		{
			NotMovingFromTime = MyAIController->GetWorld()->TimeSeconds;
		}

		if (bNoMove && (MyAIController->GetWorld()->TimeSeconds - NotMovingFromTime) > 2)
		{
			Abort();
		}
	}
	return bIsExecuted && Destination != FVector::ZeroVector && bIsMoving;
}

bool UStrategyAIAction_ReturnToBase::ShouldActivate() const
{
	check(MyAIController.IsValid());

	FVector DesiredDestination = FVector::ZeroVector;

	AMobaAIController * pController = Cast<AMobaAIController>(MyAIController.Get());

	if (pController == nullptr)
	{
		UE_VLOG(MyAIController.Get(), LogMobaAI, Log, TEXT("ERROR, ReturnToBase can only be used on AMobaAIController!"));
		return false;
	}

	const AActor* Actor = pController->GetPawn();
	if (Actor != NULL)
	{
		DesiredDestination = Actor->GetOwner()->GetActorLocation();
	}

	if (DesiredDestination != FVector::ZeroVector)
	{
		return true;
	}
	return false;
}

void UStrategyAIAction_ReturnToBase::OnMoveCompleted()
{
	bIsMoving = false;
}

void UStrategyAIAction_ReturnToBase::OnPathUpdated(INavigationPathGenerator* PathGenerator, EPathUpdate::Type inType)
{
	check(MyAIController.IsValid());

	if (inType != EPathUpdate::Update)
	{
		UE_VLOG(MyAIController.Get(), LogMobaAI, Log, TEXT("WARRNING, OnPathUpdated with error - PathUpdateTyp %d"), int32(inType));
		Abort();
	}
}


