// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "BonusCamp.h"


ABonusCamp::ABonusCamp(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	AIDirector = CreateDefaultSubobject<UBonusCampAIDirector>(TEXT("AIDirectorComp"));
	AIDirector->SetIsReplicated(false);
	CapsuleTriggerBox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleTrigger"));
	CapsuleTriggerBox->bAutoActivate = true;
	CapsuleTriggerBox->SetIsReplicated(false);
	CapsuleTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ABonusCamp::OnOverlapBegin);
	CapsuleTriggerBox->OnComponentEndOverlap.AddDynamic(this, &ABonusCamp::OnOverlapEnd);
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

void ABonusCamp::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		AMobaTestCharacter * player = Cast<AMobaTestCharacter>(OtherActor);
		if (player)
		{
			if (AIDirector != nullptr && AIDirector->MonsterAIController.IsValid())
				AIDirector->MonsterAIController->AddAuthorizedTarget(player);
		}
	}
}

void ABonusCamp::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		AMobaTestCharacter * player = Cast<AMobaTestCharacter>(OtherActor);
		if (player)
		{
			if (AIDirector != nullptr && AIDirector->MonsterAIController.IsValid())
			{
				AIDirector->MonsterAIController->RemoveAuthorizedTarget(player);
			}
				
		}
	}
}