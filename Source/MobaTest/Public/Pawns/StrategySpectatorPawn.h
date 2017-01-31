// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyTypes.h"
#include "StrategySpectatorPawn.generated.h"

//@TODO: Write a comment here
UCLASS(Blueprintable, BlueprintType)
class AStrategySpectatorPawn : public ADefaultPawn, public IStrategyTeamInterface
{
	GENERATED_UCLASS_BODY()

	// Begin ADefaultPawn interface

	/** event call on move forward input */
	virtual void MoveForward(float Val);

	/** event call on strafe right input */
	virtual void MoveRight(float Val);

	/** Add custom key bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	// End ADefaultPawn interface

	/** Camera boom positioning the camera behind the character */
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:

	/** Handles the mouse scrolling down. */
	void OnMouseScrollUp();

	/** Handles the mouse scrolling up. */
	void OnMouseScrollDown();

	// Begin StrategyTeamInterface Interface
	UFUNCTION(BlueprintCallable, Category = "Team")
	virtual uint8 GetTeamNum() const override;
	// End StrategyTeamInterface Interface

	UPROPERTY(Replicated, EditInstanceOnly, Category = Building)
	TEnumAsByte<EStrategyTeam::Type> TeamNum;
};


