// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MobaTest.h"
#include "MobaTestCharacter.h"
#include "MobaPlayerState.h"

//////////////////////////////////////////////////////////////////////////
// AMobaTestCharacter

AMobaTestCharacter::AMobaTestCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bReplicates = true;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bDoCollisionTest = true;
	CameraBoom->ProbeChannel = ECC_WorldStatic;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	AbilitySlot = CreateDefaultSubobject<UCharacterAbilityCasterComponent>(TEXT("Ability slot"));

	CombatCharacterComponent = CreateDefaultSubobject<UMobaCombatCharacterComponent>(TEXT("Combat component"));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}


void AMobaTestCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMobaTestCharacter, Icon);
	DOREPLIFETIME(AMobaTestCharacter, BaseSpeed);
	DOREPLIFETIME(AMobaTestCharacter, TickEffects);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMobaTestCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);

	AbilitySlot->SetupPlayerInputComponent(InputComponent);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AMobaTestCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AMobaTestCharacter::LookUpAtRate);
	InputComponent->BindAxis("MoveForward", this, &AMobaTestCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMobaTestCharacter::MoveRight);
}

void AMobaTestCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMobaTestCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AMobaTestCharacter::IsReady()
{
	AMobaPlayerState * ps = Cast<AMobaPlayerState>(PlayerState);
	return ps != nullptr;
}

// Called every frame
void AMobaTestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Role == ROLE_Authority)
	{
		TickEffects.RemoveAll([](UTickEffect* effect) { return !effect->GetIsActive();});
		ApplyItemEffects();
		if(GetCharacterMovement()->MaxWalkSpeed != GetModifiedSpeed())
			GetCharacterMovement()->MaxWalkSpeed = GetModifiedSpeed();
	}
}

void AMobaTestCharacter::ApplyItemEffects()
{
	UMobaItem * item1 = GetItem(EItemSlot::SLOT_1);
	UMobaItem * item2 = GetItem(EItemSlot::SLOT_2);
	UMobaItem * item3 = GetItem(EItemSlot::SLOT_3);
	UMobaItem * item4 = GetItem(EItemSlot::SLOT_4);
	UMobaItem * item5 = GetItem(EItemSlot::SLOT_5);
	UMobaItem * item6 = GetItem(EItemSlot::SLOT_6);

	if (item1)
		item1->Apply(TickEffects);
	if (item2)
		item2->Apply(TickEffects);
	if (item3)
		item3->Apply(TickEffects);
	if (item4)
		item4->Apply(TickEffects);
	if (item5)
		item5->Apply(TickEffects);
	if (item6)
		item6->Apply(TickEffects);
}

TArray<UTickEffect*> AMobaTestCharacter::GetTickEffects() const
{
	return TickEffects;
}

void AMobaTestCharacter::AddTickEffect(TSubclassOf<class UTickEffect> EffectClass)
{
	UTickEffect * effect = NewObject<UTickEffect>(this,EffectClass);
	effect->SetOwner(CombatCharacterComponent);
	TickEffects.Add(effect);
}

UMobaItem* AMobaTestCharacter::GetItem(EItemSlot Slot) const
{
	const AMobaPlayerState * CurrentState = Cast<AMobaPlayerState>(PlayerState);
	if (CurrentState)
		return CurrentState->GetItem(Slot);
	else
		return nullptr;
}

float AMobaTestCharacter::GetModifiedSpeed()
{
	return BaseSpeed;
}

int AMobaTestCharacter::GetLevel() const
{
	const AMobaPlayerState * CurrentState = Cast<AMobaPlayerState>(PlayerState);
	if (CurrentState)
		return CurrentState->GetLevel();
	else
		return 0;
}

int AMobaTestCharacter::GetExperience() const
{
	const AMobaPlayerState * CurrentState = Cast<AMobaPlayerState>(PlayerState);
	if (CurrentState)
		return CurrentState->GetExperience();
	else
		return 0;
}

int AMobaTestCharacter::GetMoney() const
{
	const AMobaPlayerState * CurrentState = Cast<AMobaPlayerState>(PlayerState);
	if (CurrentState)
		return CurrentState->GetMoney();
	else
		return 0;
}

float AMobaTestCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	return CombatCharacterComponent->ApplyDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

uint8 AMobaTestCharacter::GetTeamNum() const
{
	const AMobaPlayerState * CurrentState = Cast<AMobaPlayerState>(PlayerState);
	if (CurrentState)
		return CurrentState->GetTeam();
	else
		return 0;
}

void AMobaTestCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, Value);
	}
}

void AMobaTestCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		Value /= 2.0f;
			
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
