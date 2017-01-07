// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MobaTest.h"
#include "StrategyGameBlueprintLibrary.h"
#include "StrategyAttachment.h"

UStrategyGameBlueprintLibrary::UStrategyGameBlueprintLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

AStrategyGameMode* GetGameFromContextObject(UObject* WorldContextObject)
{
	UWorld* const MyWorld = GEngine->GetWorldFromContextObject(WorldContextObject);
	check(MyWorld);

	AStrategyGameMode* const MyGame = MyWorld->GetAuthGameMode<AStrategyGameMode>();
	return MyGame;
}

AStrategyGameState* GetGameStateFromContextObject(UObject* WorldContextObject)
{
	UWorld* const MyWorld = GEngine->GetWorldFromContextObject(WorldContextObject);
	check(MyWorld);

	AStrategyGameState* const MyGameState = MyWorld->GetGameState<AStrategyGameState>();
	return MyGameState;
}

bool UStrategyGameBlueprintLibrary::AreFriends(AActor* Actor1, AActor* Actor2)
{
	return AStrategyGameMode::OnFriendlyTeam(Actor1, Actor2);
}

bool UStrategyGameBlueprintLibrary::AreEnemies(AActor* Actor1, AActor* Actor2)
{
	return AStrategyGameMode::OnEnemyTeam(Actor1, Actor2);
}

void UStrategyGameBlueprintLibrary::GiveWeaponFromClass(AStrategyChar* InChar, TSubclassOf<UStrategyAttachment> ArmorClass)
{
	if (InChar && *ArmorClass)
	{
		auto MyWeapon = NewObject<UStrategyAttachment>(InChar, *ArmorClass);
		InChar->SetWeaponAttachment(MyWeapon);
	}
}

void UStrategyGameBlueprintLibrary::GiveWeapon(AStrategyChar* InChar, UBlueprint* WeaponBlueprint)
{
	GiveWeaponFromClass(InChar, WeaponBlueprint ? *WeaponBlueprint->GeneratedClass : nullptr);
}

void UStrategyGameBlueprintLibrary::GiveArmorFromClass(AStrategyChar* InChar, TSubclassOf<UStrategyAttachment> ArmorClass)
{
	if (InChar && *ArmorClass)
	{
		auto MyArmor = NewObject<UStrategyAttachment>(InChar, *ArmorClass);
		InChar->SetArmorAttachment(MyArmor);
	}
}

void UStrategyGameBlueprintLibrary::GiveArmor(AStrategyChar* InChar, UBlueprint* ArmorBlueprint)
{
	GiveArmorFromClass(InChar, ArmorBlueprint ? *ArmorBlueprint->GeneratedClass : nullptr);
}

void UStrategyGameBlueprintLibrary::ToggleMinionVisibility(AStrategyChar* InMinionChar)
{
	if (InMinionChar)
	{
		InMinionChar->SetActorEnableCollision(!InMinionChar->GetActorEnableCollision());
		InMinionChar->SetActorHiddenInGame(!InMinionChar->bHidden);
	}
}

bool UStrategyGameBlueprintLibrary::RemoveMinion(AStrategyChar* InMinionChar)
{
	if (InMinionChar)
	{
		return InMinionChar->Destroy();
	} 
	else
	{
		return false;
	}
}

void UStrategyGameBlueprintLibrary::ShowTitle(UObject* WorldContextObject, FString LocKey)
{
	//UWorld* const MyWorld = GEngine->GetWorldFromContextObject(WorldContextObject);
	//AStrategyGameMode* const MyGameMode = MyWorld->GetAuthGameMode<AStrategyGameMode>();

	//SAssignNew(MyGameMode->StrategyTitle,SStrategyTitle)
	//.Visibility(EVisibility::HitTestInvisible)
	//.OwnerWorld(MyWorld);
	//MyGameMode->StrategyTitle->ShowTitle(FText::FromString(Localize(TEXT("Titles"), *LocKey, TEXT("StrategyGame"))));
}

void UStrategyGameBlueprintLibrary::ShowWaveTitle(UObject* WorldContextObject, int32 WaveNumber)
{
}
