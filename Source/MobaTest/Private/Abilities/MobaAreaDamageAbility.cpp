// Fill out your copyright notice in the Description page of Project Settings.

#include "MobaTest.h"
#include "MobaAreaDamageAbility.h"
#include "MobaAreaEffect.h"

DEFINE_LOG_CATEGORY(YourLog);

UMobaAreaDamageAbility::UMobaAreaDamageAbility(const FObjectInitializer& ObjectInitializer)
{
	
}

// Called every frame
void UMobaAreaDamageAbility::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateState();
}

void UMobaAreaDamageAbility::OnCast()
{
	Super::OnCast();//played on both

	if (GetOwnerRole() == ROLE_Authority)
	{
		if (AreaEffectClass != NULL)
		{
			UWorld* const World = GetWorld();
			if (World)
			{
				FActorSpawnParameters spawnParams;
				spawnParams.Owner = GetOwner();
				spawnParams.Instigator = Cast<APawn>(GetOwner());

				FTransform mTransform;
				mTransform.SetIdentity();
				mTransform.SetLocation(ClientData.AimingPosition);
				mTransform.SetRotation(ClientData.AimingRotation.Quaternion());
				AMobaAreaEffect* myAreaEffect = World->SpawnActor<AMobaAreaEffect>(AreaEffectClass, mTransform, spawnParams);
			}

		}
		State = EAbilityStateEnum::NONE;
		UpdateState();
	}
}

void UMobaAreaDamageAbility::UpdateState()
{
	APlayerController * controller = UGameplayStatics::GetPlayerController(this, 0);

	if (GetOwnerRole() != ROLE_Authority && State == EAbilityStateEnum::AIMING)
	{
		
		//controller->bShowMouseCursor = true;
		FHitResult hit;
		FCollisionQueryParams traceParams(FName(TEXT("VictoreCore Trace")), true, GetOwner());
		traceParams.bTraceComplex = false;

		const ULocalPlayer* LocalPlayer = controller->GetLocalPlayer();
		FViewport* Viewport = LocalPlayer->ViewportClient->Viewport;
		FVector2D ViewportSize;
		LocalPlayer->ViewportClient->GetViewportSize(ViewportSize);
		const int32 X = static_cast<int32>(ViewportSize.X * 0.5f);
		const int32 Y = static_cast<int32>(ViewportSize.Y * 0.5f);
		FVector2D pos(X, Y);
		TArray<TEnumAsByte<EObjectTypeQuery>> list;

		if (controller->GetHitResultAtScreenPosition(pos, ECollisionChannel::ECC_Destructible, traceParams, hit))
		{
			FVector newImpactPoint = hit.ImpactPoint;
			TWeakObjectPtr<AActor> owner = this->GetOwner();
			if (owner.IsValid())
			{
				FVector targetRelativePos = hit.ImpactPoint - owner.Get()->GetActorLocation();
				FVector direction;
				float length;
				targetRelativePos.ToDirectionAndLength(direction, length);
				if (length > MaxDistance)
				{
					newImpactPoint = direction * MaxDistance + owner.Get()->GetActorLocation();
				}
			}

			if (TargetActor == nullptr)
			{
				FTransform mTransform;
				mTransform.SetIdentity();
				mTransform.SetLocation(newImpactPoint);

				AMobaAreaEffect * test = NewObject<AMobaAreaEffect>(this, AreaEffectClass);
				FVector ss = test->CollisionComp->RelativeScale3D;
				FVector sf = test->CollisionComp->RelativeLocation;
				TargetActor = GetWorld()->SpawnActor<AActor>(TargetActorClass, mTransform);
				TargetActor->SetActorScale3D(ss);
				TargetActor->SetActorRelativeLocation(sf);
				TargetActor->SetOwner(GetOwner());
				TargetActor->SetReplicates(false);
			}
			else
			{
				TargetActor->SetActorHiddenInGame(false);
				TargetActor->SetActorLocation(newImpactPoint);
				
				FRotator origTransform = TargetActor->GetActorRotation();
				origTransform.Yaw = controller->PlayerCameraManager->GetCameraRotation().Yaw;
				TargetActor->SetActorRotation(origTransform);
			}

		}
	}
	else
	{
		if (TargetActor)
			TargetActor->SetActorHiddenInGame(true);
		//controller->bShowMouseCursor = true;
	}
}

