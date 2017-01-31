// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MobaCombatCharacterComponent.h"
#include "GameFramework/Character.h"
#include "MobaAICharacter.generated.h"


//General Log
DECLARE_LOG_CATEGORY_EXTERN(LogMobaAIChar, Log, All);

UCLASS(Abstract)
class MOBATEST_API AMobaAICharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

	/** Identifies if pawn is in its dying state */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health)
	uint32 bIsDying : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CombatComponent)
	UMobaCombatCharacterComponent * CombatComponent;

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnClicked(UPrimitiveComponent* Component, FKey Key);

	/**
	* Kills pawn.
	* @param KillingDamage - Damage amount of the killing blow
	* @param DamageEvent - Damage event of the killing blow
	* @param Killer - Who killed this pawn
	* @param DamageCauser - the Actor that directly caused the damage (i.e. the Projectile that exploded, the Weapon that fired, etc)
	* @returns true if allowed
	*/
	virtual void Die(float KillingDamage, struct FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser);

	/** Take damage, handle death */
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/** pass hit notifies to AI */
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalForce, const FHitResult& Hit) override;
	// End Actor interface

	/**
	* Starts melee attack.
	* @return Duration of the attack anim.
	*/
	float PlayMeleeAnim();

	/** Notification triggered from the melee animation to signal impact. */
	void OnMeleeImpactNotify();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MULTICAST_PlayMeleeAnim();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MULTICAST_PlayDeathAnim();

	const struct FPawnData* GetPawnData() const;

protected:
	/** melee anim */
	UPROPERTY(Replicated, EditDefaultsOnly, Category = Pawn)
	UAnimMontage* MeleeAnim;

	/** death anim */
	UPROPERTY(Replicated, EditDefaultsOnly, Category = Pawn)
	UAnimMontage* DeathAnim;

	/** base pawn data */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	FPawnData PawnData;

	/** event called after die animation  to hide character and delete it asap */
	void OnDieAnimationEnd();
	
};
