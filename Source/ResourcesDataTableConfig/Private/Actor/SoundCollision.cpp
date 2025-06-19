// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/SoundCollision.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include <Subsystems/SubsystemBlueprintLibrary.h>
#include "WorldSubsystem/SoundSubsystem.h"
#include <ResourceBPFunctionLibrary.h>
#include <ActorComponent/SoundComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>


// Sets default values
ASoundCollision::ASoundCollision()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ShapeColor = FColor(223, 149, 157, 255);


	RootScene = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(RootScene);

	BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BodyInstance.SetResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

#if WITH_EDITOR
void ASoundCollision::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	auto Property = PropertyChangedEvent.Property;//拿到改变的属性

	if (Property->GetFName() == GET_MEMBER_NAME_CHECKED(ASoundCollision, CollisionShape))
	{
		Refresh();
	}
	else if (Property->GetFName() == "X" || Property->GetFName() == "Y" || Property->GetFName() == "Z")
	{
		if (BoxComponent)
		{
			BoxComponent->SetBoxExtent(BoxExtent);
		}
	}
	else if (Property->GetFName() == GET_MEMBER_NAME_CHECKED(ASoundCollision, SphereRadius))
	{
		if (SphereComponent)
		{
			SphereComponent->SetSphereRadius(SphereRadius);
		}
	}
	else if (Property->GetFName() == GET_MEMBER_NAME_CHECKED(ASoundCollision, CapsuleHalfHeight))
	{
		if (CapsuleComponent)
		{
			CapsuleComponent->SetCapsuleHalfHeight(CapsuleHalfHeight);
		}
	}
	else if (Property->GetFName() == GET_MEMBER_NAME_CHECKED(ASoundCollision, CapsuleRadius))
	{
		if (CapsuleComponent)
		{
			CapsuleComponent->SetCapsuleRadius(CapsuleRadius);
		}
	}
	else if (Property->GetFName() == GET_MEMBER_NAME_CHECKED(ASoundCollision, LineThickness))
	{
		if (ShapeComponent)
		{
			ShapeComponent->SetLineThickness(LineThickness);
		}
	}
	else if (Property->GetFName() == GET_MEMBER_NAME_CHECKED(ASoundCollision, ShapeColor))
	{
		if (ShapeComponent)
		{
			ShapeComponent->ShapeColor = ShapeColor;
		}
	}
	else if (Property->GetFName().ToString().Contains("Collision"))//碰撞相关
	{
		if (ShapeComponent)
		{
			ShapeComponent->BodyInstance = BodyInstance;
		}
	}
}

TArray<FString> ASoundCollision::SoundEventName()
{
	TArray<FString> SoundEventName = { "None" };
	UDataTable* DT = UResourcesConfig::GetInstance()->SoundEventDataTable.LoadSynchronous();
	if (DT)
	{
		for (FName& Row : DT->GetRowNames())
		{
			SoundEventName.Add(Row.ToString());
		}
	}
	return SoundEventName;
}

#endif

// Called when the game starts or when spawned
void ASoundCollision::BeginPlay()
{
	Super::BeginPlay();
	
	if (ShapeComponent)
	{
		ShapeComponent->OnComponentBeginOverlap.AddDynamic(this, &ASoundCollision::OnBeginOverlap);
		ShapeComponent->OnComponentEndOverlap.AddDynamic(this, &ASoundCollision::OnEndOverlap);
		ShapeComponent->RegisterComponent();
	}

	if (bBeginPlayCheckCollision)
	{
		if (CheckCollisionDelayTime > 0.0f)
		{
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASoundCollision::CheckCollision, CheckCollisionDelayTime);
		}
		else
		{
			CheckCollision();
		}
	}
}

// Called every frame
void ASoundCollision::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASoundCollision::OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OverlapActor = OtherActor;
	SoundAction(SoundCollisionAction_BeginOverlap);
}

void ASoundCollision::OnEndOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlapActor = OtherActor;
	SoundAction(SoundCollisionAction_EndOverlap);
}

