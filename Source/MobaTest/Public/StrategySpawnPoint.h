// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerStart.h"
#include "StrategySelectionInterface.h"
#include "StrategySpawnPoint.generated.h"

/**
 * 
 */
UCLASS()
class MOBATEST_API AStrategySpawnPoint : public APlayerStart, public IStrategyTeamInterface
{
	GENERATED_UCLASS_BODY()
public:
	
		/** team */
	UPROPERTY(EditInstanceOnly, Category = Team)
	TEnumAsByte<EStrategyTeam::Type> SpawnTeamNum;
	
	// Begin StrategyTeamInterface interface

	/** [IStrategyTeamInterface] get team number */
	UFUNCTION(BlueprintCallable, Category="Team")
	uint8 GetTeamNum() const override;
	// End StrategyTeamInterface interface

	UPROPERTY()
	APlayerController* Player;

	UFUNCTION(NetMulticast,Reliable)
	void MULTICAST_PlayerChanged(const FString& Name);

	UFUNCTION(BlueprintNativeEvent)
	void PlayerChanged(const FString& Name);

	UPROPERTY(EditInstanceOnly, Category = DeveloperOnly)
	bool IsSpawnable;
};
