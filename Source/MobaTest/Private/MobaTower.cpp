// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "TowerAIController.h"
#include "MobaTower.h"
#include "MobaAreaEffect.h"


AMobaTower::AMobaTower(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	MeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	DecalComponent->SetIsReplicated(true);

	DecalComponent->SetRelativeRotation(FRotator(90, 0, 0));
	DecalComponent->DecalSize.Y = 1;
	DecalComponent->DecalSize.Z = 1;
	DecalComponent->DecalSize.X = 1;
	
	Weapon = CreateDefaultSubobject<UStaticWeaponComponent>(TEXT("StaticWeapon"));
	Weapon->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	AIControllerClass = ATowerAIController::StaticClass();

	CombatComponent = CreateDefaultSubobject<UMobaCombatCharacterComponent>(TEXT("CombatComponent"));
}

void AMobaTower::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ATowerAIController * myController = Cast<ATowerAIController>(Controller);
	if (myController)
	{
		float sightRadius = myController->GetSensingComponent()->SightRadius;
		//DecalComponent->RelativeScale3D = ;
		DecalComponent->SetWorldScale3D(FVector(50, sightRadius, sightRadius));
	}
}

float AMobaTower::Attack(TWeakObjectPtr<APawn> Target)
{
	Weapon->SetTarget(Target);
	Weapon->LockAndLoad();
	return ReloadTime;
}

void AMobaTower::Abort()
{
	Weapon->Abort();
}

float AMobaTower::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	//ignore area damage
	AMobaAreaEffect * effect = Cast<AMobaAreaEffect>(DamageCauser);
	if (effect)
		return 0.0f;
	CombatComponent->ApplyDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (CombatComponent->Health <= 0)
	{
		Active = false;
		Weapon->Activate(false);
		CombatComponent->Health = 0;
		DecalComponent->SetVisibility(false);
		Abort();
		Deactivate();
	}
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
