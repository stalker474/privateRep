// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MobaTest.h"
#include "StrategyAIAction_AttackTarget.h"
#include "BaseAIController.h"
#include "StrategyChar.h"
#include "MobaAICharacter.h"
#include "VisualLogger/VisualLogger.h"

UStrategyAIAction_AttackTarget::UStrategyAIAction_AttackTarget(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
, MeleeAttackAnimationEndTime(0)
, bIsPlayingAnimation(false)
, bMovingToTarget(false)
, bCanAttack(false)
{
	// Non-property initialization
}

/** function to register as Update delegate for this action */
bool UStrategyAIAction_AttackTarget::Tick(float DeltaTime)
{
	check(MyAIController.IsValid());
	Super::Tick(DeltaTime);

	UpdateTargetInformation();

	bIsPlayingAnimation = MyAIController->GetWorld()->GetTimeSeconds() < MeleeAttackAnimationEndTime;
	if (!bIsPlayingAnimation)
	{
		if (!TargetActor.IsValid())
		{
			return false;
		}

		// try move closer if needed again
		MoveCloser();
		
		if (!bMovingToTarget && bCanAttack)
		{
			AMobaAICharacter * const MyMobaAIChar = Cast<AMobaAICharacter>(MyAIController->GetPawn());
			if (MyMobaAIChar != NULL)
			{
				MeleeAttackAnimationEndTime = MyAIController->GetWorld()->GetTimeSeconds() + MyMobaAIChar->PlayMeleeAnim();
				bIsPlayingAnimation = true;
			}
		}

		return MyAIController->IsTargetValid(TargetActor.Get());
	}

	return true;
}

void UStrategyAIAction_AttackTarget::UpdateTargetInformation()
{
	AActor* const OldTargetActor = TargetActor.Get();
	if (MyAIController->CurrentTarget != TargetActor.Get())
	{
		TargetActor = MyAIController->CurrentTarget;
	}

	if (OldTargetActor != TargetActor.Get() && bMovingToTarget)
	{
		bMovingToTarget = false;
	}

	if (MyAIController->IsTargetValid(TargetActor.Get()) )
	{
		MyAIController->SetFocus(TargetActor.Get());
	}
}

void UStrategyAIAction_AttackTarget::MoveCloser()
{
	check(MyAIController.IsValid());

	if (bIsPlayingAnimation || bMovingToTarget || TargetActor == nullptr)
	{
		return;
	}
		
	TargetDestination = TargetActor->GetActorLocation();
	AMobaAICharacter * const MyMobaAIChar = Cast<AMobaAICharacter>(MyAIController->GetPawn());
	if( MyMobaAIChar == nullptr)
	{
		UE_LOG(LogMobaAI, Warning, TEXT("Invalid Char/Pawn in Move Closer")); 
		return;
	}
	float AttackDistance;

	AttackDistance = MyMobaAIChar->GetPawnData()->AttackDistance;

	const float Dist = (TargetDestination - MyAIController->GetAdjustLocation()).Size2D();

	if (Dist > AttackDistance)
	{
		UE_VLOG(MyAIController.Get(), LogMobaAI, Log, TEXT("Let's move closer"));
		bMovingToTarget = true;
		MyAIController->MoveToActor(TargetActor.Get(), 0.9 * AttackDistance);
	}
	else
		bCanAttack = Dist <= AttackDistance;
}

void UStrategyAIAction_AttackTarget::OnMoveCompleted()
{
	bMovingToTarget = false;
	if (MyAIController->GetPathFollowingComponent())
	{
		MyAIController->GetPathFollowingComponent()->AbortMove(*MyAIController->GetOwner(), FPathFollowingResultFlags::AlreadyAtGoal);
	}
	float AttackDistance;
	AMobaAICharacter * const MyMobaAIChar = Cast<AMobaAICharacter>(MyAIController->GetPawn());

	AttackDistance = MyMobaAIChar->GetPawnData()->AttackDistance;

	const float Dist = (TargetDestination - MyAIController->GetAdjustLocation()).Size2D();

	bCanAttack = Dist <= AttackDistance;
}

void UStrategyAIAction_AttackTarget::NotifyBump(FHitResult const& Hit)
{
	check(MyAIController.IsValid());

	// if we hit our target, just stop movement
	AMobaAICharacter* const HitChar = Cast<AMobaAICharacter>(Hit.Actor.Get());
	if (HitChar != NULL && AStrategyGameMode::OnEnemyTeam(HitChar, MyAIController->GetPawn()) && bMovingToTarget)
	{
		bMovingToTarget = false;
		if (MyAIController->GetPathFollowingComponent())
		{
			MyAIController->GetPathFollowingComponent()->AbortMove(*MyAIController->GetOwner(), FPathFollowingResultFlags::Blocked);
		}
	}
}

void UStrategyAIAction_AttackTarget::Activate()
{
	check(MyAIController.IsValid());
	Super::Activate();

	bIsPlayingAnimation = false;
	MeleeAttackAnimationEndTime = 0;
	TargetActor = MyAIController->CurrentTarget;

	FOnBumpEvent BumpDelegate;
	BumpDelegate.BindUObject(this, &UStrategyAIAction_AttackTarget::NotifyBump);
	MyAIController->RegisterBumpEventDelegate(BumpDelegate);

	FOnMovementEvent MovementDelegate;
	MovementDelegate.BindUObject(this, &UStrategyAIAction_AttackTarget::OnMoveCompleted);
	MyAIController->RegisterMovementEventDelegate(MovementDelegate);
}


bool UStrategyAIAction_AttackTarget::IsSafeToAbort() const
{
	return !bIsPlayingAnimation;
}

void UStrategyAIAction_AttackTarget::Abort()
{
	check(MyAIController.IsValid());
	Super::Abort();

	if (bMovingToTarget && MyAIController->GetPathFollowingComponent())
	{
		MyAIController->GetPathFollowingComponent()->AbortMove(*MyAIController->GetOwner(), FPathFollowingResultFlags::UserAbort);
	}
	bMovingToTarget = false;
	MyAIController->ClearFocus(EAIFocusPriority::Gameplay);
	MyAIController->UnregisterBumpEventDelegate();
	MyAIController->UnregisterMovementEventDelegate();
}

bool UStrategyAIAction_AttackTarget::ShouldActivate() const
{
	check(MyAIController.IsValid());
	return MyAIController->CurrentTarget != NULL && MyAIController->IsTargetValid(MyAIController->CurrentTarget);
}
