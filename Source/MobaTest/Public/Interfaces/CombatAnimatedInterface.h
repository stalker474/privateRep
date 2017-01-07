// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CombatAnimatedInterface.generated.h"

UINTERFACE()
class UCombatAnimatedInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class ICombatAnimatedInterface
{
	GENERATED_IINTERFACE_BODY()

	virtual void StartAttack() = 0;
};
