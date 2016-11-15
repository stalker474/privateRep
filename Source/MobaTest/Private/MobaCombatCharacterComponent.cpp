// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "MobaAreaEffect.h"
#include "MobaCombatCharacterComponent.h"
#include "StrategyChar.h"

// Sets default values for this component's properties
UMobaCombatCharacterComponent::UMobaCombatCharacterComponent(const FObjectInitializer& ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts
void UMobaCombatCharacterComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UMobaCombatCharacterComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMobaCombatCharacterComponent, MaxHealth);
	DOREPLIFETIME(UMobaCombatCharacterComponent, Health);
}


// Called every frame
void UMobaCombatCharacterComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	for (UTickEffect* effect : TickEffects)
		effect->Apply(DeltaTime);

	TickEffects.RemoveAll([](UTickEffect* effect) { return !effect->GetIsActive();});
}

float UMobaCombatCharacterComponent::ApplyDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Health -= DamageAmount;
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
			UTickEffect * newEffect = NewObject<UTickEffect>(this, effect);
			newEffect->SetOwner(this);
			TickEffects.Add(newEffect);
		}
		
	
		//GetWorld()->GetTimerManager().SetTimer()
	}
	return DamageAmount;
}

