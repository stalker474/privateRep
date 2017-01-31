// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.


#include "MobaTest.h"
#include "StrategySpectatorPawn.h"
#include "StrategyCameraComponent.h"
#include "StrategySpectatorPawnMovement.h"

AStrategySpectatorPawn::AStrategySpectatorPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/*CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->ProbeChannel = ECC_WorldStatic;*/

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(RootComponent);
	FollowCamera->SetRelativeLocation(FVector(0, 0, 0));
	FollowCamera->SetRelativeRotation(FRotator(0, -45.0, 0));
	//FollowCamera->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
	FollowCamera->FieldOfView = 90.0f;
	
	bReplicates = true;
}

void AStrategySpectatorPawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AStrategySpectatorPawn, TeamNum);
}

void AStrategySpectatorPawn::OnMouseScrollUp()
{
	
}

void AStrategySpectatorPawn::OnMouseScrollDown()
{
	
}


void AStrategySpectatorPawn::SetupPlayerInputComponent(UInputComponent* _InputComponent)
{
	check(_InputComponent);
	InputComponent = _InputComponent;
	
	InputComponent->BindAction("ZoomOut", IE_Pressed, this, &AStrategySpectatorPawn::OnMouseScrollUp);
	InputComponent->BindAction("ZoomIn", IE_Pressed, this, &AStrategySpectatorPawn::OnMouseScrollDown);

	InputComponent->BindAxis("MoveForward", this, &AStrategySpectatorPawn::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AStrategySpectatorPawn::MoveRight);
}


void AStrategySpectatorPawn::MoveForward(float Val)
{
	FVector direction = GetActorForwardVector();
	GetMovementComponent()->AddInputVector(direction * Val,true);
}


void AStrategySpectatorPawn::MoveRight(float Val)
{
	FVector direction = this->GetActorRightVector();
	GetMovementComponent()->AddInputVector(direction * Val, true);
}

uint8 AStrategySpectatorPawn::GetTeamNum() const
{
	return TeamNum;
}

