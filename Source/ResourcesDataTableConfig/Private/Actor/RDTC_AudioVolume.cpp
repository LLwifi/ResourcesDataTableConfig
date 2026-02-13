// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/RDTC_AudioVolume.h"
#include <ResourcesConfig.h>

void ARDTC_AudioVolume::BeginPlay()
{
	Super::BeginPlay();
	RefreshSettingFromRowName(AudioVolumeInfoHandle.RowName);
}

#if WITH_EDITOR
TArray<FName> ARDTC_AudioVolume::GetDTAudioVolumeInfoRowNames()
{
	UDataTable* DataTable = UResourcesConfig::GetInstance()->AudioVolumeInfoDataTable.LoadSynchronous();
	if (DataTable)
	{
		return DataTable->GetRowNames();
	}
	return TArray<FName>();
}

void ARDTC_AudioVolume::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	auto Property = PropertyChangedEvent.Property;//拿到改变的属性

	if (Property->GetFName() == "RowName")
	{
		RefreshSettingFromRowName(AudioVolumeInfoHandle.RowName);
	}
}
#endif

void ARDTC_AudioVolume::RefreshSettingFromInfo(FRDTC_AudioVolumeInfo AudioVolumeInfo)
{
	SetPriority(AudioVolumeInfo.Priority);
	SetEnabled(AudioVolumeInfo.bIsEnabled);
	SetReverbSettings(AudioVolumeInfo.Settings);
	SetInteriorSettings(AudioVolumeInfo.AmbientZoneSettings);
	SetSubmixSendSettings(AudioVolumeInfo.SubmixSendSettings);
	SetSubmixOverrideSettings(AudioVolumeInfo.SubmixOverrideSettings);
}

void ARDTC_AudioVolume::RefreshSettingFromRowName(FName RowName)
{
	UDataTable* DataTable = UResourcesConfig::GetInstance()->AudioVolumeInfoDataTable.LoadSynchronous();
	if (DataTable)
	{
		FRDTC_AudioVolumeInfo* Info = DataTable->FindRow<FRDTC_AudioVolumeInfo>(RowName, TEXT(""));
		if (Info)
		{
			RefreshSettingFromInfo(*Info);
		}
	}
}
