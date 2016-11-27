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

	void InitializeComponent() override;

	UFUNCTION(BlueprintCallable, Category = Damage)
	float GetModifiedDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

	float ApplyDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = Stats)
	float BaseHealth;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = Stats)
	float BaseHealthRegen;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Stats)
	float Health;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
	float HealthLevelBonus;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
	float HealthRegenLevelBonus;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
	float BaseMagicDefense;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
	float BasePhysicalDefense;

	UFUNCTION(BlueprintCallable, Category = Stats)
	float GetModifiedHealth();

	UFUNCTION(BlueprintCallable, Category = Stats)
	float GetModifiedHealthRegen();

	UFUNCTION(BlueprintCallable, Category = Stats)
	float GetModifiedMagicDefense();

	UFUNCTION(BlueprintCallable, Category = Stats)
	float GetModifiedPhysicalDefense();

	UParticleSystemComponent * ParticleSystem;
};
