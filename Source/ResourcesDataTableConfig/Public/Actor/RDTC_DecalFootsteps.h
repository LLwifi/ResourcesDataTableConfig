// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RDTC_DecalFootsteps.generated.h"

UCLASS()
class RESOURCESDATATABLECONFIG_API ARDTC_DecalFootsteps : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARDTC_DecalFootsteps();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void AutoAlignCollision();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UDecalComponent> DecalComponent;

	//贴花材质
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = True))
	UMaterialInterface* DecalMaterialInterface;

	//碰撞物理材质
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = True))
	UPhysicalMaterial* PhysMaterial;

	//是否允许自动对齐碰撞
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = True))
	bool bIsAutoAlign = true;

	//碰撞变换
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = True, EditConditionHides, EditCondition = "!bIsAutoAlign"))
	FTransform CollisionTransform;
};
