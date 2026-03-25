// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/SoundActor.h"
#include "SoundActorCollision.generated.h"

class UBoxComponent;
class UCapsuleComponent;
class USphereComponent;

/**
 * 声音Actor，该类用于替代直接拖拽到场景的音效
 * 额外多一个碰撞触发逻辑
 */
UCLASS()
class RESOURCESDATATABLECONFIG_API ASoundActorCollision : public ASoundActor
{
	GENERATED_BODY()

public:
	ASoundActorCollision();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void BeginPlay() override;

public:
};
