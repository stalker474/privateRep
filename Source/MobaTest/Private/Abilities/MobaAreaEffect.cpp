// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "MobaAreaEffect.h"

DEFINE_LOG_CATEGORY(AreaEffectLog);

// Sets default values
AMobaAreaEffect::AMobaAreaEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
	RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("DefaultRoot"));
	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);

	ParticleComp = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("ParticleComp"));
	ParticleComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ParticleComp->bAutoActivate = true;
	ParticleComp->bAutoDestroy = true;

	bReplicates = true;
	bReplicateMovement = true;
	this->InitialLifeSpan = 2.0f;

}

void AMobaAreaEffect::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	MyController = GetInstigatorController();
	this->OnActorBeginOverlap.AddDynamic(this, &AMobaAreaEffect::OnImpact);
	//this->OnActorHit.AddDynamic(this,&AMobaAreaEffect::OnImpact);
}

void AMobaAreaEffect::OnImpact(AActor* self, AActor* victim)
{
	if (Role == ROLE_Authority)
	{
		FDamageEvent customEvent;
		customEvent.DamageTypeClass = WeaponConfig.DamageType;
		
		if(!AStrategyGameMode::OnFriendlyTeam(GetOwner(), victim))
			victim->TakeDamage(WeaponConfig.Damage, customEvent, self->GetInstigatorController(), this);
	}
}

void AMobaAreaEffect::DisableAndDestroy()
{
	UAudioComponent* ProjAudioComp = FindComponentByClass<UAudioComponent>();
	if (ProjAudioComp && ProjAudioComp->IsPlaying())
	{
		ProjAudioComp->FadeOut(0.1f, 0.f);
	}

	// give clients some time to show explosion
	SetLifeSpan(2.0f);
}


