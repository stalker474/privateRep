// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BonusCampAIDirector.h"
#include "MobaAICharacter.h"
#include "GameFramework/Pawn.h"
#include "BonusCamp.generated.h"

UCLASS()
class MOBATEST_API ABonusCamp : public APawn
{
	GENERATED_UCLASS_BODY()
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BonusCamp, meta = (AllowPrivateAccess = "true"))
	UBonusCampAIDirector* AIDirector;

public:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	/** notify about new game state */
	void OnGameplayStateChange(EGameplayState::Type NewState);

	/** The class of minion to spawn. */
	UPROPERTY(EditDefaultsOnly, Category = BonusCamp)
	TSubclassOf<AMobaAICharacter> MonsterCharClass;
};
