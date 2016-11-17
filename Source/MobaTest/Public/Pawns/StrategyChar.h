// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Public/StrategyTypes.h"
#include "Public/Interfaces/StrategyTeamInterface.h"
#include "MobaCombatCharacterComponent.h"
#include "StrategyChar.generated.h"

//General Log
DECLARE_LOG_CATEGORY_EXTERN(LogStrategyChar, Log, All);

class UStrategyAttachment;

// Base class for the minions
UCLASS(Abstract)
class AStrategyChar : public ACharacter, public IStrategyTeamInterface
{
	GENERATED_UCLASS_BODY()

	/** Identifies if pawn is in its dying state */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Health)
	uint32 bIsDying:1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CombatComponent)
	UMobaCombatCharacterComponent * CombatComponent;

public:

	/**
	 * Kills pawn.
	 * @param KillingDamage - Damage amount of the killing blow
	 * @param DamageEvent - Damage event of the killing blow
	 * @param Killer - Who killed this pawn
	 * @param DamageCauser - the Actor that directly caused the damage (i.e. the Projectile that exploded, the Weapon that fired, etc)
	 * @returns true if allowed
	 */
	virtual void Die(float KillingDamage, struct FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser);

	// Begin Actor interface
	/** initial setup */
	virtual void PostInitializeComponents() override;

	/** prevent units from basing on each other or buildings */
	virtual bool CanBeBaseForCharacter(APawn* Pawn) const override;

	/** don't export collisions for navigation */
	virtual bool IsComponentRelevantForNavigation(UActorComponent* Component) const override { return false; }

	/** Take damage, handle death */
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/** pass hit notifies to AI */
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalForce, const FHitResult& Hit) override;
	// End Actor interface


	// Begin StrategyTeamInterface interface
	UFUNCTION(BlueprintCallable, Category = "Team")
	virtual uint8 GetTeamNum() const override;
	// End StrategyTeamInterface interface


	/** 
	 * Starts melee attack. 
	 * @return Duration of the attack anim.
	 */
	float PlayMeleeAnim();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MULTICAST_PlayMeleeAnim();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MULTICAST_PlayDeathAnim();

	/** Notification triggered from the melee animation to signal impact. */
	void OnMeleeImpactNotify();

	/** set attachment for weapon slot */
	UFUNCTION(BlueprintCallable, Category=Attachment)
	void SetWeaponAttachment(UStrategyAttachment* Weapon);

	UFUNCTION(BlueprintCallable, Category=Attachment)
	bool IsWeaponAttached();

	/** set attachment for armor slot */
	UFUNCTION(BlueprintCallable, Category=Attachment)
	void SetArmorAttachment(UStrategyAttachment* Armor);

	UFUNCTION(BlueprintCallable, Category=Attachment)
	bool IsArmorAttached();

	/** set team number */
	void SetTeamNum(uint8 NewTeamNum); 

	/** get current pawn's data */
	const struct FPawnData* GetPawnData() const;

	ELaneEnum LanePosition;

protected:
	/** melee anim */
	UPROPERTY(Replicated,EditDefaultsOnly, Category=Pawn)
	UAnimMontage* MeleeAnim;

	/** death anim */
	UPROPERTY(EditDefaultsOnly, Category=Pawn)
	UAnimMontage* DeathAnim;
	
	/** Armor attachment slot */
	UPROPERTY()
	UStrategyAttachment* ArmorSlot;

	/** Weapon attachment slot */
	UPROPERTY()
	UStrategyAttachment* WeaponSlot;

	/** team number */
	UPROPERTY(Replicated)
	uint8 MyTeamNum;

	/** base pawn data */
	UPROPERTY(EditDefaultsOnly, Category=Pawn)
	FPawnData PawnData;

	/** event called after die animation  to hide character and delete it asap */
	void OnDieAnimationEnd();

private:

	/** Handle for efficient management of UpdatePawnData timer */
	FTimerHandle TimerHandle_UpdatePawnData;
};

