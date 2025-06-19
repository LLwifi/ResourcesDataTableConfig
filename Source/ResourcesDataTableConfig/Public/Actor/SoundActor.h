// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/AmbientSound.h"
#include <ResourcesStructAndEnum.h>
#include "SoundActor.generated.h"


/**
 * 声音Actor，该类用于替代直接拖拽到场景的音效
 */
UCLASS()
class RESOURCESDATATABLECONFIG_API ASoundActor : public AAmbientSound
{
	GENERATED_BODY()

public:
	ASoundActor();
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void BeginPlay() override;
	
	//刷新声音
	UFUNCTION(BlueprintCallable, CallInEditor)
	void Refresh();

public:
	//要播放的声音
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FResourceProperty_SoundAssetTag SoundAssetTag;
};
