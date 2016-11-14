// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyTypes.h"
#include "StrategyInputInterface.h"
#include "StrategyTeamInterface.h"
#include "StrategySelectionInterface.h"
#include "StrategyBuilding.generated.h"


DECLARE_DELEGATE_OneParam(FBuildFinishedDelegate, class AStrategyBuilding*);

class AStrategyChar;

UCLASS(Abstract, Blueprintable)
class AStrategyBuilding : public APawn, public IStrategyTeamInterface
{
	GENERATED_UCLASS_BODY()

	/** team */
	UPROPERTY(EditInstanceOnly, Category=Building)
	TEnumAsByte<EStrategyTeam::Type> SpawnTeamNum;

	// Begin Actor interface
	virtual void PostInitializeComponents() override;
	virtual void Destroyed() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PostLoad() override;
	// End Actor Interface

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Begin StrategyTeamInterface interface

	/** [IStrategyTeamInterface] get team number */
	virtual uint8 GetTeamNum() const override;

	// End StrategyTeamInterface interface

	//////////////////////////////////////////////////////////////////////////
	// Reading data


	/** get building's name */
	FString GetBuildingName() const;

protected:
	/** name of building */
	UPROPERTY(EditDefaultsOnly, Category=Building)
	FString BuildingName;

private:
	/** trigger box component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Touch, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* TriggerBox;
protected:

	/** get data for current team */
	struct FPlayerData* GetTeamData() const;

protected:
	/** Returns TriggerBox subobject **/
	FORCEINLINE UBoxComponent* GetTriggerBox() const { return TriggerBox; }
};


