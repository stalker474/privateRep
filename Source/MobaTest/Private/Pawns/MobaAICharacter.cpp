// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "MobaAICharacter.h"
#include "MobaPlayerController.h"
#include "BaseAIController.h"

DEFINE_LOG_CATEGORY(LogMobaAIChar);

AMobaAICharacter::AMobaAICharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->UpdatedComponent = GetCapsuleComponent();
		GetCharacterMovement()->GetNavAgentPropertiesRef().bCanJump = false;
		GetCharacterMovement()->GetNavAgentPropertiesRef().bCanWalk = true;
		GetCharacterMovement()->SetJumpAllowed(false);
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->bAlwaysCheckFloor = false;
	}

	AIControllerClass = AMobaAIController::StaticClass();

	CombatComponent = CreateDefaultSubobject<UMobaCombatCharacterComponent>(TEXT("Combat component"));
}

void AMobaAICharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMobaAICharacter, MeleeAnim);
	DOREPLIFETIME(AMobaAICharacter, DeathAnim);
}

// Called when the game starts or when spawned
void AMobaAICharacter::BeginPlay()
{
	Super::BeginPlay();
	if (this->Role == ROLE_Authority)
	{
		GetCharacterMovement()->MaxWalkSpeed = GetPawnData()->Speed;
	}
}

// Called every frame
void AMobaAICharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void AMobaAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMobaAICharacter::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalForce, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalForce, Hit);

	if (bSelfMoved)
	{
		// if we controlled by an AI, let it know we ran into something
		ABaseAIController* const AI = Cast<ABaseAIController>(Controller);
		if (AI)
		{
			AI->NotifyBump(Hit);
		}
	}
}

float AMobaAICharacter::PlayMeleeAnim()
{
	if ((CombatComponent->Health > 0.f) && MeleeAnim)
	{
		MULTICAST_PlayMeleeAnim();
		return PlayAnimMontage(MeleeAnim);
	}

	return 0.f;
}

void AMobaAICharacter::MULTICAST_PlayMeleeAnim_Implementation()
{
	PlayAnimMontage(MeleeAnim);
}

bool AMobaAICharacter::MULTICAST_PlayMeleeAnim_Validate()
{
	return true;
}

void AMobaAICharacter::OnMeleeImpactNotify()
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

	for (int32 i = 0; i<Hits.Num(); i++)
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

float AMobaAICharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (CombatComponent->Health <= 0.f)
	{
		// no further damage if already dead
		return 0.f;
	}
	AMobaPlayerController * ctrl = Cast<AMobaPlayerController>(EventInstigator);
	if (ctrl)
	{
		ABaseAIController * aiCtrl = Cast<ABaseAIController>(Controller);
		aiCtrl->ClaimAsTarget(ctrl);
	}
		
	CombatComponent->ApplyDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	return Damage;
}


void AMobaAICharacter::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser)
{
	if (bIsDying										// already dying
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
	ABaseAIController* const AIController = Cast<ABaseAIController>(Controller);
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
		DeathAnimDuration = PlayAnimMontage(DeathAnim) / (GetMesh() && GetMesh()->GlobalAnimRateScale > 0 ? GetMesh()->GlobalAnimRateScale : 1);
		UAnimInstance * AnimInstance = (GetMesh()) ? GetMesh()->GetAnimInstance() : nullptr;
	}

	// Use a local timer handle as we don't need to store it for later but we don't need to look for something to clear
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AMobaAICharacter::OnDieAnimationEnd, DeathAnimDuration + 0.01, false);
}

void AMobaAICharacter::MULTICAST_PlayDeathAnim_Implementation()
{
	PlayAnimMontage(DeathAnim);
}

bool AMobaAICharacter::MULTICAST_PlayDeathAnim_Validate()
{
	return true;
}

void AMobaAICharacter::OnDieAnimationEnd()
{
	this->SetActorHiddenInGame(true);
	// delete the pawn asap
	SetLifeSpan(0.01f);
}

const struct FPawnData* AMobaAICharacter::GetPawnData() const
{
	return &PawnData;
}
