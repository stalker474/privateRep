// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Public/StrategyTypes.h"
#include "Pawns/MobaAICharacter.h"
#include "Public/Interfaces/StrategyTeamInterface.h"
#include "StrategyChar.generated.h"

class UStrategyAttachment;

// Base class for the minions
UCLASS(Abstract)
class AStrategyChar : public AMobaAICharacter, public IStrategyTeamInterface
{
	GENERATED_UCLASS_BODY()

public:

	// Begin StrategyTeamInterface interface
	UFUNCTION(BlueprintCallable, Category = "Team")
	virtual uint8 GetTeamNum() const override;
	// End StrategyTeamInterface interface

	/** set attachment for weapon slot */
	UFUNCTION(BlueprintCallable, Category=Attachment)
	void SetWeaponAttachment(UStrategyAttachment* Weapon);

	UFUNCTION(BlueprintCallable, Category=Attachment)
	bool IsWeaponAttached();

	/** set attachment for armor slot */
	UFUNCTION(BlueprintCallable, Category=Attachment)
	void SetArmorAttachment(UStrategyAttachment* Armor);

	UFUNCTION(BlueprintCallable, Category=Attachment)
	bool IsArmorAttached();

	/** set team number */
	void SetTeamNum(uint8 NewTeamNum); 

	ELaneEnum LanePosition;

protected:
	
	/** Armor attachment slot */
	UPROPERTY()
	UStrategyAttachment* ArmorSlot;

	/** Weapon attachment slot */
	UPROPERTY()
	UStrategyAttachment* WeaponSlot;

	/** team number */
	UPROPERTY(Replicated)
	uint8 MyTeamNum;
};

