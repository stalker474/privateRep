// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "MobaAnimInstance.h"


UMobaAnimInstance::UMobaAnimInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMobaAnimInstance::AnimNotify_Melee(UAnimNotify* Notify)
{
	// notify the pawn of the impact
	AMobaAICharacter* const MyChar = Cast<AMobaAICharacter>(TryGetPawnOwner());
	if (MyChar)
	{
		MyChar->OnMeleeImpactNotify();
	}
}

