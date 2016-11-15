// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "StrategyBuilding.h"
#include "StaticWeapon.h"
#include "MobaTower.generated.h"

UCLASS()
class MOBATEST_API AMobaTower : public AStrategyBuilding
{
	GENERATED_UCLASS_BODY()
	
public:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Tower, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CombatComponent)
	UMobaCombatCharacterComponent * CombatComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CombatComponent)
	UDecalComponent * DecalComponent;

	UPROPERTY(EditAnywhere, Category = Weapon)
	UStaticWeaponComponent * Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	float ReloadTime;

	virtual void PostInitializeComponents() override;

	/**
	* Starts melee attack.
	* @return Duration of the attack anim.
	*/
	float Attack(TWeakObjectPtr<APawn> Target);

	void Abort();

	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

private:
	
	
};
