// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "CharacterAbilityCasterComponent.h"
#include "Engine.h"
#include "MobaCombatCharacterComponent.h"
#include "MobaProjectileAbility.h"
#include "MobaAreaDamageAbility.h"


// Sets default values for this component's properties
UCharacterAbilityCasterComponent::UCharacterAbilityCasterComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
	bReplicates = true;
}

void UCharacterAbilityCasterComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetOwnerRole() == ROLE_Authority)
	{
		if (DefaultAbilityClass)
		{
			DefaultAbility = NewObject<UAbility>(GetOwner(), DefaultAbilityClass);
			DefaultAbility->RegisterComponent();
		}

		if (Ability1Class)
		{
			Ability1 = NewObject<UAbility>(GetOwner(), Ability1Class);
			Ability1->RegisterComponent();
		}

		if (Ability2Class)
		{
			Ability2 = NewObject<UAbility>(GetOwner(), Ability2Class);
			Ability2->RegisterComponent();
		}

		if (Ability3Class)
		{
			Ability3 = NewObject<UAbility>(GetOwner(), Ability3Class);
			Ability3->RegisterComponent();
		}

		if (Ability4Class)
		{
			Ability4 = NewObject<UAbility>(GetOwner(), Ability4Class);
			Ability4->RegisterComponent();
		}
	}
}

void UCharacterAbilityCasterComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCharacterAbilityCasterComponent, MaxMana);
	DOREPLIFETIME(UCharacterAbilityCasterComponent, Mana);
	DOREPLIFETIME(UCharacterAbilityCasterComponent, DefaultAbility);
	DOREPLIFETIME(UCharacterAbilityCasterComponent, Ability1);
	DOREPLIFETIME(UCharacterAbilityCasterComponent, Ability2);
	DOREPLIFETIME(UCharacterAbilityCasterComponent, Ability3);
	DOREPLIFETIME(UCharacterAbilityCasterComponent, Ability4);
}

void UCharacterAbilityCasterComponent::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		this->InputComponent = InputComponent;
		InputComponent->BindAction("Action1", IE_Pressed, this, &UCharacterAbilityCasterComponent::LaunchDefaultAbility);
		InputComponent->BindAction("ActionAbility1", IE_Pressed, this, &UCharacterAbilityCasterComponent::LaunchAbility1);
		InputComponent->BindAction("ActionAbility2", IE_Pressed, this, &UCharacterAbilityCasterComponent::LaunchAbility2);
		InputComponent->BindAction("ActionAbility3", IE_Pressed, this, &UCharacterAbilityCasterComponent::LaunchAbility3);
		InputComponent->BindAction("ActionAbility4", IE_Pressed, this, &UCharacterAbilityCasterComponent::LaunchAbility4);

		InputComponent->BindAction("Cancel", IE_Pressed, this, &UCharacterAbilityCasterComponent::CancelAbility);
	}
}

void UCharacterAbilityCasterComponent::ServerLaunchAbility_Implementation(const int AbilityIndex, const FAbilityClientData clientData)
{
	UAbility * ability = GetAbilityByIndex(AbilityIndex);
	UAnimMontage * montage = GetAnimationByIndex(AbilityIndex);
	ability->UpdateClientData(clientData);

	if (GetOwnerRole() == ROLE_Authority)
	{
		Mana -= ability->ManaCost;
		ability->StartCast(0.1f); // no need to wait right now
		MULTICAST_LaunchAbility(AbilityIndex);
	}
}

bool UCharacterAbilityCasterComponent::ServerLaunchAbility_Validate(const int AbilityIndex, const FAbilityClientData clientData)
{
	return true;
}

void UCharacterAbilityCasterComponent::MULTICAST_LaunchAbility_Implementation(const int AbilityIndex)
{
	UAnimMontage * montage = GetAnimationByIndex(AbilityIndex);
	ACharacter * myChar = Cast<ACharacter>(GetOwner());
	myChar->PlayAnimMontage(montage);
}

