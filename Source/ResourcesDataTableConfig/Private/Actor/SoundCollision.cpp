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


// Sets default values
ASoundCollision::ASoundCollision()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	RootScene = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(RootScene);
}

#if WITH_EDITOR
void ASoundCollision::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	auto Property = PropertyChangedEvent.Property;//拿到改变的属性

	//if (Property->GetFName().ToString().Contains("Collision"))//碰撞相关
	//{
	//	if (ShapeComponent)
	//	{
	//		ShapeComponent->BodyInstance = BodyInstance;
	//	}
	//}
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

	if (!ShapeComponent)
	{
		ShapeComponent = GetComponentByClass<UShapeComponent>();
	}
	
	if (ShapeComponent)
	{
		BoxComponent = Cast<UBoxComponent>(ShapeComponent);
		SphereComponent = Cast<USphereComponent>(ShapeComponent);
		CapsuleComponent = Cast<UCapsuleComponent>(ShapeComponent);

		ShapeComponent->OnComponentBeginOverlap.AddDynamic(this, &ASoundCollision::OnBeginOverlap);
		ShapeComponent->OnComponentEndOverlap.AddDynamic(this, &ASoundCollision::OnEndOverlap);
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
	if (OnlyPassClassArray.Num() <= 0)
	{
		SoundActionArray(SoundCollisionAction_BeginOverlap.GetAction());
	}
	else
	{
		for (TSoftClassPtr<AActor>& ActorClass : OnlyPassClassArray)
		{
			if (OverlapActor->IsA(ActorClass.LoadSynchronous()))
			{
				SoundActionArray(SoundCollisionAction_BeginOverlap.GetAction());
				break;
			}
		}
	}
}

void ASoundCollision::OnEndOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlapActor = OtherActor;
	if (OnlyPassClassArray.Num() <= 0)
	{
		SoundActionArray(SoundCollisionAction_EndOverlap.GetAction());
		if (bCollisionOnce)
		{
			ShapeComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	else
	{
		for (TSoftClassPtr<AActor>& ActorClass : OnlyPassClassArray)
		{
			if (OverlapActor->IsA(ActorClass.LoadSynchronous()))
			{
				SoundActionArray(SoundCollisionAction_EndOverlap.GetAction());
				if (bCollisionOnce)
				{
					ShapeComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				}
				break;
			}
		}
	}

}

void ASoundCollision::CheckCollision()
{
	//根据配置获取最终要触发的声音动作
	TArray<FSoundCollisionAction> UseSoundCollisionAction;
	if (bCheckSoundActionUseBeginOrEnd)
	{
		if (bUseBeginOverlap)
		{
			UseSoundCollisionAction = SoundCollisionAction_BeginOverlap.GetAction();
		}
		else
		{
			UseSoundCollisionAction = SoundCollisionAction_EndOverlap.GetAction();
		}
	}
	else
	{
		UseSoundCollisionAction = SoundCollisionAction_Check.GetAction();
	}

	FHitResult OutHit;
	bool IsPlay = false;
	FVector CheckLocation = GetActorLocation();
	if (SphereComponent)
	{
		CheckLocation += SphereComponent->GetRelativeLocation();
		IsPlay = UKismetSystemLibrary::SphereTraceSingleForObjects(this, CheckLocation, CheckLocation, SphereComponent->GetUnscaledSphereRadius(), CollisionObjectTypes, false, ActorsToIgnore, DrawDebugType, OutHit, true);
	}
	else if (CapsuleComponent)
	{
		CheckLocation += CapsuleComponent->GetRelativeLocation();
		IsPlay = UKismetSystemLibrary::CapsuleTraceSingleForObjects(this, CheckLocation, CheckLocation, CapsuleComponent->GetUnscaledCapsuleRadius(), CapsuleComponent->GetUnscaledCapsuleHalfHeight(), CollisionObjectTypes, false, ActorsToIgnore, DrawDebugType, OutHit, true);
	}
	else if (BoxComponent)
	{
		CheckLocation += BoxComponent->GetRelativeLocation();
		IsPlay = UKismetSystemLibrary::BoxTraceSingleForObjects(this, CheckLocation, CheckLocation, BoxComponent->GetUnscaledBoxExtent(), GetActorRotation(), CollisionObjectTypes, false, ActorsToIgnore, DrawDebugType, OutHit, true);
	}

	if (IsPlay)
	{
		OverlapActor = OutHit.GetActor();
		if (OnlyPassClassArray.Num() <= 0)
		{
			SoundActionArray(UseSoundCollisionAction);
		}
		else
		{
			for (TSoftClassPtr<AActor>& ActorClass : OnlyPassClassArray)
			{
				if (OverlapActor->IsA(ActorClass.LoadSynchronous()))
				{
					SoundActionArray(UseSoundCollisionAction);
					break;
				}
			}
		}
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
			if (OverlapActor && OverlapActor->Implements<USoundEventInteract>())
			{
				FCC_CompareInfo DynamicCompareParameter = ISoundEventInteract::Execute_GetSoundEventCompareParameter(OverlapActor, SoundCollisionAction.SoundEventName);
				DynamicCompareParameter.Append(SoundCollisionAction.CompareParameter);
				SoundSubsystem->TriggerSoundEvent(SoundCollisionAction.SoundEventName, DynamicCompareParameter, SoundComs, BGMChannel);
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
		case ESoundActionType::TriggerOtherCollisionCheck:
		{
			for (ASoundCollision*& SoundCollision : SoundCollisionAction.LinkSoundCollision)
			{
				if (SoundCollision)
				{
					SoundCollision->CheckCollision();
				}
			}
			break;
		}
		case ESoundActionType::TriggerOtherCollisionBeginOverlap:
		{
			for (ASoundCollision*& SoundCollision : SoundCollisionAction.LinkSoundCollision)
			{
				if (SoundCollision)
				{
					SoundCollision->OverlapActor = OverlapActor;
					SoundCollision->SoundActionArray(SoundCollision->SoundCollisionAction_BeginOverlap.GetAction());
				}
			}
			break;
		}
		case ESoundActionType::TriggerOtherCollisionEndOverlap:
		{
			for (ASoundCollision*& SoundCollision : SoundCollisionAction.LinkSoundCollision)
			{
				if (SoundCollision)
				{
					SoundCollision->OverlapActor = OverlapActor;
					SoundCollision->SoundActionArray(SoundCollision->SoundCollisionAction_EndOverlap.GetAction());
				}
			}
			break;
		}
		default:
			break;
		}
	}
}

void ASoundCollision::SoundActionArray(TArray<FSoundCollisionAction> SoundCollisionActionArray)
{
	for (FSoundCollisionAction& SoundCollisionAction : SoundCollisionActionArray)
	{
		SoundAction(SoundCollisionAction);
	}
}

