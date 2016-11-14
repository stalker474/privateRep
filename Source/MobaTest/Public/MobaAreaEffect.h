// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "TickEffect.h"
#include "MobaAreaEffect.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(AreaEffectLog, Log, All);

class AMobaAreaEffect;

USTRUCT()
struct FAreaWeaponData
{
	GENERATED_USTRUCT_BODY()

	/** effect class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect)
	TSubclassOf<AMobaAreaEffect> AreaEffectClass;

	/** life time */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect)
	float AreaLife;

	/** type of damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponStat)
	TSubclassOf<UDamageType> DamageType;

	/** type of effects */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponStat)
	TArray<TSubclassOf<UTickEffect>> Effects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponStat)
	float Damage;

	/** defaults */
	FAreaWeaponData()
	{
		AreaEffectClass = NULL;
		AreaLife = 10.0f;
		DamageType = UDamageType::StaticClass();
	}
};

UCLASS(Abstract, Blueprintable)
class MOBATEST_API AMobaAreaEffect : public AActor
{
	GENERATED_UCLASS_BODY()

	/** initial setup */
	virtual void PostInitializeComponents() override;

public:
	/** collisions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision)
	UShapeComponent* CollisionComp;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	USphereComponent* CollisionCompSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	UBoxComponent* CollisionCompBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision)
	ECollisionShapeEnum CollisionShape;*/

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UParticleSystemComponent* ParticleComp;

	/** projectile data */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponStat)
	struct FAreaWeaponData WeaponConfig;

	/** handle hit */
	UFUNCTION()
	void OnImpact(AActor* self, AActor* victim);

protected:
	/** controller that fired me (cache for damage calculations) */
	TWeakObjectPtr<AController> MyController;

	/** shutdown area and prepare for destruction */
	void DisableAndDestroy();

	/** Returns CollisionComp subobject **/
	//FORCEINLINE UShapeComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ParticleComp subobject **/
	//FORCEINLINE UParticleSystemComponent* GetParticleComp() const { return ParticleComp; }
};
