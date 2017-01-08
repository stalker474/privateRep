// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MobaTest.h"
#include "StrategyAIController.h"
#include "StrategyAttachment.h"

#include "VisualLogger/VisualLogger.h"

DEFINE_LOG_CATEGORY(LogStrategyChar);

AStrategyChar::AStrategyChar(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent>(ACharacter::CharacterMovementComponentName)) 
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	// no collisions in mesh
	//GetMesh()->BodyInstance.SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->UpdatedComponent = GetCapsuleComponent();
		GetCharacterMovement()->GetNavAgentPropertiesRef().bCanJump = false;
		GetCharacterMovement()->GetNavAgentPropertiesRef().bCanWalk = true;
		GetCharacterMovement()->SetJumpAllowed(false);
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->bAlwaysCheckFloor = false;
	}

	AIControllerClass = AStrategyAIController::StaticClass();

	CombatComponent = CreateDefaultSubobject<UMobaCombatCharacterComponent>(TEXT("Combat component"));
}

void AStrategyChar::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

bool AStrategyChar::CanBeBaseForCharacter(APawn* Pawn) const
{
	return false;
}

void AStrategyChar::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AStrategyChar, MeleeAnim);
	DOREPLIFETIME(AStrategyChar, DeathAnim);
	DOREPLIFETIME(AStrategyChar, MyTeamNum);
}

void AStrategyChar::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalForce, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalForce, Hit);

	if (bSelfMoved)
	{
		// if we controlled by an AI, let it know we ran into something
		AStrategyAIController* const AI = Cast<AStrategyAIController>(Controller);
		if (AI)
		{
			AI->NotifyBump(Hit);
		}
	}
}

float AStrategyChar::PlayMeleeAnim()
{
	if ( (CombatComponent->Health > 0.f) && MeleeAnim )
	{
		MULTICAST_PlayMeleeAnim();
		return PlayAnimMontage(MeleeAnim);
	}

	return 0.f;
}

void AStrategyChar::MULTICAST_PlayMeleeAnim_Implementation()
{
	PlayAnimMontage(MeleeAnim);
}

bool AStrategyChar::MULTICAST_PlayMeleeAnim_Validate()
{
	return true;
}

void AStrategyChar::OnMeleeImpactNotify()
{
	const int32 MeleeDamage = FMath::RandRange(PawnData.AttackMin, PawnData.AttackMax);
	const TSubclassOf<UDamageType> MeleeDmgType = UDamageType::StaticClass();

	// Do a trace to see what we hit
	const float CollisionRadius = GetCapsuleComponent() ? GetCapsuleComponent()->GetScaledCapsuleRadius() : 0.f;
	const float TraceDistance = CollisionRadius + (PawnData.AttackDistance * 1.3f);
	const FVector TraceStart = GetActorLocation();
	const FVector TraceDir = GetActorForwardVector();
	const FVector TraceEnd = TraceStart + TraceDir * TraceDistance;
	TArray<FHitResult> Hits;
	FCollisionQueryParams TraceParams(TEXT("MeleeHit"), false, this);
	TraceParams.bTraceAsyncScene = true;			// also trace against the async/cosmetic scene
	FCollisionResponseParams ResponseParam(ECollisionResponse::ECR_Overlap);
	GetWorld()->SweepMultiByChannel(Hits, TraceStart, TraceEnd, FQuat::Identity, COLLISION_WEAPON, FCollisionShape::MakeBox(FVector(80.f)), TraceParams, ResponseParam);

	for (int32 i=0; i<Hits.Num(); i++)
	{
		FHitResult const& Hit = Hits[i];
		if (AStrategyGameMode::OnEnemyTeam(this, Hit.GetActor()))
		{
			UGameplayStatics::ApplyPointDamage(Hit.GetActor(), MeleeDamage, TraceDir, Hit, Controller, this, MeleeDmgType);

			// only damage first hit
			break;
		}
	}
}

