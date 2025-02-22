// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MobaTest.h"
#include "StrategyAIDirector.h"
#include "StrategyBuilding_Brewery.h"
#include "StrategyGameBlueprintLibrary.h"
#include "StrategyAttachment.h"

DEFINE_LOG_CATEGORY(LogGame);

UStrategyAIDirector::UStrategyAIDirector(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, WaveSize(8)
	, RadiusToSpawnOn(0)
	, CustomScale(1.0)
	, AnimationRate(1)
	, NextSpawnTime(0)
	, MyTeamNum(EStrategyTeam::Spectator)
	,LeftToSpawn(8)
	,TimeBetweenSpawns(60.0f)
	,TimeSinceLastSpawn(0.0f)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// @todo, why aren't these set in BuffData ctor?
	BuffModifier.BuffData.AttackMin = 0;
	BuffModifier.BuffData.AttackMax = 0;
	BuffModifier.BuffData.DamageReduction = 0;
	BuffModifier.BuffData.MaxHealthBonus = 0;
	BuffModifier.BuffData.HealthRegen = 0;
	BuffModifier.BuffData.Speed = 0;
	BuffModifier.Duration = 0;
	BuffModifier.bInfiniteDuration = false;
}

uint8 UStrategyAIDirector::GetTeamNum() const
{
	return MyTeamNum;
}

void  UStrategyAIDirector::SetTeamNum(uint8 inTeamNum)
{
	MyTeamNum = inTeamNum;
}

void UStrategyAIDirector::OnGameplayStateChange(EGameplayState::Type NewState)
{
	if (NewState == EGameplayState::Playing)
	{
		Activate();
		NextSpawnTime = 0;
	}
}

AStrategyBuilding_Brewery* UStrategyAIDirector::GetEnemyBrewery() const
{
	return EnemyBrewery.Get();
}

void UStrategyAIDirector::SetDefaultArmor(UBlueprint* InArmor)
{
	DefaultArmor = InArmor ? *InArmor->GeneratedClass : nullptr;
}

void UStrategyAIDirector::SetDefaultArmorClass(TSubclassOf<UStrategyAttachment> InArmor)
{
	DefaultArmor = InArmor;
}

void UStrategyAIDirector::SetDefaultWeapon(UBlueprint* InWeapon)
{
	DefaultWeapon = InWeapon ? *InWeapon->GeneratedClass : nullptr;
}

void UStrategyAIDirector::SetDefaultWeaponClass(TSubclassOf<UStrategyAttachment> InWeapon)
{
	DefaultWeapon = InWeapon;
}

void UStrategyAIDirector::SetBuffModifier(AStrategyChar* InChar, int32 AttackMin, int32 AttackMax, int32 DamageReduction, int32 MaxHealthBonus, int32 HealthRegen, float Speed, int32 DrunkLevel, float Duration, bool bInfiniteDuration, float InCustomScale, float InAnimaRate)
{
	BuffModifier.BuffData.AttackMin = AttackMin;
	BuffModifier.BuffData.AttackMax = AttackMax;
	BuffModifier.BuffData.DamageReduction = DamageReduction;
	BuffModifier.BuffData.MaxHealthBonus = MaxHealthBonus;
	BuffModifier.BuffData.HealthRegen = HealthRegen;
	BuffModifier.BuffData.Speed = Speed;
	BuffModifier.Duration = Duration;
	BuffModifier.bInfiniteDuration = bInfiniteDuration;
	CustomScale = InCustomScale;
	AnimationRate = InAnimaRate;
}

struct OffsetsGeneratorHelper
{
	float Offset[6];
	int32 LastIndex;

	OffsetsGeneratorHelper()
		: LastIndex( FMath::RandRange(0, 5) )
	{
		TArray<float> AllSlots;
		for (int32 Idx = 0; Idx < 6; Idx++)
		{
			AllSlots.Add((Idx - 6/2) * 45);
		}

		// let's give better order for our spots
		const int32 Indexes[6] = {3,2,4,1,5,0};
		for(int32 Idx = 0; Idx < 6; Idx++)
		{
			Offset[Idx] = AllSlots[Indexes[Idx]];
		}
	}

	float GetOffset()
	{
		LastIndex = ++LastIndex >= 6 ? 0 : LastIndex;
		return Offset[LastIndex];
	}
};

