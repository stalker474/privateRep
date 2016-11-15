// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "TickEffect.h"
#include "MobaCombatCharacterComponent.h"

DEFINE_LOG_CATEGORY(TickEffectLog);

UTickEffect::UTickEffect(const FObjectInitializer& ObjectInitializer)
{
	Duration = 0.0f;
	ApplyEveryDt = 0.0f;
	DamageValue = 0.0f;
	SpeedFactor = 1.0f;
	HealthFactor = 1.0f;
	ManaFactor = 1.0f;
	WeaponDamageFactor = 1.0f;
	MagicArmorFactor = 1.0f;
	PhysicalArmorFactor = 1.0f;
	DisablePrimaryAbility = false;
	DisableCastingAbilities = false;
	Particles = nullptr;

	TotalElapsedTime = 0.0f;
	TimeSinceLastTick = 0.0f;
	IsOver = false;
	DamageType = nullptr;
	AppliedOnce = false;
}

void UTickEffect::Apply(float dT)
{
	TotalElapsedTime += dT;
	TimeSinceLastTick += dT;
	if (TotalElapsedTime > Duration)
	{
		IsOver = true;
		//restore values
		ACharacter * character = Cast<ACharacter>(Owner.Get()->GetOwner());
		if(SpeedFactor != 1.0f)
			character->GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
	}
		
	else
	{
		if (Owner.IsValid())
		{
			UMobaCombatCharacterComponent * myOwner = Cast<UMobaCombatCharacterComponent>(Owner.Get());
			ACharacter * character = Cast<ACharacter>(Owner.Get()->GetOwner());
			if (!AppliedOnce && Particles)
			{
				UParticleSystemComponent * particleSystem = NewObject<UParticleSystemComponent>(this);
				particleSystem->SetActive(true);
				particleSystem->ActivateSystem();
				particleSystem->RegisterComponent();
				//particleSystem->AttachTo(character);
				particleSystem->Template = Particles;
				character->AddInstanceComponent(particleSystem);
			}
			if (myOwner)
			{
				const FDamageEvent damageEvent(DamageType);
				if (ApplyEveryDt == 0.0f && !AppliedOnce)
				{
					myOwner->ApplyDamage(DamageValue, damageEvent, nullptr, Owner.Get()->GetOwner());
					
					if (SpeedFactor != 1.0f)
					{
						BaseSpeed = character->GetCharacterMovement()->MaxWalkSpeed;
						character->GetCharacterMovement()->MaxWalkSpeed *= SpeedFactor;
					}
					
					AppliedOnce = true;
				}
				else
				{
					if (ApplyEveryDt <= TimeSinceLastTick) //time to tick
					{
						myOwner->ApplyDamage(DamageValue, damageEvent, nullptr, Owner.Get()->GetOwner());
						TimeSinceLastTick = 0.0f;
						AppliedOnce = true;
					}
					
				}
				
			}
			else
				UE_LOG(TickEffectLog, Warning, TEXT("Tick effect has no valid owner!"));
		}
		else
			UE_LOG(TickEffectLog, Warning, TEXT("Tick effect has no valid owner!"));
	}
}