float AStrategyChar::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (CombatComponent->Health <= 0.f)
	{
		// no further damage if already dead
		return 0.f;
	}
	CombatComponent->ApplyDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	
	// Modify based on game rules.
	AStrategyGameMode* const Game = GetWorld()->GetAuthGameMode<AStrategyGameMode>();
	Damage = Game ? Game->ModifyDamage(Damage, this, DamageEvent, EventInstigator, DamageCauser) : 0.f;
	return Damage;
}


void AStrategyChar::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser)
{
	if ( bIsDying										// already dying
		|| IsPendingKill()								// already destroyed
		|| GetWorld()->GetAuthGameMode() == nullptr)	// level transition occurring
	{
		return;
	}

	bIsDying = true;
	//Health = FMath::Min(0.0f, Health);

	// figure out who killed us
	UDamageType const* const DamageType = DamageEvent.DamageTypeClass ? 
		Cast<const UDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject()) 
		: GetDefault<UDamageType>();
	Killer = GetDamageInstigator(Killer, *DamageType);

	// forcibly end any timers that may be in flight
	GetWorldTimerManager().ClearAllTimersForObject(this);

	// disable any AI
	AStrategyAIController* const AIController = Cast<AStrategyAIController>(Controller);
	if (AIController)
	{
		AIController->EnableLogic(false);
	}

	// turn off collision
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->BodyInstance.SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->BodyInstance.SetResponseToChannel(ECC_Pawn, ECR_Ignore);
		GetCapsuleComponent()->BodyInstance.SetResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
	}

	// turn off movement
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();
	}

	// detach the controller
	if (Controller != nullptr)
	{
		Controller->UnPossess();
	}	

	// play death animation
	float DeathAnimDuration = 0.f;
	if (DeathAnim)
	{
		MULTICAST_PlayDeathAnim();
		DeathAnimDuration = PlayAnimMontage(DeathAnim) / ( GetMesh() && GetMesh()->GlobalAnimRateScale > 0 ? GetMesh()->GlobalAnimRateScale : 1);
		UAnimInstance * AnimInstance = (GetMesh())? GetMesh()->GetAnimInstance() : nullptr; 
	}

	// Use a local timer handle as we don't need to store it for later but we don't need to look for something to clear
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AStrategyChar::OnDieAnimationEnd, DeathAnimDuration + 0.01, false);
}

void AStrategyChar::MULTICAST_PlayDeathAnim_Implementation()
{
	PlayAnimMontage(DeathAnim);
}

bool AStrategyChar::MULTICAST_PlayDeathAnim_Validate()
{
	return true;
}

void AStrategyChar::OnDieAnimationEnd()
{
	this->SetActorHiddenInGame(true);
	// delete the pawn asap
	SetLifeSpan( 0.01f );
}

void AStrategyChar::SetWeaponAttachment(UStrategyAttachment* Weapon)
{
	if (WeaponSlot != Weapon)
	{
		// detach any existing weapon attachment
		if (WeaponSlot )
		{
			WeaponSlot->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		}

		// attach this one
		WeaponSlot = Weapon;
		if (WeaponSlot )
		{
			WeaponSlot->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSlot->AttachPoint);
		}
	}
}

void AStrategyChar::SetArmorAttachment(UStrategyAttachment* Armor)
{
	if (ArmorSlot != Armor)
	{
		// detach any existing armor attachment
		if (ArmorSlot )
		{
			ArmorSlot->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		}
		// attach this one
		ArmorSlot = Armor;
		if (ArmorSlot )
		{
			ArmorSlot->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, ArmorSlot->AttachPoint);
		}
	}
}

bool AStrategyChar::IsWeaponAttached()
{
	return WeaponSlot != nullptr;
}

bool AStrategyChar::IsArmorAttached()
{
	return ArmorSlot != nullptr;
}

uint8 AStrategyChar::GetTeamNum() const
{
	return MyTeamNum;
}

void AStrategyChar::SetTeamNum(uint8 NewTeamNum)
{
	MyTeamNum = NewTeamNum;
}

const struct FPawnData* AStrategyChar::GetPawnData() const
{
	return &PawnData;
}

