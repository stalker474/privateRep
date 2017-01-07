// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "MobaAreaEffectBox.h"

// Sets default values
AMobaAreaEffectBox::AMobaAreaEffectBox(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	UBoxComponent * comp = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("CollisionCompBox"));
	comp->SetBoxExtent(FVector(1, 1, 1));
	CollisionComp = comp;
	CollisionComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	CollisionComp->bAutoActivate = true;
	int k = 6;
}





