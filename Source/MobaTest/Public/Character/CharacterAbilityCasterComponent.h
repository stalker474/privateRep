// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "Ability.h"
#include "CharacterAbilityCasterComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MOBATEST_API UCharacterAbilityCasterComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:	

	void SetupPlayerInputComponent(class UInputComponent* InputComponent);

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	/** Launch an ability */
	void LaunchDefaultAbility();

	/** Launch an ability */
	void LaunchAbility1();

	/** Launch an ability */
	void LaunchAbility2();

	/** Launch an ability */
	void LaunchAbility3();

	/** Launch an ability */
	void LaunchAbility4();

	/** cancel an ability */
	void CancelAbility();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerLaunchAbility(const int AbilityIndex,const FAbilityClientData clientData);

	UFUNCTION(Reliable, NetMulticast)
	void MULTICAST_LaunchAbility(const int AbilityIndex);

	UFUNCTION(BlueprintCallable, Category = Stats)
	float GetModifiedAttackSpeed();

	UFUNCTION(BlueprintCallable, Category = Stats)
	float GetModifiedMana();

	UFUNCTION(BlueprintCallable, Category = Stats)
	float GetModifiedManaRegen();

	UFUNCTION(BlueprintCallable, Category = Stats)
	float GetModifiedPower();

	UFUNCTION(BlueprintCallable, Category = Stats)
	float GetModifiedStrength();

	FAbilityClientData ClientLaunchAbility(const int AbilityIndex);

	UAbility * GetAbilityByIndex(const int Index);

	UAnimMontage * GetAnimationByIndex(const int Index);

protected:

	//void PostInitProperties() override;

	void InitializeComponent() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
	TSubclassOf<class UAbility> DefaultAbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
	TSubclassOf<class UAbility> Ability1Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
	TSubclassOf<class UAbility> Ability2Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
	TSubclassOf<class UAbility> Ability3Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
	TSubclassOf<class UAbility> Ability4Class;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = AbilityInstances)
	class UAbility * Ability1;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = AbilityInstances)
	class UAbility * Ability2;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = AbilityInstances)
	class UAbility * Ability3;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = AbilityInstances)
	class UAbility * Ability4;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = AbilityInstances)
	class UAbility * DefaultAbility;

	/** melee anim */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* DefaultAttackAnimation;

	/** casting anim */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* Ability1Animation;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* Ability2Animation;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* Ability3Animation;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* Ability4Animation;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
	float BaseMana;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
	float BaseManaRegen;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float Mana;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
	float ManaLevelBonus;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
	float ManaRegenLevelBonus;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
	float BasePower;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
	float BaseStrength;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float BaseAttackSpeed;

	void LaunchAbility(const int AbilityIndex);

private:

	class UInputComponent * InputComponent;
};
