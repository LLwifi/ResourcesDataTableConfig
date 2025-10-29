// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/AmbientSound.h"
#include <ResourcesStructAndEnum.h>
#include "SoundActor.generated.h"

class USoundAssetTagAudioComponent;

/**
 * 声音Actor，该类用于替代直接拖拽到场景的音效
 */
UCLASS()
class RESOURCESDATATABLECONFIG_API ASoundActor : public AActor
{
	GENERATED_BODY()

public:
	ASoundActor();

	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundAssetTagAudioComponent* SoundAssetTagAudioComponent;
};
