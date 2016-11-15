// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "MobaAreaEffectSphere.h"

AMobaAreaEffectSphere::AMobaAreaEffectSphere(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	USphereComponent * comp = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("CollisionCompSphere"));
	comp->SetSphereRadius(1.0f);
	CollisionComp = comp;
	CollisionComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
}



