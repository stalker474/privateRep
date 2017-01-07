// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TowerAIAction.h"
#include "TowerAIAction_AttackTarget.generated.h"

/**
 * 
 */
UCLASS()
class MOBATEST_API UTowerAIAction_AttackTarget : public UTowerAIAction
{
	GENERATED_UCLASS_BODY()

public:

	// Begin StrategyAIAction interface

	/** Update in time current action */
	virtual bool Tick(float DeltaTime) override;

	/** activate action */
	virtual void Activate() override;

	/** abort action to start something else */
	virtual void Abort() override;

	/** can we abort this action? */
	virtual bool IsSafeToAbort() const override;

	/** Should we activate action this time ? */
	virtual bool ShouldActivate() const override;

	// End StrategyAIAction interface

protected:
	/** updates any information about target, his location, target changes in ai controller, etc. */
	void UpdateTargetInformation();

	/** target actor to attack */
	TWeakObjectPtr<APawn> TargetActor;

	/** time when we will finish playing melee animation */
	float MeleeAttackAnimationEndTime;

	/** if pawn is playing attack animation */
	uint32 bIsPlayingAnimation : 1;
};
