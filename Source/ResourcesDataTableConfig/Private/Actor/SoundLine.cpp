// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/SoundLine.h"
#include "Components/SplineComponent.h"
#include "Components/AudioComponent.h"
#include "SceneComponent/SoundAssetTagAudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASoundLine::ASoundLine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SoundSpline = CreateDefaultSubobject<USplineComponent>("SoundSpline");
	SetRootComponent(SoundSpline);
	SoundAssetTagAudioComponent = CreateDefaultSubobject<USoundAssetTagAudioComponent>(TEXT("SoundAssetTagAudioComponent"));
	SoundAssetTagAudioComponent->SetupAttachment(RootComponent);
}

#if WITH_EDITOR
void ASoundLine::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	auto Property = PropertyChangedEvent.Property;
}
#endif

// Called when the game starts or when spawned
void ASoundLine::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(CheckIntervalTimerHandler, this, &ASoundLine::CheckIntervalBack, CheckInterval, true);
}

// Called every frame
void ASoundLine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASoundLine::Destroyed()
{
}

AActor* ASoundLine::GetTargetActor()
{
	if (!TargetActor)
	{
		TargetActor = UGameplayStatics::GetPlayerPawn(this, 0);
	}
	return TargetActor;
}

void ASoundLine::CheckIntervalBack()
{
	if (GetTargetActor())
	{
		FVector TargetLocation = GetTargetActor()->GetActorLocation();//目标位置
		float InputKey = SoundSpline->FindInputKeyClosestToWorldLocation(TargetLocation);//根据目标位置获取在线段上的0~1值
		FVector SplineLocation = SoundSpline->GetLocationAtSplineInputKey(InputKey,ESplineCoordinateSpace::World);//线段上的位置
		if (bIsRange)
		{
			FRotator SplineRotator = SoundSpline->GetRotationAtSplineInputKey(InputKey, ESplineCoordinateSpace::World);//线段上的旋转/朝向
			FVector SplineRotatorDir = UKismetMathLibrary::Normal(UKismetMathLibrary::GetForwardVector(SplineRotator));
			FVector SplineToTargetDir = UKismetMathLibrary::Normal(TargetLocation - SplineLocation);//线段上的位置指向目标位置的方向
			FVector Cross = UKismetMathLibrary::Cross_VectorVector(SplineRotatorDir, SplineToTargetDir);

			//顺时针，圈内是正数
			if (bIsClockwise)
			{
				if (Cross.Z >= 0.0f)//圈内
				{
					SoundAssetTagAudioComponent->SetWorldLocation(TargetLocation);
				}
				else//圈外
				{
					SoundAssetTagAudioComponent->SetWorldLocation(SplineLocation);
					SoundAssetTagAudioComponent->SetWorldRotation(SplineRotator);
				}
			}
			else//逆时针，圈内是负数
			{
				if (Cross.Z <= 0.0f)//圈内
				{
					SoundAssetTagAudioComponent->SetWorldLocation(TargetLocation);
				}
				else//圈外
				{
					SoundAssetTagAudioComponent->SetWorldLocation(SplineLocation);
					SoundAssetTagAudioComponent->SetWorldRotation(SplineRotator);
				}
			}
		}
		else
		{
			SoundAssetTagAudioComponent->SetWorldLocation(SplineLocation);
		}
	}
}

