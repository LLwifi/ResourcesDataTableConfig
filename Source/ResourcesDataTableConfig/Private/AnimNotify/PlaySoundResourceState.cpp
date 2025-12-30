// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/PlaySoundResourceState.h"
#include <Subsystems/SubsystemBlueprintLibrary.h>
#include "WorldSubsystem/SoundSubsystem.h"
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "Components/AudioComponent.h"

FString UPlaySoundResourceState::GetNotifyName_Implementation() const
{

	if (SoundAssetTag.Num() > 0)
	{
		FString Name;
		for (FResourceProperty_SoundAssetTag AssetTag : SoundAssetTag)
		{
			Name += AssetTag.RowName.ToString() + "-" + AssetTag.ResourceNameOrIndex + "|";
		}
		return Name;
	}
	return Super::GetNotifyName_Implementation();
}

void UPlaySoundResourceState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	//Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	AnimOwner = MeshComp->GetOwner();
	World = MeshComp->GetWorld();
#if WITH_EDITORONLY_DATA
	if (World && World->WorldType == EWorldType::EditorPreview)
	{
		Play(MeshComp);//编辑器始终能听见声音
	}
	else
#endif
	{
		if (!bUseActorVelocityCheck)//如果不检测速度，那么直接播放
		{
			Play(MeshComp);
		}
	}
}

void UPlaySoundResourceState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	if (bUseActorVelocityCheck && AnimOwner)
	{
		float CurVelocity = UKismetMathLibrary::VSize(AnimOwner->GetVelocity());
		if (bVelocityGreaterThan)
		{
			if (CurVelocity > ActorVelocity && !bIsPlay_Tick)//达到条件 && 还没开始播放声音
			{
				Play(MeshComp);
				bIsPlay_Tick = true;
			}
			else if (CurVelocity < ActorVelocity && bIsPlay_Tick)
			{
				EndPlay();
			}
		}
	}
}

void UPlaySoundResourceState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	EndPlay();
}

USoundSubsystem* UPlaySoundResourceState::GetSoundSubsystem()
{
	if (!SoundSubsystem)
	{
		SoundSubsystem = Cast<USoundSubsystem>(USubsystemBlueprintLibrary::GetWorldSubsystem(GWorld, USoundSubsystem::StaticClass()));
	}
	return SoundSubsystem;
}

TObjectPtr<USoundBase> UPlaySoundResourceState::GetSound(FName SoundRowName, FString SoundResourceNameOrIndex)
{
	FResourceProperty_SoundInfo SoftSound;
	if (UResourceBPFunctionLibrary::GetResourceFromString_Sound(SoundRowName, SoundResourceNameOrIndex, SoftSound))
	{
		return SoftSound.Sound.LoadSynchronous();
	}
	return nullptr;
}

void UPlaySoundResourceState::SetParameter(UAudioComponent* AudioComponent, FName SoundRowName, FString SoundResourceNameOrIndex)
{
	if (AudioComponent)
	{
		if (bUseAnimNotifyParameter)
		{
			UResourceBPFunctionLibrary::SetParameterFromAudioComponent(AudioComponent, SoundParameters);
		}
		else
		{
			FResourceProperty_SoundInfo SoundInfo;
			if (UResourceBPFunctionLibrary::GetResourceFromString_Sound(SoundRowName, SoundResourceNameOrIndex, SoundInfo))
			{
				UResourceBPFunctionLibrary::SetParameterFromAudioComponent(AudioComponent, SoundInfo.Parameters);
			}
		}
	}
}

