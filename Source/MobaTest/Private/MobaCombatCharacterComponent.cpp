// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "MobaAreaEffect.h"
#include "MobaCombatCharacterComponent.h"
#include "StrategyChar.h"
#include "MobaTestCharacter.h"

// Sets default values for this component's properties
UMobaCombatCharacterComponent::UMobaCombatCharacterComponent(const FObjectInitializer& ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
	bReplicates = true;
	HealthLevelBonus = 1.0f;
	HealthRegenLevelBonus = 1.0f;
}

// Called when the game starts
void UMobaCombatCharacterComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UMobaCombatCharacterComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetOwnerRole() == ROLE_Authority)
	{
		Health = BaseHealth;
	}
}

void UMobaCombatCharacterComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMobaCombatCharacterComponent, BaseHealth);
	DOREPLIFETIME(UMobaCombatCharacterComponent, BaseHealthRegen);
	DOREPLIFETIME(UMobaCombatCharacterComponent, Health);
	DOREPLIFETIME(UMobaCombatCharacterComponent, HealthLevelBonus);
	DOREPLIFETIME(UMobaCombatCharacterComponent, HealthRegenLevelBonus);
	DOREPLIFETIME(UMobaCombatCharacterComponent, BaseMagicDefense);
	DOREPLIFETIME(UMobaCombatCharacterComponent, BasePhysicalDefense);
}


// Called every frame
void UMobaCombatCharacterComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (GetOwnerRole() == ROLE_Authority)
	{
		const AMobaTestCharacter * character = Cast<AMobaTestCharacter>(GetOwner());
		if (character)
		{
			TArray<UTickEffect*> effects = character->GetTickEffects();
			if (Health < GetModifiedHealth())
			{
				Health += GetModifiedHealthRegen() * DeltaTime;
				if (Health > GetModifiedHealth())
				{
					Health = GetModifiedHealth();
				}
			}
			
			for (UTickEffect* effect : effects)
				effect->Apply(DeltaTime);
		}
	}
}

float UMobaCombatCharacterComponent::GetModifiedDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	const AMobaTestCharacter * character = Cast<AMobaTestCharacter>(GetOwner());
	if (character)
	{
		TArray<UTickEffect*> effects = character->GetTickEffects();
		for (UTickEffect * effect : effects)
		{
			if (effect->GetIsActive())
			{
				if (effect->Invulnerable)
					return 0;
				else
				{

				}
			}
		}
	}
	return DamageAmount;
}

float UMobaCombatCharacterComponent::ApplyDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		Health -= GetModifiedDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		if (Health <= 0)
		{
			AStrategyChar * myOwner = Cast<AStrategyChar>(GetOwner());
			if (myOwner)
				myOwner->Die(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		}

		AMobaAreaEffect* effect = Cast<AMobaAreaEffect>(DamageCauser);
		if (effect != nullptr) //if it's an area effect
		{
			for (TSubclassOf<UTickEffect>& effect : effect->WeaponConfig.Effects)
			{
				AMobaTestCharacter * character = Cast<AMobaTestCharacter>(GetOwner());
				
				if (character)
					character->AddTickEffect(effect);
			}


			//GetWorld()->GetTimerManager().SetTimer()
		}
		return DamageAmount;
	}
	else
		return 0;
}
float UMobaCombatCharacterComponent::GetModifiedHealth()
{
	int Level = 1;
	const AMobaTestCharacter * character = Cast<AMobaTestCharacter>(GetOwner());
	AStrategyChar * holop = Cast<AStrategyChar>(GetOwner());
	if (character)
		Level = character->GetLevel();
	return  Level == 1 ? BaseHealth : BaseHealth * (HealthLevelBonus * Level);
}

float UMobaCombatCharacterComponent::GetModifiedHealthRegen()
{
	int Level = 1;
	const AMobaTestCharacter * character = Cast<AMobaTestCharacter>(GetOwner());
	AStrategyChar * holop = Cast<AStrategyChar>(GetOwner());
	if (character)
		Level = character->GetLevel();
	return  Level == 1 ? BaseHealthRegen : BaseHealthRegen * (HealthRegenLevelBonus * Level);
}

float UMobaCombatCharacterComponent::GetModifiedMagicDefense()
{
	return BaseMagicDefense;
}

float UMobaCombatCharacterComponent::GetModifiedPhysicalDefense()
{
	return BasePhysicalDefense;
}

