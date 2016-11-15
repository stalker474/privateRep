// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "TickEffect.h"
#include "MobaCombatCharacterComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MOBATEST_API UMobaCombatCharacterComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:	
	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	float ApplyDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = Stats)
	float MaxHealth;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = Stats)
	float Health;

	UParticleSystemComponent * ParticleSystem;

	TArray<UTickEffect*> TickEffects;
};
