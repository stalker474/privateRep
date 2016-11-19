// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MobaTest.h"
#include "StrategyBuilding.h"
#include "GameFramework/PlayerController.h"
#include "StrategySelectionInterface.h"


AStrategyBuilding::AStrategyBuilding(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
	, BuildingName(TEXT("Unknown"))
	, SpawnTeamNum(EStrategyTeam::Unknown)
	,Active(true)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bReplicates = true;

	USceneComponent* const TranslationComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	TranslationComp->Mobility = EComponentMobility::Static;
	RootComponent = TranslationComp;

	ParticleComp = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("ParticleComp"));
	ParticleComp->bAutoActivate = false;
	ParticleComp->bAutoDestroy = false;
	ParticleComp->SetupAttachment(RootComponent);
	ParticleComp->SetIsReplicated(true);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	TriggerBox->bVisible = true;
	TriggerBox->bHiddenInGame = true;
	TriggerBox->CastShadow = false;
	TriggerBox->InitBoxExtent(FVector(512, 128, 128));
	TriggerBox->RelativeLocation = FVector(512, 0, 128);
	TriggerBox->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->BodyInstance.SetResponseToAllChannels(ECR_Ignore);
	TriggerBox->BodyInstance.SetResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	//TriggerBox->AttachParent = RootComponent;

	bCanBeDamaged = false;
}

void AStrategyBuilding::PostLoad()
{
	Super::PostLoad();
	// only do this for BP
	if(IsTemplate() && RootComponent)
	{
		RootComponent->RelativeRotation = FRotator::ZeroRotator;
		RootComponent->RelativeLocation = FVector::ZeroVector;
	}
}

void AStrategyBuilding::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AStrategyBuilding, SpawnTeamNum);
}

void AStrategyBuilding::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AStrategyBuilding::Destroyed()
{
	Super::Destroyed();
}

void AStrategyBuilding::Deactivate()
{
	Active = false;
	MULTICAST_Deactivate();
}

void AStrategyBuilding::MULTICAST_Deactivate_Implementation()
{
	ParticleComp->ActivateSystem(true);
}

bool AStrategyBuilding::IsActive()
{
	return Active;
}

FPlayerData* AStrategyBuilding::GetTeamData() const
{
	check( GetWorld() );	
	AStrategyGameState* const StrategyGame = GetWorld()->GetGameState<AStrategyGameState>();
	if( StrategyGame != nullptr )
	{
		return StrategyGame->GetPlayerData(SpawnTeamNum);
	}
	return nullptr;
}

void AStrategyBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FString AStrategyBuilding::GetBuildingName() const
{
	return BuildingName;
}

uint8 AStrategyBuilding::GetTeamNum() const
{
	return SpawnTeamNum;
}