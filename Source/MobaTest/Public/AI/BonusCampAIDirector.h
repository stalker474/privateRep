// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MobaAIController.h"
#include "Components/ActorComponent.h"
#include "BonusCampAIDirector.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MOBATEST_API UBonusCampAIDirector : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:	

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	/** notify about new game state */
	void OnGameplayStateChange(EGameplayState::Type NewState);

	UPROPERTY(EditAnywhere)
	TWeakObjectPtr<AMobaAIController> MonsterAIController;

protected:
	/** check conditions and spawn if possible */
	void Spawn();

	/** next time to spawn */
	UPROPERTY()
	float NextSpawnTime;
	
	UPROPERTY()
	bool MonstersAlive;
};
