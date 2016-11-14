// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FakeProjectile.h"
#include "StaticWeapon.generated.h"

UCLASS()
class MOBATEST_API UStaticWeaponComponent : public UStaticMeshComponent
{
	GENERATED_UCLASS_BODY()
	
public:	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaSeconds, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile)
	TSubclassOf<AFakeProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float ReloadTime;

	UFUNCTION(Blueprintable)
	void LockAndLoad();

	UFUNCTION(Blueprintable)
	void Abort();

	UPROPERTY(Replicated)
	TWeakObjectPtr<APawn> Target;

	UPROPERTY(Replicated)
	bool IsAiming;

	inline void SetTarget(TWeakObjectPtr<APawn> myTarget) { Target = myTarget; }

private:
	void Shoot();

protected:
	FRotator DefaultRotation;
	float TimeSinceLastShot;
	
};
