// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <ResourcesStructAndEnum.h>
#include "SoundLine.generated.h"

class ASoundActor;

//声音线
UCLASS()
class RESOURCESDATATABLECONFIG_API ASoundLine : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoundLine();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Destroyed() override;

	//刷新创建声音
	UFUNCTION(BlueprintCallable, CallInEditor)
	void Refresh();
	//删除声音
	UFUNCTION(BlueprintCallable)
	void Delete();

public:
	//要在线段上平均创建的声音数量
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SoundNum = 3;
	//声音资产标记
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FResourceProperty_SoundAssetTag SoundAssetTag;

	//创建出来的声音
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<ASoundActor*> SoundActors;

	//统一设置声音的衰减
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USoundAttenuation> AttenuationSettings;

	//声音线
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USplineComponent* SoundSpline;
};
