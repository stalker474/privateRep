// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneComponent.h"
#include "Ability.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EAbilityStateEnum : uint8
{
	NONE 	UMETA(DisplayName = "None"),
	AIMING 	UMETA(DisplayName = "Aiming"),
	CASTING UMETA(DisplayName = "Casting")
};

USTRUCT()
struct FAbilityClientData
{
	GENERATED_BODY()

	UPROPERTY()
	FRotator CamFrwd;
	UPROPERTY()
	FVector AimingPosition;
	UPROPERTY()
	FRotator AimingRotation;
};

UCLASS()
class MOBATEST_API UAbility : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:	

	// Called when the game starts
	virtual void BeginPlay() override;

	void OnReloaded();

	void Aim();

	virtual void UpdateState() {};

	virtual void InitializeComponent() override;

	void UpdateClientData(const FAbilityClientData& AbilityClientData);

	void StartCast(float castingTime);

	virtual void OnCast();

	void Cancel();

	UFUNCTION(BlueprintCallable, Category = Stats)
	void LevelUp();

	virtual bool CanLevelUp();

	EAbilityStateEnum State;

	UPROPERTY(EditDefaultsOnly, Category = Timers)
	float ReloadingTime;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = AbilityCost)
	float Level1ManaCost;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = AbilityCost)
	float Level2ManaCost;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = AbilityCost)
	float Level3ManaCost;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = AbilityCost)
	float Level4ManaCost;

	UFUNCTION(BlueprintCallable, Category = Stats)
	float GetModifiedManaCost();

	UFUNCTION(BlueprintCallable, Category = Timers)
	float GetTimeUntilReady();

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = GUI)
	class UTexture2D* Icon;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int Level;

	UPROPERTY(Replicated)
	bool Reloaded;

protected:

	FTimerHandle ReloadTimerHandle;
	FTimerHandle CastingTimerHandle;
	
	FAbilityClientData ClientData;
	
};