void UStrategyAIDirector::SpawnMinions()
{
	static OffsetsGeneratorHelper OffsetsGenerator;

	const bool bShoudSpawnNewUnits = GetWorld()->GetTimeSeconds() > NextSpawnTime;
	if (!bShoudSpawnNewUnits)
	{
		return;
	}

	if (EnemyBrewery == nullptr)
	{
		const EStrategyTeam::Type EnemyTeamNum = (MyTeamNum == EStrategyTeam::Blue ? EStrategyTeam::Red : EStrategyTeam::Blue);
		FPlayerData* const EnemyTeamData = GetWorld()->GetGameState<AStrategyGameState>()->GetPlayerData(EnemyTeamNum);
		if (EnemyTeamData != nullptr && EnemyTeamData->GetLaneBuilding(LanePosition) != nullptr)
		{
			EnemyBrewery = EnemyTeamData->GetLaneBuilding(LanePosition);
		}
	}

	if(LeftToSpawn > 0)
	{
		// find best place on ground to spawn at
		const AStrategyBuilding_Brewery* const Owner = Cast<AStrategyBuilding_Brewery>(GetOwner());
		check(Owner);
		bool bSpawnedNewMinion = false;
		if( Owner->MinionCharClass != nullptr )
		{
			FVector Loc = Owner->GetActorLocation();
			const FVector X = Owner->GetTransform().GetScaledAxis( EAxis::X );
			const FVector Y = Owner->GetTransform().GetScaledAxis( EAxis::Y );
			Loc += X * RadiusToSpawnOn +  Y * OffsetsGenerator.GetOffset();

			const FVector Scale(CustomScale);
			const FVector TraceOffset(0.0f,0.0f,RadiusToSpawnOn * 0.5 * Scale.Z);
			FHitResult Hit;
			FCollisionObjectQueryParams ObjectParams( FCollisionObjectQueryParams::AllStaticObjects );
			GetWorld()->LineTraceSingleByObjectType(Hit, Loc + TraceOffset, Loc - TraceOffset, ObjectParams);
			if (Hit.Actor.IsValid())
			{
				Loc = Hit.Location + FVector(0.0f,0.0f,Scale.Z * 10.0f);
			}
			AStrategyChar* StrategyChar = Owner->MinionCharClass->GetDefaultObject<AStrategyChar>();
			const float CapsuleHalfHeight = StrategyChar->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
			const float CapsuleRadius = StrategyChar->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
			Loc = Loc + FVector( 0.0f,0.0f,Scale.Z * CapsuleHalfHeight);

			// and spawn our minion
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AStrategyChar* const MinionChar =  GetWorld()->SpawnActor<AStrategyChar>(Owner->MinionCharClass, Loc, Owner->GetActorRotation(), SpawnInfo);
			MinionChar->LanePosition = LanePosition;
			// don't continue if he died right away on spawn
			if ( (MinionChar != nullptr) && (MinionChar->bIsDying == false) )
			{
				// Flag a successful spawn
				bSpawnedNewMinion = true;

				MinionChar->SetTeamNum(GetTeamNum());

				MinionChar->SpawnDefaultController();
				MinionChar->GetCapsuleComponent()->SetRelativeScale3D(Scale);
				MinionChar->GetCapsuleComponent()->SetCapsuleSize(CapsuleRadius, CapsuleHalfHeight);
				MinionChar->GetMesh()->GlobalAnimRateScale = AnimationRate;

				AStrategyGameState* const GameState = GetWorld()->GetGameState<AStrategyGameState>();

				LeftToSpawn -= 1;
				LeftToSpawn = FMath::Max(LeftToSpawn, 0);
				if (Owner != nullptr && WaveSize <= 0)
				{
					Owner->OnWaveSpawned.Broadcast();
				}
				NextSpawnTime = GetWorld()->GetTimeSeconds() + FMath::FRandRange(2.0f, 3.0f);
			}
			else
			{
				UE_LOG(LogGame, Warning, TEXT("Failed to spawn minion.") );
			}
		}
		else
		{
			// If we dont have a class type we cannot spawn a minion. 
			UE_LOG(LogGame, Warning, TEXT("No minion class specified in %s. Cannot spawn minion"), *Owner->GetName() );			
		}
		// If we failed to spawn a minion try again soon
		if( bSpawnedNewMinion == false )
		{
			NextSpawnTime = GetWorld()->GetTimeSeconds() + 0.1f;
		}
	}
}

void UStrategyAIDirector::RequestSpawn()
{
	LeftToSpawn += 1;
}

void UStrategyAIDirector::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (GetOwnerRole() == ROLE_Authority)
	{
		TimeSinceLastSpawn += DeltaTime;
		if (TimeSinceLastSpawn >= TimeBetweenSpawns)
		{
			TimeSinceLastSpawn = 0.0f;
			LeftToSpawn = WaveSize;
		}
		SpawnMinions();
	}
}
