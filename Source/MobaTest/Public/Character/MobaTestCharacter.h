// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "StrategyTeamInterface.h"
#include "CombatAnimatedInterface.h"
#include "CharacterAbilityCasterComponent.h"
#include "MobaCombatCharacterComponent.h"
#include "MobaItem.h"
#include "MobaTestCharacter.generated.h"


UCLASS(config=Game)
class AMobaTestCharacter : public ACharacter, public IStrategyTeamInterface
{
	GENERATED_UCLASS_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Status")
	bool IsReady();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities)
	class UCharacterAbilityCasterComponent * AbilitySlot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CombatStats)
	class UMobaCombatCharacterComponent * CombatCharacterComponent;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = GUI)
	struct FDefaultIconStruct Icon;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = GUI)
	FString Name;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly)
	float BaseSpeed;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
	TArray<UTickEffect*> TickEffects;

	UFUNCTION(BlueprintCallable, Category = "Effects")
	TArray<UTickEffect*> GetTickEffects() const;

	UFUNCTION()
	void AddTickEffect(TSubclassOf<class UTickEffect> EffectClass);

	UFUNCTION(BlueprintCallable, Category = "Items")
	UMobaItem* GetItem(EItemSlot Slot) const;

	UFUNCTION(BlueprintCallable, Category = "Items")
	UMobaItem* GetActiveItem(EActiveItemSlot Slot) const;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetModifiedSpeed();

	UFUNCTION(BlueprintCallable, Category = "Stats")
	int GetLevel() const;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	int GetExperience() const;

	UFUNCTION(BlueprintCallable, Category = "Money")
	int GetMoney() const;

	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/* IStrategyTeamInterface
	*/
	UFUNCTION(BlueprintCallable, Category = "Team")
	uint8 GetTeamNum() const override;

	/* End IStrategyTeamInterface
	*/

protected:

	void ApplyItemEffects();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

