// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "MobaTestCharacter.h"
#include "StrategyChar.h"
#include "FakeProjectile.h"


// Sets default values
AFakeProjectile::AFakeProjectile(const FObjectInitializer& ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MovementComp->bIsHomingProjectile = true;
}

// Called when the game starts or when spawned
void AFakeProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AFakeProjectile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	if (Target.IsValid())
	{
		ACharacter * anyChar = Cast<ACharacter>(Target.Get());
		if(anyChar)
			MovementComp->HomingTargetComponent = anyChar->GetMesh();
	}
}

void AFakeProjectile::OnImpact(const FHitResult& HitResult)
{
	//ignore everything else
	if(HitResult.Actor == Target)
		Super::OnImpact(HitResult);
}

