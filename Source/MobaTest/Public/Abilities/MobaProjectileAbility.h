// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Ability.h"
#include "MobaProjectileAbility.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class MOBATEST_API UMobaProjectileAbility : public UAbility
{
	GENERATED_UCLASS_BODY()

public:

	void OnCast() override;

	UPROPERTY(EditAnywhere, Category = Projectile)
	TSubclassOf<class AMobaProjectile> ProjectileClass;
};
