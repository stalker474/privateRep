// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "StrategyTypes.generated.h"

#pragma once

UENUM(BlueprintType)
namespace EStrategyTeam
{
	enum Type
	{
		Spectator,
		Blue,
		Red,
		MAX
	};
}

namespace EGameKey
{
	enum Type
	{
		Tap,
		Hold,
		Swipe,
		SwipeTwoPoints,
		Pinch,
	};
}

namespace EGameplayState
{
	enum Type
	{
		Joining,
		ChosingCharacter,
		Waiting,
		Playing,
		Finished,
	};
}

UENUM()
namespace EGameDifficulty
{
	enum Type
	{
		Easy,
		Medium,
		Hard,
	};
}

DECLARE_DELEGATE_RetVal(bool, FActionButtonDelegate);
DECLARE_DELEGATE_RetVal(FText, FGetQueueLength)

struct FActionButtonData
{
	FText StrButtonText;
	FText StrTooltip;
	EVisibility Visibility;
	bool bIsEnabled;
	int32 ActionCost;
	UTexture2D*	ActionIcon;
	FActionButtonDelegate TriggerDelegate;
	FGetQueueLength GetQueueLengthDelegate;

	FActionButtonData()
	{
		StrButtonText = FText::GetEmpty();
		StrTooltip = FText::GetEmpty();
		bIsEnabled = true;
		ActionCost = 0;
		Visibility = EVisibility::Hidden;
		ActionIcon = NULL;
	}
};

struct FActionButtonInfo
{
	TSharedPtr<class SStrategyButtonWidget> Widget;
	FActionButtonData Data;
};

USTRUCT()
struct FPawnData
{
	GENERATED_USTRUCT_BODY()

	/** minimal attack damage */
	UPROPERTY(EditDefaultsOnly, Category=Data)
	int32 AttackMin;

	/** maximum attack damage */
	UPROPERTY(EditDefaultsOnly, Category=Data)
	int32 AttackMax;

	/** range of pawn attack */
	UPROPERTY(EditDefaultsOnly, Category=Data)
	int32 AttackDistance;

	/** damage reduction */
	UPROPERTY(EditDefaultsOnly, Category=Data)
	int32 DamageReduction;

	/** maximum health increase */
	UPROPERTY(EditDefaultsOnly, Category=Data)
	int32 MaxHealthBonus;

	/** health change every 5 seconds */
	UPROPERTY(EditDefaultsOnly, Category=Data)
	int32 HealthRegen;

	/** movement speed */
	UPROPERTY(EditDefaultsOnly, Category=Data)
	float Speed;

	/** defaults */
	FPawnData()
	{
		AttackMin = 5;
		AttackMax = 10;
		DamageReduction = 0;
		MaxHealthBonus = 0;
		HealthRegen = 0;
		Speed = 0.0;
		AttackDistance = 100;
	}
};

USTRUCT()
struct FBuffData
{
	GENERATED_USTRUCT_BODY()

	/** additive values */
	UPROPERTY(EditDefaultsOnly, Category=Data)
	FPawnData BuffData;

	/** set to ignore buff duration, not time limited */
	UPROPERTY(EditDefaultsOnly, Category=Buff)
	uint8 bInfiniteDuration : 1;

	/** buff duration in seconds */
	UPROPERTY(EditDefaultsOnly, Category=Buff)
	float Duration;

	/** runtime: buff ending time calculated when it's added */
	float EndTime;

	/** defaults */
	FBuffData()
	{
		bInfiniteDuration = false;
		Duration = 20.0f;
		EndTime = 0.0f;
	}

	/** 
	* Helper function for applying buff data.
	*
	* @param	PawnData		Data to apply.
	*/
	void ApplyBuff(struct FPawnData& PawnData);
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ELaneEnum : uint8
{
	VE_Top		UMETA(DisplayName = "Top"),
	VE_Mid		UMETA(DisplayName = "Mid"),
	VE_Short	UMETA(DisplayName = "Short")
};

struct FPlayerData
{

	/** HQ */
	TWeakObjectPtr<class AStrategyBuilding_Brewery> BreweryLaneTop;
	TWeakObjectPtr<class AStrategyBuilding_Brewery> BreweryLaneMid;
	TWeakObjectPtr<class AStrategyBuilding_Brewery> BreweryLaneShort;

	TWeakObjectPtr<class AStrategyBuilding_Brewery> GetLaneBuilding(ELaneEnum lane)
	{
		if (lane == ELaneEnum::VE_Top)
			return BreweryLaneTop;
		else if (lane == ELaneEnum::VE_Mid)
			return BreweryLaneMid;
		else
			return BreweryLaneShort;
	}
};
