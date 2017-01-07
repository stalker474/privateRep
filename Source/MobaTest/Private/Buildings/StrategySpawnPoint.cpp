// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "StrategySpawnPoint.h"

AStrategySpawnPoint::AStrategySpawnPoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Player = nullptr;
	IsSpawnable = true;
}

void AStrategySpawnPoint::MULTICAST_PlayerChanged_Implementation(const FString& Name)
{
	PlayerChanged(Name);
}

void AStrategySpawnPoint::PlayerChanged_Implementation(const FString& Name) {

}

uint8 AStrategySpawnPoint::GetTeamNum() const
{
	return SpawnTeamNum;
}
