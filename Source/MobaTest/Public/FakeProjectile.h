// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MobaProjectile.h"
#include "FakeProjectile.generated.h"

UCLASS()
class MOBATEST_API AFakeProjectile : public AMobaProjectile
{
	GENERATED_UCLASS_BODY()

	void OnImpact(const FHitResult& HitResult);
	
public:	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Target)
	TWeakObjectPtr<APawn> Target;
};
