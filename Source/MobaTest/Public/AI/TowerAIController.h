// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "TowerAIAction.h"
#include "TowerSensingComponent.h"
#include "TowerAIController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTowerAI, Display, All);

/**
 * 
 */
UCLASS()
class MOBATEST_API ATowerAIController : public AAIController, public IStrategyTeamInterface
{
	GENERATED_UCLASS_BODY()

private:
	/** Component used to detect and respond to sight and sound events. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
		class UTowerSensingComponent* SensingComponent;
	
public:

	/** List of all possible actions for this logic */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AIController)
		TArray<TSubclassOf<UTowerAIAction> > AllowedActions;
	/** List for all actions for this logic instance */
	UPROPERTY()
		TArray<UTowerAIAction*> AllActions;

	/** Current, selected action to execute */
	UPROPERTY()
		UTowerAIAction* CurrentAction;

	/** List of all possible targets for us */
	UPROPERTY()
		TArray<APawn*>	AllTargets;

	/** Current selected target to attack */
	UPROPERTY()
		APawn*			CurrentTarget;

	// Begin AActor Interface
	virtual void Tick(float DeltaTime) override;
	// End AActor Interface

	// Begin Controller Interface
	virtual void Possess(APawn* inPawn) override;
	virtual void UnPossess() override;
	// End Controller Interface

	// Begin StrategyTeamInterface Interface
	virtual uint8 GetTeamNum() const override;
	// End StrategyTeamInterface Interface

	/** Checks if we are allowed to use some action */
	bool IsActionAllowed(TSubclassOf<UTowerAIAction> inClass) const;

	/** Return instance of action we are allowed to use */
	UTowerAIAction* GetInstanceOfAction(TSubclassOf<UTowerAIAction> inClass) const;

	/** Checks actor and returns true if valid */
	bool IsTargetValid(AActor* InActor) const;

	/** Claim controller as target */
	void ClaimAsTarget(TWeakObjectPtr<ATowerAIController> InController);

	/** UnClaim controller as target */
	void UnClaimAsTarget(TWeakObjectPtr<ATowerAIController> InController);

	/** Check if desired controller claimed this one */
	bool IsClaimedBy(TWeakObjectPtr<ATowerAIController> InController) const;

	/** get number of enemies who claimed this one as target */
	int32 GetNumberOfAttackers() const;

	/** Returns SensingComponent subobject **/
	FORCEINLINE UTowerSensingComponent* GetSensingComponent() const { return SensingComponent; }

protected:
	/** Check targets list and select one as current target */
	virtual void SelectTarget();

	/** array of controllers claimed this one as target */
	TArray< TWeakObjectPtr<ATowerAIController> > ClaimedBy;
};
