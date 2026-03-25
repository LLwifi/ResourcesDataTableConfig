// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneComponent/SoundAssetTagAudioComponent.h"
#include <ResourceBPFunctionLibrary.h>
#include <WorldSubsystem/SoundSubsystem.h>
#include <Subsystems/SubsystemBlueprintLibrary.h>

#if WITH_EDITOR
void USoundAssetTagAudioComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	auto Property = PropertyChangedEvent.Property;

	//if (Property->GetFName() == "ResourceNameOrIndex" || Property->GetFName() == "RowName")
	//{
	//	Refresh();
	//}
}
#endif

void USoundAssetTagAudioComponent::Activate(bool bReset)
{
	Refresh();
	if (SoundAssetTag.SoundAssetType == ESoundAssetType::BGM)//BGM 不适用音频组件播放
	{
		USoundSubsystem* SoundSubsystem = Cast<USoundSubsystem>(USubsystemBlueprintLibrary::GetWorldSubsystem(this, USoundSubsystem::StaticClass()));
		if (SoundSubsystem)
		{
			FBGMInfo BGMInfo;
			if (UResourceBPFunctionLibrary::GetResourceFromString_BGM(SoundAssetTag.RowName, SoundAssetTag.ResourceNameOrIndex, BGMInfo))
			{
				SoundSubsystem->PushBGMToChannelOfInfo(BGMInfo);
			}
		}
	}
	else
	{
		Super::Activate(bReset);
	}
}

void USoundAssetTagAudioComponent::Refresh()
{
	FResourceProperty_SoundInfo SoundInfo;
	FBGMInfo BGMInfo;
	USoundBase* SoundBase = nullptr;
	TArray<FAudioParameter> AudioParameters;

	switch (SoundAssetTag.SoundAssetType)
	{
	case ESoundAssetType::Sound:
	{
		if (UResourceBPFunctionLibrary::GetResourceFromString_Sound(SoundAssetTag.RowName, SoundAssetTag.ResourceNameOrIndex, SoundInfo))
		{
			SoundBase = SoundInfo.Sound.LoadSynchronous();
			AudioParameters = SoundInfo.Parameters.GetAllAudioParameter();
		}

		if (ConcurrencySet.Num() <= 0)//如果外部没有指定的话，尝试获取表里面配置的音效并发性设置
		{
			for (TSoftObjectPtr<USoundConcurrency>& Concurrency : SoundInfo.PlaySoundSetting.ConcurrencySettings)
			{
				ConcurrencySet.Add(Concurrency.LoadSynchronous());
			}
		}

		if (!AttenuationSettings)//如果外部没有指定的话，获取表里面配置的衰减
		{
			SetAttenuationSettings(SoundInfo.PlaySoundSetting.AttenuationSettings.LoadSynchronous());
		}
		break;
	}
	case ESoundAssetType::BGM:
	{
		if (UResourceBPFunctionLibrary::GetResourceFromString_BGM(SoundAssetTag.RowName, SoundAssetTag.ResourceNameOrIndex, BGMInfo))
		{
			SoundBase = BGMInfo.SoundBase.LoadSynchronous();
			AudioParameters = BGMInfo.Parameters.GetAllAudioParameter();
		}
		break;
	}
	default:
		break;
	}

	SetSound(SoundBase);
	DefaultParameters = AudioParameters;
}
