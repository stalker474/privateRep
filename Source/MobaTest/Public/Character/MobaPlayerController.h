// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "MobaPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MOBATEST_API AMobaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void OnKilled();

	UFUNCTION()
	void Respawn();

private:
	FTimerHandle TimerHandle_Respawn;
};
