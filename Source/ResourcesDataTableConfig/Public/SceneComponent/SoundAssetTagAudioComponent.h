// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include <ResourcesStructAndEnum.h>
#include "SoundAssetTagAudioComponent.generated.h"


/**
 * 使用SoundAssetTag配置播放音效的组件
 * 需要注意的是，当SoundAssetTag == ESoundAssetType::BGM时，并不会使用组件播放声音而是转用SoundSubsystem->PushBGMToChannelOfInfo(BGMInfo);
 */
UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent))
class RESOURCESDATATABLECONFIG_API USoundAssetTagAudioComponent : public UAudioComponent
{
	GENERATED_BODY()
public:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	virtual void Activate(bool bReset = false) override;

	//刷新声音
	UFUNCTION(BlueprintCallable)
	void Refresh();
	
public:
	//要播放的声音
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FResourceProperty_SoundAssetTag SoundAssetTag;
};
