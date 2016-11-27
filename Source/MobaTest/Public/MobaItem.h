// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "TickEffect.h"
#include "MobaItem.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EItemSlot : uint8
{
	SLOT_1 	UMETA(DisplayName = "Slot1"),
	SLOT_2 	UMETA(DisplayName = "Slot2"),
	SLOT_3 	UMETA(DisplayName = "Slot3"),
	SLOT_4 	UMETA(DisplayName = "Slot4"),
	SLOT_5 	UMETA(DisplayName = "Slot5"),
	SLOT_6 	UMETA(DisplayName = "Slot6"),
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EActiveItemSlot : uint8
{
	SLOT_1 	UMETA(DisplayName = "Slot1"),
	SLOT_2 	UMETA(DisplayName = "Slot2"),
};

USTRUCT()
struct FDefaultIconStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GUI)
	class UTexture2D* Icon;

	//Constructor
	FDefaultIconStruct()
	{
		//Always initialize your USTRUCT variables!
		//   exception is if you know the variable type has its own default constructor
		Icon = nullptr;
	}
};
/**
 * 
 */
UCLASS(Blueprintable)
class MOBATEST_API UMobaItem : public UActorComponent
{
	GENERATED_UCLASS_BODY()
	
public:
	UFUNCTION()
	void Apply(TArray<UTickEffect*>& TickEffects);

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = GUI)
	FString Name;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = GUI)
	FString Description;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = GUI)
	struct FDefaultIconStruct Icon;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
	int Cost;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
	int SellPrice;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
	bool Consumable;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
	TArray<TSubclassOf<UTickEffect>> Effects;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
	TSubclassOf<UTickEffect> ParentItem;
};
