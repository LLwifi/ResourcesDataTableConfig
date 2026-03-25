// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/AmbientSound.h"
#include <ResourcesStructAndEnum.h>
#include "SoundActor.generated.h"

class USoundAssetTagAudioComponent;
class UBoxComponent;
class UCapsuleComponent;
class USphereComponent;

/**
 * 声音Actor，该类用于替代直接拖拽到场景的音效
 */
UCLASS()
class RESOURCESDATATABLECONFIG_API ASoundActor : public AActor
{
	GENERATED_BODY()

public:
	ASoundActor();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void BeginPlay() override;

	//激活声音组件
	UFUNCTION(BlueprintCallable)
	void ActiveSoundComponent();

	UFUNCTION()
	void OnBeginOverlapCheck(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlapCheck(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintNativeEvent)
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintNativeEvent)
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	void OnEndOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


public:
	UPROPERTY(BlueprintReadWrite)
	USceneComponent* Root;

	/*激活时间
	* <0 默认不激活
	* =0 自动激活
	* >0 在延迟的指定时长后激活
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AutoActiveTime = 0.0f;
	UPROPERTY()
	FTimerHandle AutoActiveTimeTimerHandler;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundAssetTagAudioComponent* SoundAssetTagAudioComponent;

	/*激活时间
	* <0 默认不激活
	* =0 自动激活
	* >0 在延迟的指定时长后激活
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CollisionAutoActiveTime = 0.0f;
	UPROPERTY()
	FTimerHandle CollisionAutoActiveTimeTimerHandler;

	//当前生效的碰撞
	UPROPERTY(BlueprintReadOnly)
	TArray<UShapeComponent*> AllShapeComponent;
	//盒型碰撞
	UPROPERTY(BlueprintReadOnly, Category = "Shape")
	TArray<UBoxComponent*> AllBoxComponent;
	//圆形碰撞
	UPROPERTY(BlueprintReadOnly, Category = "Shape")
	TArray<USphereComponent*> AllSphereComponent;
	//胶囊体碰撞
	UPROPERTY(BlueprintReadOnly, Category = "Shape")
	TArray<UCapsuleComponent*> AllCapsuleComponent;

	//圆形碰撞
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	class USphereComponent* SphereComponent;

	//触发重叠的Actor
	UPROPERTY()
	TArray<AActor*> OverlapAllActor;

	//触发进入重叠事件的次数
	UPROPERTY(BlueprintReadWrite)
	TMap<AActor*, FSoundCollisionOverlapCheck> OverlapCheck;
};
