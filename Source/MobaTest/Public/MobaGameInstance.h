// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "MobaTestCharacter.h"
#include "MobaGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MOBATEST_API UMobaGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	
public:
	UPROPERTY()
	TSubclassOf<AMobaTestCharacter> ChosenCharacter;
	
};
