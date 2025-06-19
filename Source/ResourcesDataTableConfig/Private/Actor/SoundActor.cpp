// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/SoundActor.h"
#include "Components/AudioComponent.h"
#include <ResourceBPFunctionLibrary.h>

ASoundActor::ASoundActor()
{
}

#if WITH_EDITOR
void ASoundActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	auto Property = PropertyChangedEvent.Property;

	if (Property->GetFName() == "ResourceNameOrIndex" || Property->GetFName() == "RowName")
	{
		Refresh();
	}
}
#endif
void ASoundActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASoundActor::Refresh()
{
	FResourceProperty_SoundInfo SoundInfo;
	FBGMInfo BGMInfo;
	USoundBase* Sound = nullptr;
	TArray<FAudioParameter> AudioParameters;

	switch (SoundAssetTag.SoundAssetType)
	{
	case ESoundAssetType::Sound:
	{
		if (UResourceBPFunctionLibrary::GetResourceFromString_Sound(SoundAssetTag.RowName, SoundAssetTag.ResourceNameOrIndex, SoundInfo))
		{
			Sound = SoundInfo.Sound.LoadSynchronous();
			AudioParameters = SoundInfo.Parameters.GetAllAudioParameter();
		}
		break;
	}
	case ESoundAssetType::BGM:
	{
		if (UResourceBPFunctionLibrary::GetResourceFromString_BGM(SoundAssetTag.RowName, SoundAssetTag.ResourceNameOrIndex, BGMInfo))
		{
			Sound = BGMInfo.SoundBase.LoadSynchronous();
			AudioParameters = BGMInfo.Parameters.GetAllAudioParameter();
		}
		break;
	}
	default:
		break;
	}

	GetAudioComponent()->SetSound(Sound);
	GetAudioComponent()->DefaultParameters = AudioParameters;
}