void UPlaySoundResourceState::Play(USkeletalMeshComponent* MeshComp)
{
	UE_LOG(LogTemp, Log, TEXT("UPlaySoundResourceState----------Play"));
	for (UAudioComponent*& AudioCom : SoundComponent)
	{
		if (AudioCom)
		{
			if (bIsFadeOut)
			{
				AudioCom->FadeOut(FadeOutTime, 0.0f);
			}
			else
			{
				AudioCom->Stop();
			}
		}
	}

	TArray<FResourceProperty_SoundAssetTag> UseSoundAssetTags = SoundAssetTag;
	if (IsRandomPlayOneSound && UseSoundAssetTags.Num() > 1)//如果只有一个就无需再随机了
	{
		FResourceProperty_SoundAssetTag OneSoundAssetTag = UseSoundAssetTags[UKismetMathLibrary::RandomIntegerInRange(0, UseSoundAssetTags.Num() - 1)];
		UseSoundAssetTags.Empty();
		UseSoundAssetTags.Add(OneSoundAssetTag);
	}
	TArray<UAudioComponent*> NewSoundComponent;

#if WITH_EDITORONLY_DATA
	if (World && World->WorldType == EWorldType::EditorPreview)
	{
		USoundConcurrency* ConcurrencySettings = nullptr;//并发性设置
		for (const TSoftObjectPtr<USoundConcurrency>& SoftPtr : PlaySoundSetting.ConcurrencySettings)
		{
			if (USoundConcurrency* LoadedAsset = SoftPtr.LoadSynchronous())//尝试同步加载（确保获取到有效对象）
			{
				ConcurrencySettings = LoadedAsset;
				break; //找到第一个有效的就退出
			}
		}
		for (int32 i = 0; i < UseSoundAssetTags.Num(); i++)
		{
			UAudioComponent* AudioComponent = UGameplayStatics::SpawnSound2D(World, GetSound(UseSoundAssetTags[i].RowName, UseSoundAssetTags[i].ResourceNameOrIndex), PlaySoundSetting.VolumeMultiplier, PlaySoundSetting.PitchMultiplier, PlaySoundSetting.StartTime, ConcurrencySettings, PlaySoundSetting.bPersistAcrossLevelTransition, PlaySoundSetting.bAutoDestroy);
			NewSoundComponent.Add(AudioComponent);
			//SetParameter(AudioComponent, UseSoundAssetTags[i].RowName, UseSoundAssetTags[i].ResourceNameOrIndex);
		}
	}
	else
#endif
	{
		if (bFollow)
		{
			NewSoundComponent = GetSoundSubsystem()->PlaySound_Attached_Array(MeshComp->GetOwner(), UseSoundAssetTags, SoundTag, MeshComp, PlaySoundSetting, IsRandomPlayOneSound, !bUseAnimNotifyParameter, AttachName, FVector(ForceInit), FRotator(), EAttachLocation::SnapToTarget, false);
		}
		else
		{
			NewSoundComponent = GetSoundSubsystem()->PlaySound_Location_Array(MeshComp->GetWorld(), MeshComp->GetOwner(), UseSoundAssetTags, SoundTag, MeshComp->GetComponentLocation(), PlaySoundSetting, IsRandomPlayOneSound, !bUseAnimNotifyParameter, FRotator());
		}
	}

	//bool NewSoundComponentIsNull = true;
	for (int32 i = 0; i < NewSoundComponent.Num(); i++)
	{
		if (NewSoundComponent[i])
		{
			//NewSoundComponentIsNull = false;
			SetParameter(NewSoundComponent[i], UseSoundAssetTags[i].RowName, UseSoundAssetTags[i].ResourceNameOrIndex);
			if (bIsFadeIn)
			{
				NewSoundComponent[i]->FadeIn(FadeInTime);
			}
		}
	}
	SoundComponent = NewSoundComponent;
}

void UPlaySoundResourceState::EndPlay()
{
	UE_LOG(LogTemp, Log, TEXT("UPlaySoundResourceState----------EndPlay"));
	for (UAudioComponent*& AudioCom : SoundComponent)
	{
		if (AudioCom)
		{
			if (bIsFadeOut)
			{
				AudioCom->FadeOut(FadeOutTime, 0.0f);
			}
			else
			{
				AudioCom->Stop();
			}
		}
	}
	SoundComponent.Empty();
	bIsPlay_Tick = false;
}
