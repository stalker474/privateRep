// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimInstance.h"
#include "MobaAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MOBATEST_API UMobaAnimInstance : public UAnimInstance
{
	GENERATED_UCLASS_BODY()

	/**
	*  Notify pawn of the impact.
	*
	* @param Notify		The AnimNotify instance that triggered this notification.
	*/
	UFUNCTION()
	void AnimNotify_Melee(UAnimNotify* Notify);
	
	
	
	
};
