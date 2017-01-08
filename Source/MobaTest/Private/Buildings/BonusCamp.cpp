// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "BonusCamp.h"


ABonusCamp::ABonusCamp(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	AIDirector = CreateDefaultSubobject<UBonusCampAIDirector>(TEXT("AIDirectorComp"));
	AIDirector->SetIsReplicated(false);
}

// Called when the game starts or when spawned
void ABonusCamp::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABonusCamp::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ABonusCamp::OnGameplayStateChange(EGameplayState::Type NewState)
{
	if (Role == ROLE_Authority)
	{
		if (AIDirector != nullptr)
		{
			AIDirector->OnGameplayStateChange(NewState);
		}
	}
}