// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "TowerAIAction.generated.h"

class ATowerAIController;
/**
 * 
 */
UCLASS(abstract, BlueprintType)
class MOBATEST_API UTowerAIAction : public UObject
{
	GENERATED_UCLASS_BODY()

	/**
	* Update this action.
	* @return false to finish this action, true to continue
	*/
	virtual bool Tick(float DeltaTime);

	/** Activate action. */
	virtual void Activate();

	/** Should we activate action this time ? */
	virtual bool ShouldActivate() const;

	/** Abort action to start something else. */
	virtual void Abort();

	/** Can we abort this action? */
	virtual bool IsSafeToAbort() const;

	/** Set owning AI controller. */
	void SetController(ATowerAIController* InOwner);

protected:
	/** Weak pointer to AI controller, to have faster access (cached information). */
	TWeakObjectPtr<ATowerAIController>	MyAIController;

	/**	Tells us if we are already executed. */
	bool bIsExecuted;
};
