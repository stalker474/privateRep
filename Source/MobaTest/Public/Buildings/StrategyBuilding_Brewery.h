// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyBuilding.h"
#include "StrategyBuilding_Brewery.generated.h"

class AStrategyChar;
class UStrategyAIDirector;
class AStrategyBuilding;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaveSpawnedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameFinishedDelegate);


UCLASS(Abstract, Blueprintable)
class AStrategyBuilding_Brewery : public AStrategyBuilding
{
	GENERATED_UCLASS_BODY()

private:
	/** team's AI director */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Brewery, meta = (AllowPrivateAccess = "true"))
	UStrategyAIDirector* AIDirector;
public:

	/** The class of minion to spawn. */
	UPROPERTY(EditDefaultsOnly, Category=Brewery)
	TSubclassOf<AStrategyChar> MinionCharClass;

	/** delegate to broadcast about finished wave */
	UPROPERTY(BlueprintAssignable)
	FWaveSpawnedDelegate OnWaveSpawned;

	UPROPERTY(EditAnywhere, Category = Lane)
	ELaneEnum LanePosition;

	// Begin Actor interface
	/** initial setup */
	virtual void PostInitializeComponents() override;
	// End Actor interface


	// Begin StrategyBuilding interface

	// End StrategyBuilding interface

	/** spawns a dwarf */
	bool SpawnDwarf();

	/** gets spawn queue length string */
	FText GetSpawnQueueLength() const;

	/** notify about new game state */
	void OnGameplayStateChange(EGameplayState::Type NewState);

public:
	/** Returns AIDirector subobject **/
	FORCEINLINE UStrategyAIDirector* GetAIDirector() const { return AIDirector; }
};
