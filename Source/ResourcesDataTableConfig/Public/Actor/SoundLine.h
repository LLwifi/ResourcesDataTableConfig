// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <ResourcesStructAndEnum.h>
#include "SoundLine.generated.h"

/*声音线
* 声音线会不断检测线段距离玩家最近的位置，从而设置音频的位置
* 可以适配河流，道路，将Spline沿着路线扩展
* 可以适配一片有声音的区域，将Spline围成一个闭合图形，将bIsRange 配置为true，此时需要配置bIsClockwise参数
*/
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

	//间隔检测回调
	UFUNCTION(BlueprintPure)
	AActor* GetTargetActor();

	//间隔检测回调
	UFUNCTION(BlueprintCallable)
	void CheckIntervalBack();

public:
	//声音线
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USplineComponent* SoundSpline;

	//声音组件
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundAssetTagAudioComponent* SoundAssetTagAudioComponent;

	UPROPERTY()
	FTimerHandle CheckIntervalTimerHandler;

	//用于检测的目标Actor，通常是玩家
	UPROPERTY()
	AActor* TargetActor;

	//检测玩家位置的时间间隔
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CheckInterval = 0.2;

	//是否是一个范围，当操作Spline围成一个闭合图形（圈/方）时将该值配置为true并且配置bIsClockwise
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsRange = false;
	/*如果根据Spline围成了一个闭合图形（圈/方），那么需要设定该圈是顺时针还是逆时针
	* 这个配置关乎到逻辑判断，该值为true表示顺时针，否则表示逆时针
	* 如果玩家在圈外，那么会设置音效位置到线段上最近的一个点
	* 如果玩家在圈内，会设置音效在玩家脚底下
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditConditionHides, EditCondition = "bIsRange"))
	bool bIsClockwise = true;
};
