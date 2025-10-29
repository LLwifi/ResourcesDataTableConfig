// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/SoundLine.h"
#include "Components/SplineComponent.h"
#include "Components/AudioComponent.h"
#include "Actor/SoundActor.h"
#include "SceneComponent/SoundAssetTagAudioComponent.h"

// Sets default values
ASoundLine::ASoundLine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SoundSpline = CreateDefaultSubobject<USplineComponent>("SoundSpline");
	SetRootComponent(SoundSpline);
}
#if WITH_EDITOR
void ASoundLine::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	auto Property = PropertyChangedEvent.Property;//�õ��ı������

	if (Property->GetFName() == "AttenuationSettings")
	{
		for (ASoundActor*& SoundActor : SoundActors)
		{
			SoundActor->SoundAssetTagAudioComponent->SetAttenuationSettings(AttenuationSettings);
		}
	}
	else if (Property->GetFName() == "ResourceNameOrIndex" || Property->GetFName() == "RowName")
	{
		for (ASoundActor*& SoundActor : SoundActors)
		{
			SoundActor->SoundAssetTagAudioComponent->SoundAssetTag = SoundAssetTag;
			SoundActor->SoundAssetTagAudioComponent->Refresh();
		}
	}
}
#endif
// Called when the game starts or when spawned
void ASoundLine::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASoundLine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASoundLine::Destroyed()
{
	Delete();
}

void ASoundLine::Refresh()
{
	Delete();
	float Weight = 1.0f / (float)(SoundNum - 1.0f);//ÿһ��ռ��Ȩ��
	for (int32 i = 0; i < SoundNum; i++)
	{
		ASoundActor* SoundActor = GetWorld()->SpawnActor<ASoundActor>();
		if (SoundActor)
		{
			SoundActor->SetActorLocation(SoundSpline->GetLocationAtTime(Weight * i, ESplineCoordinateSpace::World));
			SoundActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			SoundActors.Add(SoundActor);
			SoundActor->SoundAssetTagAudioComponent->SoundAssetTag = SoundAssetTag;
			SoundActor->SoundAssetTagAudioComponent->Refresh();
		}
	}
}

void ASoundLine::Delete()
{
	TArray<ASoundActor*> AllSoundActor = SoundActors;
	for (ASoundActor* SoundActor : AllSoundActor)
	{
		if (SoundActor)
		{
			SoundActor->Destroy();
		}
	}
	SoundActors.Empty();
}

