// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/SoundActorCollision.h"
#include "Components/SphereComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"

ASoundActorCollision::ASoundActorCollision()
{
}

#if WITH_EDITOR
void ASoundActorCollision::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	auto Property = PropertyChangedEvent.Property;//拿到改变的属性
}
#endif

void ASoundActorCollision::BeginPlay()
{
	Super::BeginPlay();
}