void ASoundCollision::Refresh()
{
	if (ShapeComponent)
	{
		ShapeComponent->DestroyComponent();
		ShapeComponent = nullptr;

		SphereComponent = nullptr;
		CapsuleComponent = nullptr;
		BoxComponent = nullptr;
	}

	switch (CollisionShape)
	{
	case ESoundCollisionShape::Sphere:
	{
		SphereComponent = NewObject<USphereComponent>(this, USphereComponent::StaticClass(), "Sphere");
		SphereComponent->SetSphereRadius(SphereRadius);
		///SphereComponent->RegisterComponent();
		ShapeComponent = SphereComponent;
		break;
	}
	case ESoundCollisionShape::Capsule:
	{
		CapsuleComponent = NewObject<UCapsuleComponent>(this, UCapsuleComponent::StaticClass(), "Capsule");
		CapsuleComponent->SetCapsuleHalfHeight(CapsuleHalfHeight);
		CapsuleComponent->SetCapsuleRadius(CapsuleRadius);
		//SphereComponent->RegisterComponent();
		ShapeComponent = CapsuleComponent;
		break;
	}
	case ESoundCollisionShape::Box:
	{
		BoxComponent = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), "Box");
		BoxComponent->SetBoxExtent(BoxExtent);
		//BoxComponent->RegisterComponent();
		ShapeComponent = BoxComponent;
		break;
	}
	default:
		break;
	}

	if (ShapeComponent)
	{
		ShapeComponent->AttachToComponent(RootScene, FAttachmentTransformRules::KeepRelativeTransform);
		ShapeComponent->BodyInstance = BodyInstance;
		ShapeComponent->ShapeColor = ShapeColor;
		ShapeComponent->SetLineThickness(LineThickness + 1.0f);
	}

	if (BoxComponent)
	{
		BoxComponent->SetBoxExtent(BoxExtent);
	}
	if (SphereComponent)
	{
		SphereComponent->SetSphereRadius(SphereRadius);
	}
	if (CapsuleComponent)
	{
		CapsuleComponent->SetCapsuleHalfHeight(CapsuleHalfHeight);
	}
	if (CapsuleComponent)
	{
		CapsuleComponent->SetCapsuleRadius(CapsuleRadius);
	}
}

void ASoundCollision::CheckCollision()
{
	FHitResult OutHit;
	switch (CollisionShape)
	{
	case ESoundCollisionShape::None:
		break;
	case ESoundCollisionShape::Sphere:
	{
		if (UKismetSystemLibrary::SphereTraceSingleForObjects(this, GetActorLocation(), GetActorLocation(), SphereRadius, CollisionObjectTypes, false, ActorsToIgnore, DrawDebugType, OutHit, true))
		{
			FHitResult HitResult;
			OnBeginOverlap(nullptr, OutHit.GetActor(), nullptr, 0, false, HitResult);
		}
		break;
	}
	case ESoundCollisionShape::Capsule:
	{
		if (UKismetSystemLibrary::CapsuleTraceSingleForObjects(this, GetActorLocation(), GetActorLocation(), CapsuleRadius, CapsuleHalfHeight, CollisionObjectTypes, false, ActorsToIgnore, DrawDebugType, OutHit, true))
		{
			FHitResult HitResult;
			OnBeginOverlap(nullptr, OutHit.GetActor(), nullptr, 0, false, HitResult);
		}
		break;
	}
	case ESoundCollisionShape::Box:
	{
		if (UKismetSystemLibrary::BoxTraceSingleForObjects(this, GetActorLocation(), GetActorLocation(), BoxExtent, GetActorRotation(), CollisionObjectTypes,false, ActorsToIgnore, DrawDebugType, OutHit, true))
		{
			FHitResult HitResult;
			OnBeginOverlap(nullptr,OutHit.GetActor(),nullptr,0,false, HitResult);
		}
		break;
	}
	default:
		break;
	}
}

void ASoundCollision::CameraLookCheckAngle()
{
	if (LookTriggerNum == -1 || CurLookTriggerNum < LookTriggerNum)
	{
		APlayerCameraManager* CameraMgr = UGameplayStatics::GetPlayerCameraManager(this, 0);
		if (CameraMgr)
		{
			FVector A = UKismetMathLibrary::Normal(CameraMgr->GetCameraRotation().Vector());
			FVector B = UKismetMathLibrary::Normal(GetActorLocation() - CameraMgr->GetCameraLocation());
			float LookAngle = UKismetMathLibrary::DegAcos(UKismetMathLibrary::Dot_VectorVector(A, B));
			if (LookAngle <= LookCheckAngle)
			{
				if (!CurLookIsTrigger)
				{
					LookTrigger();
					CurLookIsTrigger = true;
					CurLookTriggerNum++;
					if (LookTriggerNum != -1 && CurLookTriggerNum >= LookTriggerNum)
					{
						GetWorld()->GetTimerManager().ClearTimer(LookCheckTimerHandle);
					}
				}
			}
			else
			{
				CurLookIsTrigger = false;
			}
		}
	}

}

