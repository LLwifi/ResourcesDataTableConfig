// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/SoundActor.h"
#include "Components/AudioComponent.h"
#include <ResourceBPFunctionLibrary.h>
#include "SceneComponent/SoundAssetTagAudioComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"

ASoundActor::ASoundActor()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SoundAssetTagAudioComponent = CreateDefaultSubobject<USoundAssetTagAudioComponent>(TEXT("SoundAssetTagAudioComponent"));
	SoundAssetTagAudioComponent->SetupAttachment(Root);
	SoundAssetTagAudioComponent->bAutoActivate = AutoActiveTime == 0.0f;
}

#if WITH_EDITOR
void ASoundActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	auto Property = PropertyChangedEvent.Property;

	if (Property->GetFName() == GET_MEMBER_NAME_CHECKED(ASoundActor, AutoActiveTime))
	{
		SoundAssetTagAudioComponent->bAutoActivate = AutoActiveTime == 0.0f;
	}
}
#endif

void ASoundActor::BeginPlay()
{
	Super::BeginPlay();

	if (AllShapeComponent.Num() <= 0)
	{
		GetComponents(UShapeComponent::StaticClass(), AllShapeComponent);
		UBoxComponent* BoxCom;
		USphereComponent* SphereCom;
		UCapsuleComponent* CapsuleCom;

		for (UShapeComponent*& ShapeCom : AllShapeComponent)
		{
			ShapeCom->OnComponentBeginOverlap.AddDynamic(this, &ASoundActor::OnBeginOverlapCheck);
			ShapeCom->OnComponentEndOverlap.AddDynamic(this, &ASoundActor::OnEndOverlapCheck);

			BoxCom = Cast<UBoxComponent>(ShapeCom);
			if (BoxCom)
			{
				AllBoxComponent.Add(BoxCom);
			}
			SphereCom = Cast<USphereComponent>(ShapeCom);
			if (SphereCom)
			{
				AllSphereComponent.Add(SphereCom);
			}
			CapsuleCom = Cast<UCapsuleComponent>(ShapeCom);
			if (CapsuleCom)
			{
				AllCapsuleComponent.Add(CapsuleCom);
			}
		}
	}

	if (AutoActiveTime > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(AutoActiveTimeTimerHandler, this, &ASoundActor::ActiveSoundComponent, AutoActiveTime);
	}
}

void ASoundActor::ActiveSoundComponent()
{
	SoundAssetTagAudioComponent->SetActive(true);
}

void ASoundActor::OnBeginOverlapCheck(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlapCheck.Contains(OtherActor))
	{
		OverlapCheck[OtherActor].OverlapCollision.Add(OverlappedComponent);
	}
	else
	{
		OverlapCheck.Add(OtherActor, FSoundCollisionOverlapCheck(OverlappedComponent));
	}

	OnBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void ASoundActor::OnEndOverlapCheck(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OverlapCheck.Contains(OtherActor))
	{
		OverlapCheck[OtherActor].OverlapCollision.Remove(OverlappedComponent);
		if (OverlapCheck[OtherActor].OverlapCollision.Num() <= 0)
		{
			OnEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
		}
	}
}

void ASoundActor::OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CollisionAutoActiveTime == 0.0f)
	{
		ActiveSoundComponent();
	}
	else if (CollisionAutoActiveTime > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(CollisionAutoActiveTimeTimerHandler, this, &ASoundActor::ActiveSoundComponent, CollisionAutoActiveTime);
	}
}

void ASoundActor::OnEndOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
