// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "StaticWeapon.h"

#include "FakeProjectile.h"


// Sets default values
UStaticWeaponComponent::UStaticWeaponComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	Target(nullptr),
	IsAiming(false),
	TimeSinceLastShot(0),
	ReloadTime(1.0f)
{
	bAutoActivate = true;
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	//MeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this,TEXT("StaticMeshComponent"));
	//MeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void UStaticWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	DefaultRotation = GetComponentRotation();
	
}

void UStaticWeaponComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UStaticWeaponComponent, Target);
	DOREPLIFETIME(UStaticWeaponComponent, IsAiming);
}

// Called every frame
void UStaticWeaponComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction);

	TimeSinceLastShot += DeltaTime;

	if (GetOwnerRole() != ROLE_Authority && IsAiming) //only server can shoot
	{
		FVector targetLocation = Target.Get()->GetActorLocation();
		FVector myLocation = GetComponentLocation();
		FVector aimingVector = targetLocation - myLocation;
		aimingVector.Normalize();
		FRotator newRotation;
		newRotation = aimingVector.ToOrientationRotator();
		newRotation.Roll = -newRotation.Pitch;
		newRotation.Pitch = 0;
		SetRelativeRotation(DefaultRotation + newRotation);
	}

	if (IsAiming && Target.IsValid())
	{
		if (GetOwnerRole() == ROLE_Authority) //only server can shoot
		{
			if (TimeSinceLastShot > ReloadTime)
			{
				Shoot();
				TimeSinceLastShot = 0;
			}
		}
	}
}

void UStaticWeaponComponent::LockAndLoad()
{
	IsAiming = true;
}

void UStaticWeaponComponent::Abort()
{
	IsAiming = false;
}

void UStaticWeaponComponent::Shoot()
{
	FActorSpawnParameters params;
	params.Owner = GetOwner();
	FVector location;
	FRotator rotation;
	location = GetComponentLocation();
	rotation = GetComponentRotation();
	TWeakObjectPtr<AFakeProjectile> projectile = GetWorld()->SpawnActor<AFakeProjectile>(ProjectileClass,location,rotation,params);
	if (projectile.IsValid() && Target.IsValid())
	{
		projectile.Get()->Target = Target.Get();
	}
}

