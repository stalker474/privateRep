// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "Ability.h"
#include "UnrealNetwork.h"


// Sets default values for this component's properties
UAbility::UAbility(const FObjectInitializer& ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
	bReplicates = true;

	State = EAbilityStateEnum::NONE;
	UpdateState();

	Reloaded = true;
	ReloadingTime = 1.0f;
	ClientData.CamFrwd = FRotator::ZeroRotator;
	ClientData.AimingPosition = FVector::ZeroVector;
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
	DOREPLIFETIME(UAbility, ManaCost);
	DOREPLIFETIME(UAbility, Icon);
}

void UAbility::Aim()
{
	State = EAbilityStateEnum::AIMING;
	UpdateState();
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
