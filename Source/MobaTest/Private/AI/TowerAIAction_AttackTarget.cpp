// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "TowerAIAction_AttackTarget.h"


UTowerAIAction_AttackTarget::UTowerAIAction_AttackTarget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MeleeAttackAnimationEndTime(0)
	, bIsPlayingAnimation(false)
{
	// Non-property initialization
}

/** function to register as Update delegate for this action */
bool UTowerAIAction_AttackTarget::Tick(float DeltaTime)
{
	check(MyAIController.IsValid());
	Super::Tick(DeltaTime);

	UpdateTargetInformation();

	bIsPlayingAnimation = MyAIController->GetWorld()->GetTimeSeconds() < MeleeAttackAnimationEndTime;
	if (!bIsPlayingAnimation)
	{
		if (!TargetActor.IsValid())
		{
			AMobaTower* const MyChar = Cast<AMobaTower>(MyAIController->GetPawn());
			if (MyChar != NULL)
			{
				MyChar->Abort();
				bIsPlayingAnimation = false;
			}
			return false;
		}

		AMobaTower* const MyChar = Cast<AMobaTower>(MyAIController->GetPawn());
		if (MyChar != NULL)
		{
			MeleeAttackAnimationEndTime = MyAIController->GetWorld()->GetTimeSeconds() + MyChar->Attack(TargetActor);
			bIsPlayingAnimation = true;
		}

		return MyAIController->IsTargetValid(TargetActor.Get());
	}

	return true;
}

void UTowerAIAction_AttackTarget::UpdateTargetInformation()
{
	AActor* const OldTargetActor = TargetActor.Get();
	if (MyAIController->CurrentTarget != TargetActor.Get())
	{
		TargetActor = MyAIController->CurrentTarget;
	}

	if (MyAIController->IsTargetValid(TargetActor.Get()))
	{
		MyAIController->SetFocus(TargetActor.Get());
	}
}

void UTowerAIAction_AttackTarget::Activate()
{
	check(MyAIController.IsValid());
	Super::Activate();

	bIsPlayingAnimation = false;
	MeleeAttackAnimationEndTime = 0;
	TargetActor = MyAIController->CurrentTarget;
}


bool UTowerAIAction_AttackTarget::IsSafeToAbort() const
{
	return !bIsPlayingAnimation;
}

void UTowerAIAction_AttackTarget::Abort()
{
	check(MyAIController.IsValid());
	Super::Abort();

	MyAIController->ClearFocus(EAIFocusPriority::Gameplay);
}

bool UTowerAIAction_AttackTarget::ShouldActivate() const
{
	check(MyAIController.IsValid());
	return MyAIController->CurrentTarget != NULL && MyAIController->IsTargetValid(MyAIController->CurrentTarget);
}

