// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Ability.h"
#include "MobaAreaDamageAbility.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(YourLog, Log, All);

/**
 * 
 */
UCLASS(Blueprintable)
class MOBATEST_API UMobaAreaDamageAbility : public UAbility
{
	GENERATED_UCLASS_BODY()

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void OnCast() override;

	void UpdateState() override;

	UPROPERTY(EditAnywhere, Category = State1_Material)
	TSubclassOf<class AActor> TargetActorClass;

	UPROPERTY(EditAnywhere, Category = Effect)
	TSubclassOf<class AMobaAreaEffect> AreaEffectClass;

	UPROPERTY(EditAnywhere, Category = MaxDistance)
	float MaxDistance;

	class AActor * TargetActor;

};
