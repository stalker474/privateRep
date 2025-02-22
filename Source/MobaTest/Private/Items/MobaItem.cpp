// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "MobaItem.h"

UMobaItem::UMobaItem(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//bReplicates = true;
	Cost = 0;
	Consumable = false;
	ParentItem = nullptr;
	bReplicates = true;
}

void UMobaItem::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMobaItem, Name);
	DOREPLIFETIME(UMobaItem, Description);
	DOREPLIFETIME(UMobaItem, Icon);
	DOREPLIFETIME(UMobaItem, Cost);
	DOREPLIFETIME(UMobaItem, SellPrice);
	DOREPLIFETIME(UMobaItem, Consumable);
	DOREPLIFETIME(UMobaItem, Effects);
	DOREPLIFETIME(UMobaItem, ParentItem);
}

void UMobaItem::Apply(TArray<UTickEffect*>& TickEffects)
{
	for (TSubclassOf<UTickEffect>& tickEffectClass : Effects)
	{
		if (!TickEffects.ContainsByPredicate([tickEffectClass](UTickEffect* effect) { return effect->GetClass() == tickEffectClass;}))
		{
			UTickEffect * newEffect = NewObject<UTickEffect>(this, tickEffectClass);
			TickEffects.Add(newEffect);
		}
	}
}
