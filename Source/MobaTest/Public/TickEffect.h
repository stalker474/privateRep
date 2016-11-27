// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "TickEffect.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(TickEffectLog, Log, All);

/**
 * 
 */
UCLASS(Blueprintable)
class MOBATEST_API UTickEffect : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Configuration)
	float Duration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Configuration)
	float ApplyEveryDt;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Configuration)
	float DamageValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Configuration)
	float SpeedFactor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Configuration)
	float HealthFactor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Configuration)
	float ManaFactor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Configuration)
	float WeaponDamageFactor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Configuration)
	float MagicArmorFactor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Configuration)
	float PhysicalArmorFactor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Configuration)
	bool DisablePrimaryAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Configuration)
	bool DisableCastingAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Configuration)
	bool Invulnerable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Configuration)
	UParticleSystem * Particles;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Configuration)
	TSubclassOf<class UDamageType> DamageType;

	void Apply(float dT);

	inline bool GetIsActive() { return !IsOver; };

	inline void SetOwner(UActorComponent * MyOwner) { Owner = MyOwner; }
	

private:
	float TotalElapsedTime;
	float TimeSinceLastTick;
	bool IsOver;
	bool AppliedOnce;

	float BaseSpeed; //used to save speed before applying modifier

	TWeakObjectPtr<UActorComponent> Owner;
};
