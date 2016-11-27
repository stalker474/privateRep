// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "Ability.h"
#include "UnrealNetwork.h"


// Sets default values for this component's properties
UAbility::UAbility(const FObjectInitializer& ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
	bReplicates = true;

	State = EAbilityStateEnum::NONE;
	UpdateState();

	Reloaded = true;
	ReloadingTime = 1.0f;
	ClientData.CamFrwd = FRotator::ZeroRotator;
	ClientData.AimingPosition = FVector::ZeroVector;

	Level = 1;

	Level1ManaCost = 0;
	Level2ManaCost = 0;
	Level3ManaCost = 0;
	Level4ManaCost = 0;
	// ...
}

// Called when the game starts
void UAbility::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UAbility::InitializeComponent()
{
	Super::InitializeComponent();
}

void UAbility::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAbility, Reloaded);
	DOREPLIFETIME(UAbility, Level1ManaCost);
	DOREPLIFETIME(UAbility, Level2ManaCost);
	DOREPLIFETIME(UAbility, Level3ManaCost);
	DOREPLIFETIME(UAbility, Level4ManaCost);
	DOREPLIFETIME(UAbility, Icon);
	DOREPLIFETIME(UAbility, Level);
}

void UAbility::Aim()
{
	if (Level > 0)
	{
		State = EAbilityStateEnum::AIMING;
		UpdateState();
	}
}

void UAbility::StartCast(float castingTime)
{
	State = EAbilityStateEnum::CASTING;
	UpdateState();
	GetWorld()->GetTimerManager().SetTimer(CastingTimerHandle, this, &UAbility::OnCast, castingTime);
}

void UAbility::OnCast()
{
	Reloaded = false;
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &UAbility::OnReloaded, ReloadingTime);
}

void UAbility::Cancel()
{
	State = EAbilityStateEnum::NONE;
	UpdateState();
}

void UAbility::LevelUp()
{
	if (Level < 4 && CanLevelUp())
		Level++;
}

bool UAbility::CanLevelUp()
{
	return true;
}

float UAbility::GetModifiedManaCost()
{
	switch (Level)
	{
	case 0:
		return 0; //no mana cost and no ability to cast
	case 1:
		return Level1ManaCost;
	case 2:
		return Level2ManaCost;
	case 3:
		return Level3ManaCost;
	case 4:
		return Level4ManaCost;
	}
	return 0;
}

float UAbility::GetTimeUntilReady()
{
	if(!Reloaded)
		return GetWorld()->GetTimerManager().GetTimerRemaining(ReloadTimerHandle);
	else return 0.0f;
}

void UAbility::OnReloaded()
{
	if (GetOwnerRole() == ROLE_Authority) //only server decides when it's ready
	{
		Reloaded = true;
	}
	
}

void UAbility::UpdateClientData(const FAbilityClientData& clientData)
{
	ClientData = clientData;
}
