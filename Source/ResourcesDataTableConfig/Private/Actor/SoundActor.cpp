// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/SoundActor.h"
#include "Components/AudioComponent.h"
#include <ResourceBPFunctionLibrary.h>
#include "SceneComponent/SoundAssetTagAudioComponent.h"

ASoundActor::ASoundActor()
{
	SoundAssetTagAudioComponent = CreateDefaultSubobject<USoundAssetTagAudioComponent>(TEXT("SoundAssetTagAudioComponent"));
}

void ASoundActor::BeginPlay()
{
	Super::BeginPlay();
}
