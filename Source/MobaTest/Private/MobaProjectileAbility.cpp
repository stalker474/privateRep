// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "MobaProjectileAbility.h"
#include "MobaProjectile.h"

// Sets default values for this component's properties
UMobaProjectileAbility::UMobaProjectileAbility(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void UMobaProjectileAbility::OnCast()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		Super::OnCast();
		if (ProjectileClass != NULL)
		{
			UWorld* const World = GetWorld();
			if (World)
			{
				FTransform mTransform;
				mTransform.SetIdentity();

				ACharacter * myPlayer = Cast<ACharacter>(GetOwner());
				FVector vec = myPlayer->GetActorLocation();
				FVector forward = myPlayer->GetActorForwardVector();

				//offset projectile spawn by a few
				mTransform.SetLocation(vec + forward * 2);
				//just aim where the camera is pointing
				mTransform.SetRotation(ClientData.CamFrwd.Quaternion());

				FActorSpawnParameters spawnParams;
				spawnParams.Owner = GetOwner();
				spawnParams.Instigator = Cast<APawn>(GetOwner());

				AMobaProjectile* myProjectile = World->SpawnActor<AMobaProjectile>(ProjectileClass, mTransform, spawnParams);
			}

		}
		State = EAbilityStateEnum::NONE;
		UpdateState();
	}
}