void ASoundCollision::LookTrigger_Implementation()
{
	SoundAction(SoundCollisionAction_Look);
}

void ASoundCollision::SoundAction(FSoundCollisionAction SoundCollisionAction)
{
	USoundSubsystem* SoundSubsystem = Cast<USoundSubsystem>(USubsystemBlueprintLibrary::GetWorldSubsystem(this, USoundSubsystem::StaticClass()));
	if (SoundSubsystem)
	{
		switch (SoundCollisionAction.SoundActionType)
		{
		case ESoundActionType::None:
		{
			break;
		}
		case ESoundActionType::Sound2D:
		{
			SoundSubsystem->PlaySound_2D_Array(this, this, SoundCollisionAction.ResourceProperty, FGameplayTag());
			break;
		}
		case ESoundActionType::Sound3D:
		{
			SoundSubsystem->PlaySound_Location_Array(this, this, SoundCollisionAction.ResourceProperty, FGameplayTag(), GetActorLocation());
			break;
		}
		case ESoundActionType::PushBGM:
		{
			for (FResourceProperty_SoundAssetTag& SoundAssetTag : SoundCollisionAction.ResourceProperty)
			{
				if (bPushBGMCheck)
				{
					FBGMInfo BGMInfo;
					if (UResourceBPFunctionLibrary::GetResourceFromString_BGM(SoundAssetTag.RowName, SoundAssetTag.ResourceNameOrIndex, BGMInfo))
					{
						//没有这个通道 该通道没有声音在播放 新的声音不是想推送的声音
						if (!SoundSubsystem->AllBGMChannel.Contains(BGMInfo.BGMChannelName) || !SoundSubsystem->AllBGMChannel[BGMInfo.BGMChannelName]->CurAudioCom || SoundSubsystem->AllBGMChannel[BGMInfo.BGMChannelName]->CurBGMInfo != BGMInfo)
						{
							BGMChannel.Add(SoundSubsystem->PushBGMToChannelOfInfo(BGMInfo));
						}
					}
				}
				else
				{
					BGMChannel.Add(SoundSubsystem->PushBGMToChannel(SoundAssetTag.RowName, SoundAssetTag.ResourceNameOrIndex));
				}
			}
			break;
		}
		case ESoundActionType::PopBGMOfChannelName:
		{
			for (FName& ChannelName : SoundCollisionAction.BGMChannelName)
			{
				SoundSubsystem->PopBGMChannel(ChannelName);
			}
			break;
		}
		case ESoundActionType::PopBGMAndStopSound:
		{
			for (UBGMChannel*& Channel : BGMChannel)
			{
				SoundSubsystem->PopBGMChannelOfChannel(Channel);
			}
			for (UAudioComponent*& SoundCom : SoundComs)
			{
				SoundCom->FadeOut(UResourcesConfig::GetInstance()->BGMFadeOutTime_2D, 0.0f);
			}
			break;
		}
		case ESoundActionType::PopBGM:
		{
			for (UBGMChannel*& Channel : BGMChannel)
			{
				SoundSubsystem->PopBGMChannelOfChannel(Channel);
			}
			break;
		}
		case ESoundActionType::StopSound:
		{
			for (UAudioComponent*& SoundCom : SoundComs)
			{
				SoundCom->FadeOut(UResourcesConfig::GetInstance()->BGMFadeOutTime_2D, 0.0f);
			}
			break;
		}
		case ESoundActionType::TriggerSoundEvent:
		{
			if (SoundCollisionAction.bCompareParameterIsDynamic && OverlapActor && OverlapActor->Implements<USoundEventInteract>())
			{
				SoundSubsystem->TriggerSoundEvent(SoundCollisionAction.SoundEventName, ISoundEventInteract::Execute_GetSoundEventCompareParameter(OverlapActor, SoundCollisionAction.SoundEventName), SoundComs, BGMChannel);
			}
			else
			{
				SoundSubsystem->TriggerSoundEvent(SoundCollisionAction.SoundEventName, SoundCollisionAction.CompareParameter, SoundComs, BGMChannel);
			}
			break;
		}
		case ESoundActionType::LookCheck:
		{
			GetWorld()->GetTimerManager().SetTimer(LookCheckTimerHandle, this, &ASoundCollision::CameraLookCheckAngle, LookCheckTime, true);
			break;
		}
		case ESoundActionType::LookCheckStop:
		{
			CurLookTriggerNum = 0;
			GetWorld()->GetTimerManager().ClearTimer(LookCheckTimerHandle);
			break;
		}
		default:
			break;
		}
	}
}