FAbilityClientData UCharacterAbilityCasterComponent::ClientLaunchAbility(const int AbilityIndex)
{
	FAbilityClientData data;
	data.CamFrwd = FRotator::ZeroRotator;
	data.AimingPosition = FVector::ZeroVector;
	if (GetOwnerRole() != ROLE_Authority)
	{
		UAbility * ability = GetAbilityByIndex(AbilityIndex);
		UAnimMontage * montage = GetAnimationByIndex(AbilityIndex);

		APlayerController * controller = UGameplayStatics::GetPlayerController(this, 0);

		FVector  camLocation;
		FRotator camRotation;

		controller->PlayerCameraManager->GetCameraViewPoint(camLocation, camRotation);
		data.CamFrwd = camRotation;

		TWeakObjectPtr<UMobaAreaDamageAbility> areaAbility = Cast<UMobaAreaDamageAbility>(ability);
		if (areaAbility.IsValid())
		{
			data.AimingPosition = areaAbility.Get()->TargetActor->GetActorLocation();
			data.AimingRotation = areaAbility.Get()->TargetActor->GetActorRotation();
		}
	}
	return data;
}

void UCharacterAbilityCasterComponent::LaunchDefaultAbility()
{
	//if an ability is already ready aiming, cast it
	if (GetOwnerRole() != ROLE_Authority)
	{
		if (Ability1 && Ability1->State == EAbilityStateEnum::AIMING)
			LaunchAbility1();
		else if (Ability2 && Ability2->State == EAbilityStateEnum::AIMING)
			LaunchAbility2();
		else if (Ability3 && Ability3->State == EAbilityStateEnum::AIMING)
			LaunchAbility3();
		else if (Ability4 && Ability4->State == EAbilityStateEnum::AIMING)
			LaunchAbility4();
		else if (DefaultAbility != nullptr && DefaultAbility->GetTimeUntilReady() == 0.0f)
		{
			FAbilityClientData clientData = ClientLaunchAbility(0);
			ServerLaunchAbility(0, clientData);
		}
	}
}

void UCharacterAbilityCasterComponent::LaunchAbility(const int AbilityIndex)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		UAbility * ability = GetAbilityByIndex(AbilityIndex);
		if (ability != nullptr && ability->GetTimeUntilReady() == 0.0f && ability->ManaCost <= Mana)
		{
			if (ability->State == EAbilityStateEnum::NONE)
				ability->Aim();
			else if (ability->State == EAbilityStateEnum::AIMING)
			{
				FAbilityClientData clientData = ClientLaunchAbility(AbilityIndex);
				ServerLaunchAbility(AbilityIndex, clientData);
				CancelAbility();
			}
		}
	}
}

void UCharacterAbilityCasterComponent::LaunchAbility1()
{	
	if (GetOwnerRole() != ROLE_Authority)
		LaunchAbility(1);
}

void UCharacterAbilityCasterComponent::LaunchAbility2()
{
	if (GetOwnerRole() != ROLE_Authority)
		LaunchAbility(2);
}

void UCharacterAbilityCasterComponent::LaunchAbility3()
{
	if (GetOwnerRole() != ROLE_Authority)
		LaunchAbility(3);
}

void UCharacterAbilityCasterComponent::LaunchAbility4()
{
	if (GetOwnerRole() != ROLE_Authority)
		LaunchAbility(4);
}

void UCharacterAbilityCasterComponent::CancelAbility()
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		if (Ability1 && Ability1->State == EAbilityStateEnum::AIMING)
			Ability1->Cancel();
		else if (Ability2 && Ability2->State == EAbilityStateEnum::AIMING)
			Ability2->Cancel();
		else if (Ability3 && Ability3->State == EAbilityStateEnum::AIMING)
			Ability3->Cancel();
		else if (Ability4 && Ability4->State == EAbilityStateEnum::AIMING)
			Ability4->Cancel();
	}
}

UAbility * UCharacterAbilityCasterComponent::GetAbilityByIndex(const int Index)
{
	switch (Index)
	{
	case 0:
		return DefaultAbility;
	case 1:
		return Ability1;
	case 2:
		return Ability2;
	case 3:
		return Ability3;
	case 4:
		return Ability4;
	}
	return nullptr;
}

UAnimMontage * UCharacterAbilityCasterComponent::GetAnimationByIndex(const int Index)
{
	switch (Index)
	{
	case 0:
		return DefaultAttackAnimation;
	case 1:
		return Ability1Animation;
	case 2:
		return Ability2Animation;
	case 3:
		return Ability3Animation;
	case 4:
		return Ability4Animation;
	}
	return nullptr;
}


// Called when the game starts
void UCharacterAbilityCasterComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCharacterAbilityCasterComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}
