// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MobaTest.h"
#include "StrategyAIController.h"
#include "StrategyAttachment.h"

#include "VisualLogger/VisualLogger.h"

AStrategyChar::AStrategyChar(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent>(ACharacter::CharacterMovementComponentName)) 
{
}

void AStrategyChar::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AStrategyChar, MyTeamNum);
}

void AStrategyChar::SetWeaponAttachment(UStrategyAttachment* Weapon)
{
	if (WeaponSlot != Weapon)
	{
		// detach any existing weapon attachment
		if (WeaponSlot )
		{
			WeaponSlot->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		}

		// attach this one
		WeaponSlot = Weapon;
		if (WeaponSlot )
		{
			WeaponSlot->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSlot->AttachPoint);
		}
	}
}

void AStrategyChar::SetArmorAttachment(UStrategyAttachment* Armor)
{
	if (ArmorSlot != Armor)
	{
		// detach any existing armor attachment
		if (ArmorSlot )
		{
			ArmorSlot->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		}
		// attach this one
		ArmorSlot = Armor;
		if (ArmorSlot )
		{
			ArmorSlot->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, ArmorSlot->AttachPoint);
		}
	}
}

bool AStrategyChar::IsWeaponAttached()
{
	return WeaponSlot != nullptr;
}

bool AStrategyChar::IsArmorAttached()
{
	return ArmorSlot != nullptr;
}

uint8 AStrategyChar::GetTeamNum() const
{
	return MyTeamNum;
}

void AStrategyChar::SetTeamNum(uint8 NewTeamNum)
{
	MyTeamNum = NewTeamNum;
}
