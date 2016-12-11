// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "MobaItem.h"
#include "StrategySpawnPoint.h"
#include "MobaPlayerState.h"

AMobaPlayerState::AMobaPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	State = EMobaState::ChosingCharacter;

	SelectedChar = nullptr;
	MyStart = nullptr;

	Level = 1;
	Experience = 0;
	Money = 1000; //set to 0 later

	MyTeamNum = EStrategyTeam::Blue;

	TArray<UObject*> ItemAssets;
	EngineUtils::FindOrLoadAssetsByPath(TEXT("/Game/Blueprints/Items/"), ItemAssets, EngineUtils::ATL_Regular);

	static ConstructorHelpers::FClassFinder<UMobaItem> ItemBPClass(TEXT("/Game/Blueprints/Items/PerunsBlessing"));
	if (ItemBPClass.Class != NULL)
	{
		AvailableItems.Add(ItemBPClass.Class);
	}
	static ConstructorHelpers::FClassFinder<UMobaItem> ItemBPClass2(TEXT("/Game/Blueprints/Items/ManaPotion"));
	if (ItemBPClass2.Class != NULL)
	{
		AvailableItems.Add(ItemBPClass2.Class);
	}
	static ConstructorHelpers::FClassFinder<UMobaItem> ItemBPClass3(TEXT("/Game/Blueprints/Items/HealthPotion"));
	if (ItemBPClass3.Class != NULL)
	{
		AvailableItems.Add(ItemBPClass3.Class);
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
	DOREPLIFETIME(AMobaPlayerState, SelectedCharClass);
	DOREPLIFETIME(AMobaPlayerState, State);
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

void AMobaPlayerState::CopyProperties(class APlayerState* PlayerState)
{

	APlayerState::CopyProperties(PlayerState);

	AMobaPlayerState* MyPlayerState = Cast<AMobaPlayerState>(PlayerState);

	if (MyPlayerState)
	{
		MyPlayerState->SelectedCharClass = SelectedCharClass;
		MyPlayerState->MyTeamNum = MyTeamNum;
		MyPlayerState->State = State;
	}

}

void AMobaPlayerState::RegisterPlayerWithSession(bool bWasFromInvite)
{
	Super::RegisterPlayerWithSession(bWasFromInvite);
}

void AMobaPlayerState::ChoseCharacter_Implementation(TSubclassOf<class AMobaTestCharacter> CharacterClass)
{
	UWorld* const world = GetWorld(); // get a reference to the world
	if (world) {
		APlayerStart* Start = MyStart;
		if (!MyStart)
		{
			for (TActorIterator<APlayerStart> It(world); It; ++It)
			{
				Start = *It;
				AStrategySpawnPoint * thisStartPoint = Cast<AStrategySpawnPoint>(Start);
				if (thisStartPoint && thisStartPoint->GetTeamNum() == GetTeam() && thisStartPoint->IsSpawnable && !thisStartPoint->Player)
				{
					thisStartPoint->Player = GetNetOwningPlayer()->PlayerController;
					thisStartPoint->MULTICAST_PlayerChanged(PlayerName);
					MyStart = thisStartPoint;
					break;
				}
			}
		}

		if (Start)
		{
			if (SelectedChar && SelectedChar->GetClass() == CharacterClass)
				return;
			if (SelectedChar)
				SelectedChar->Destroy();

			SelectedChar = world->SpawnActor<AMobaTestCharacter>(CharacterClass, Start->GetActorLocation(), Start->GetActorRotation());
			SelectedCharClass = CharacterClass;

		}
	}
}

void AMobaPlayerState::Ready_Implementation()
{
	AStrategyGameMode * gameMode = Cast<AStrategyGameMode>(GetWorld()->GetAuthGameMode());
	gameMode->Ready(GetNetOwningPlayer()->PlayerController);
}

void AMobaPlayerState::SendTextMessage_Implementation(const FString& msg)
{
	AStrategyGameState * gs = Cast<AStrategyGameState>(GetWorld()->GetGameState());
	if (gs)
	{
		gs->ChatText += GetNetOwningPlayer()->GetName() + ": " + msg + "\n";
	}
}

bool AMobaPlayerState::ChoseCharacter_Validate(TSubclassOf<class AMobaTestCharacter> CharacterClass)
{
	AStrategyGameState * gs = Cast<AStrategyGameState>(GetWorld()->GetGameState());
	if (gs)
	{
		return gs->AvailableCharacters.Contains(CharacterClass);
	}
	else
		return false;
}

bool AMobaPlayerState::Ready_Validate()
{
	return SelectedChar != nullptr;
}

void AMobaPlayerState::LevelUp_Implementation()
{
	if (Level < 20)
	{
		Level++;
	}
}

bool AMobaPlayerState::SendTextMessage_Validate(const FString& msg)
{
	return true;
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

void AMobaPlayerState::EmptySlot(EItemSlot Slot)
{
	switch (Slot)
	{
	case EItemSlot::SLOT_1:
		ItemSlot1 = nullptr;
		break;
	case EItemSlot::SLOT_2:
		ItemSlot2 = nullptr;
		break;
	case EItemSlot::SLOT_3:
		ItemSlot3 = nullptr;
		break;
	case EItemSlot::SLOT_4:
		ItemSlot4 = nullptr;
		break;
	case EItemSlot::SLOT_5:
		ItemSlot5 = nullptr;
		break;
	case EItemSlot::SLOT_6:
		ItemSlot6 = nullptr;
		break;
	}
}

void AMobaPlayerState::EmptyActiveSlot(EActiveItemSlot Slot)
{
	switch (Slot)
	{
	case EItemSlot::SLOT_1:
		ActiveItemSlot1 = nullptr;
		break;
	case EItemSlot::SLOT_2:
		ActiveItemSlot2 = nullptr;
		break;
	}
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
	UMobaItem ** freeSlot;

	if (!ItemClass.GetDefaultObject()->Consumable)
		freeSlot = GetNextEmptySlot();
	else
		freeSlot = GetNextActiveEmptySlot();
	 
	if (freeSlot != nullptr)
	{
		(*freeSlot) = NewObject<UMobaItem>(this, ItemClass);
		if ((*freeSlot)->Cost <= Money)
		{
			(*freeSlot)->RegisterComponent();

			if ((*freeSlot)->Cost <= Money)
			{
				Money -= (*freeSlot)->Cost;
				(*freeSlot)->RegisterComponent();
			}
		}
	}
}

void AMobaPlayerState::SellItem_Implementation(EItemSlot Slot)
{
	UMobaItem * item = GetItem(Slot);
	if (item != nullptr)
	{
		Money += item->SellPrice;
		item->UnregisterComponent();
		EmptySlot(Slot);
	}
}

void AMobaPlayerState::SellActiveItem_Implementation(EActiveItemSlot Slot)
{
	UMobaItem * item = GetActiveItem(Slot);
	if (item != nullptr)
	{
		Money += item->SellPrice;
		item->UnregisterComponent();
		EmptyActiveSlot(Slot);
	}
}

void AMobaPlayerState::ActivateItem_Implementation(EActiveItemSlot Slot)
{
	UMobaItem * item = GetActiveItem(Slot);
	if (item != nullptr)
	{
		item->UnregisterComponent();
		EmptyActiveSlot(Slot);
	}
}

bool AMobaPlayerState::BuyItem_Validate(TSubclassOf<class UMobaItem> ItemClass)
{
	return ItemClass != nullptr && GetNextEmptySlot() != nullptr;
}

bool AMobaPlayerState::SellItem_Validate(EItemSlot Slot)
{
	return GetItem(Slot) != nullptr;
}

bool AMobaPlayerState::SellActiveItem_Validate(EActiveItemSlot Slot)
{
	return GetActiveItem(Slot) != nullptr;
}

bool AMobaPlayerState::ActivateItem_Validate(EActiveItemSlot Slot)
{
	return GetActiveItem(Slot) != nullptr;
}
