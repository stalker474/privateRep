// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "MobaItem.h"
#include "MobaPlayerState.h"

AMobaPlayerState::AMobaPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Level = 1;
	Experience = 0;
	Money = 0;

	MyTeamNum = EStrategyTeam::Player;

	TArray<UObject*> ItemAssets;
	EngineUtils::FindOrLoadAssetsByPath(TEXT("/Game/Blueprints/Items/"), ItemAssets, EngineUtils::ATL_Regular);

	static ConstructorHelpers::FClassFinder<UMobaItem> ItemBPClass(TEXT("/Game/Blueprints/Items/PerunsBlessing"));
	if (ItemBPClass.Class != NULL)
	{
		AvailableItems.Add(ItemBPClass.Class);
	}

	ItemSlot1 = nullptr;
	ItemSlot2 = nullptr;
	ItemSlot3 = nullptr;
	ItemSlot4 = nullptr;
	ItemSlot5 = nullptr;
	ItemSlot6 = nullptr;

	ActiveItemSlot1 = nullptr;
	ActiveItemSlot2 = nullptr;
}

void AMobaPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMobaPlayerState, Level);
	DOREPLIFETIME(AMobaPlayerState, Experience);
	DOREPLIFETIME(AMobaPlayerState, Money);
	DOREPLIFETIME(AMobaPlayerState, MyTeamNum);
	DOREPLIFETIME(AMobaPlayerState, ItemSlot1);
	DOREPLIFETIME(AMobaPlayerState, ItemSlot2);
	DOREPLIFETIME(AMobaPlayerState, ItemSlot3);
	DOREPLIFETIME(AMobaPlayerState, ItemSlot4);
	DOREPLIFETIME(AMobaPlayerState, ItemSlot5);
	DOREPLIFETIME(AMobaPlayerState, ItemSlot6);
	DOREPLIFETIME(AMobaPlayerState, ActiveItemSlot1);
	DOREPLIFETIME(AMobaPlayerState, ActiveItemSlot2);
	DOREPLIFETIME(AMobaPlayerState, AvailableItems);
}

void AMobaPlayerState::LevelUp_Implementation()
{
	if (Level < 20)
	{
		Level++;
	}
}

bool AMobaPlayerState::LevelUp_Validate()
{
	return (Level < 20);
}

uint8 AMobaPlayerState::GetTeam() const
{
	return MyTeamNum;
}

FString AMobaPlayerState::GetName() const
{
	return PlayerName;
}

int AMobaPlayerState::GetLevel() const
{
	return Level;
}

int AMobaPlayerState::GetMoney() const
{
	return Money;
}

int AMobaPlayerState::GetExperience() const
{
	return Experience;
}

UMobaItem* AMobaPlayerState::GetItem(EItemSlot Slot) const
{
	switch (Slot)
	{
	case EItemSlot::SLOT_1:
		return ItemSlot1;
	case EItemSlot::SLOT_2:
		return ItemSlot2;
	case EItemSlot::SLOT_3:
		return ItemSlot3;
	case EItemSlot::SLOT_4:
		return ItemSlot4;
	case EItemSlot::SLOT_5:
		return ItemSlot5;
	case EItemSlot::SLOT_6:
		return ItemSlot6;
	}
	return nullptr;
}

UMobaItem* AMobaPlayerState::GetActiveItem(EActiveItemSlot Slot) const
{
	switch (Slot)
	{
	case EActiveItemSlot::SLOT_1:
		return ActiveItemSlot1;
	case EActiveItemSlot::SLOT_2:
		return ActiveItemSlot2;
	}
	return nullptr;
}

UMobaItem ** AMobaPlayerState::GetNextEmptySlot()
{
	if (ItemSlot1 == nullptr)
		return &ItemSlot1;
	else if (ItemSlot2 == nullptr)
		return &ItemSlot2;
	else if (ItemSlot3 == nullptr)
		return &ItemSlot3;
	else if (ItemSlot4 == nullptr)
		return &ItemSlot4;
	else if (ItemSlot5 == nullptr)
		return &ItemSlot5;
	else if (ItemSlot6 == nullptr)
		return &ItemSlot6;
	else
		return nullptr;
}

UMobaItem ** AMobaPlayerState::GetNextActiveEmptySlot()
{
	if (ActiveItemSlot1 == nullptr)
		return &ActiveItemSlot1;
	else if (ActiveItemSlot2 == nullptr)
		return &ActiveItemSlot2;
	else
		return nullptr;
}

void AMobaPlayerState::BuyItem_Implementation(TSubclassOf<class UMobaItem> ItemClass)
{
	UMobaItem ** freeSlot = GetNextEmptySlot();
	if (freeSlot != nullptr)
	{
		*freeSlot = NewObject<UMobaItem>(this, ItemClass);
		if ((*freeSlot)->Cost <= Money)
		{
			Money -= (*freeSlot)->Cost;
			(*freeSlot)->RegisterComponent();

			if ((*freeSlot)->Cost <= Money)
			{
				Money -= (*freeSlot)->Cost;
				(*freeSlot)->RegisterComponent();
			}
		}
	}
}

bool AMobaPlayerState::BuyItem_Validate(TSubclassOf<class UMobaItem> ItemClass)
{
	return ItemClass != nullptr && GetNextEmptySlot() != nullptr;
}
