// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BonusCampAIDirector.h"
#include "MobaAICharacter.h"
#include "GameFramework/Pawn.h"
#include "BonusCamp.generated.h"

UCLASS()
class MOBATEST_API ABonusCamp : public APawn
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BonusCamp, meta = (AllowPrivateAccess = "true"))
	UBonusCampAIDirector* AIDirector;

	UPROPERTY(EditDefaultsOnly, Category = BonusCamp, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* CapsuleTriggerBox;

public:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** notify about new game state */
	void OnGameplayStateChange(EGameplayState::Type NewState);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	/** The class of minion to spawn. */
	UPROPERTY(EditAnywhere, Category = BonusCamp)
	TSubclassOf<AMobaAICharacter> MonsterCharClass;
};
