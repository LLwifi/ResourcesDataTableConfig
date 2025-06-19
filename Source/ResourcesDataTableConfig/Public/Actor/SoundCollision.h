// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Sound/SoundEvent.h>
#include "Kismet/KismetSystemLibrary.h"
#include "SoundCollision.generated.h"

class UBGMChannel;

//碰撞形状
UENUM(BlueprintType)
enum class ESoundCollisionShape : uint8
{
	None = 0 UMETA(DisplayName = "无"),
	Sphere UMETA(DisplayName = "圆"),
	Capsule UMETA(DisplayName = "胶囊体"),
	Box UMETA(DisplayName = "盒")
};

//碰撞处理的方式
UENUM(BlueprintType)
enum class ESoundActionType : uint8
{
	None = 0 UMETA(DisplayName = "无"),
	Sound2D UMETA(DisplayName = "播放2D声音"),
	Sound3D UMETA(DisplayName = "播放3D声音"),
	PushBGM UMETA(DisplayName = "推送BGM"),
	PopBGMOfChannelName UMETA(DisplayName = "通过名称弹出BGM通道"),
	PopBGMAndStopSound UMETA(DisplayName = "弹出由该碰撞引起的BGM和声音"),
	PopBGM UMETA(DisplayName = "仅弹出由该碰撞引起的BGM"),
	StopSound UMETA(DisplayName = "仅弹出由该碰撞引起的声音"),
	TriggerSoundEvent UMETA(DisplayName = "触发音效事件"),
	LookCheck UMETA(DisplayName = "开始摄像机查看检测"),
	LookCheckStop UMETA(DisplayName = "停止摄像机查看检测")
};

/*
* 声音碰撞的动作
*/
USTRUCT(BlueprintType)
struct FSoundCollisionAction
{
	GENERATED_BODY()

public:
	//进入时的声音动作
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	ESoundActionType SoundActionType = ESoundActionType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true, EditConditionHides, EditCondition = "SoundActionType == ESoundActionType::PushBGM || SoundActionType == ESoundActionType::Sound2D || SoundActionType == ESoundActionType::Sound3D"), Category = "Sound")
	TArray<FResourceProperty_SoundAssetTag> ResourceProperty;

	//在弹出BGM通道时使用
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true, EditConditionHides, EditCondition = "SoundActionType == ESoundActionType::PopBGMOfChannelName"), Category = "Sound")
	TArray<FName> BGMChannelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true, GetOptions = "SoundEventName", EditConditionHides, EditCondition = "SoundActionType == ESoundActionType::TriggerSoundEvent"), Category = "Sound")
	FName SoundEventName;
	//音效事件的相关参数是否动态获取
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true, EditConditionHides, EditCondition = "SoundActionType == ESoundActionType::TriggerSoundEvent"), Category = "Sound")
	bool bCompareParameterIsDynamic = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true, EditConditionHides, EditCondition = "SoundActionType == ESoundActionType::TriggerSoundEvent && !bCompareParameterIsDynamic"), Category = "Sound")
	FSoundCompareParameter CompareParameter;
};

/*声音碰撞
* 当检测到碰撞时触发声音/BGM的播放/音效事件的触发
*/
UCLASS()
class RESOURCESDATATABLECONFIG_API ASoundCollision : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoundCollision();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	UFUNCTION()
	TArray<FString> SoundEventName();
#endif

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent)
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintNativeEvent)
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	void OnEndOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//刷新声音碰撞
	UFUNCTION(BlueprintCallable, CallInEditor)
	void Refresh();

	UFUNCTION(BlueprintCallable)
	void CheckCollision();

	//摄像机查看检测
	UFUNCTION(BlueprintCallable)
	void CameraLookCheckAngle();

	//摄像机查看触发事件——当摄像机和这个Actor的角度小于LookCheckAngle时触发
	UFUNCTION(BlueprintNativeEvent)
	void LookTrigger();
	void LookTrigger_Implementation();

	UFUNCTION(BlueprintCallable)
	void SoundAction(FSoundCollisionAction SoundCollisionAction);

public:
	//开始游戏后主动检测碰撞
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	bool bBeginPlayCheckCollision = false;
	//检测碰撞的等待时间 0.0f表示不等待
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bBeginPlayCheckCollision"), Category = "Collision")
	float CheckCollisionDelayTime = 1.0f;
	//碰撞检测类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bBeginPlayCheckCollision"), Category = "Collision")
	TArray<TEnumAsByte<EObjectTypeQuery>> CollisionObjectTypes = { EObjectTypeQuery::ObjectTypeQuery3 };
	//碰撞检测忽略的对象
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bBeginPlayCheckCollision"), Category = "Collision")
	TArray<AActor*> ActorsToIgnore;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bBeginPlayCheckCollision"), Category = "Collision")
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType;

	//推送BGM时是否要检测该BGM已经存在，如果已经存在则不会再次推送相同的BGM
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	bool bPushBGMCheck = true;

	//进入时的声音动作
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	FSoundCollisionAction SoundCollisionAction_BeginOverlap;

#if WITH_EDITORONLY_DATA
	//显示视线检测相关参数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	bool bShowLookParameter = false;
#endif
	//玩家视野命中的的声音动作
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true, EditConditionHides, EditCondition = "bShowLookParameter"), Category = "Sound")
	FSoundCollisionAction SoundCollisionAction_Look;

	//查看检测的角度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true, EditConditionHides, EditCondition = "bShowLookParameter"), Category = "Sound")
	float LookCheckAngle = 5.0f;
	//查看检测的间隔
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true, EditConditionHides, EditCondition = "bShowLookParameter"), Category = "Sound")
	float LookCheckTime = 0.05f;
	UPROPERTY()
	FTimerHandle LookCheckTimerHandle;
	//能够触发的次数 -1表示可无限触发
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true, EditConditionHides, EditCondition = "bShowLookParameter"), Category = "Sound")
	int32 LookTriggerNum = -1;
	//当前触发的次数
	UPROPERTY()
	int32 CurLookTriggerNum = 0;
	//当前是否已经触发了
	UPROPERTY()
	bool CurLookIsTrigger = false;

	//离开时的声音动作
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	FSoundCollisionAction SoundCollisionAction_EndOverlap;

	UPROPERTY()
	TArray<UBGMChannel*> BGMChannel;
	UPROPERTY()
	TArray<UAudioComponent*> SoundComs;

	//渲染碰撞的厚度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape")
	float LineThickness = 10.0f;
	//渲染碰撞的颜色
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape")
	FColor ShapeColor;

	//碰撞形状
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape")
	ESoundCollisionShape CollisionShape = ESoundCollisionShape::None;

	//碰撞信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape")
	FBodyInstance BodyInstance;

	//根
	UPROPERTY(BlueprintReadOnly, Category = "Shape")
	class USceneComponent* RootScene;
	//当前生效的碰撞
	UPROPERTY(BlueprintReadOnly, Category = "Shape")
	class UShapeComponent* ShapeComponent;

	//盒型碰撞
	UPROPERTY(BlueprintReadOnly, Category = "Shape")
	class UBoxComponent* BoxComponent;
	//圆形碰撞
	UPROPERTY(BlueprintReadOnly, Category = "Shape")
	class USphereComponent* SphereComponent;
	//胶囊体碰撞
	UPROPERTY(BlueprintReadOnly, Category = "Shape")
	class UCapsuleComponent* CapsuleComponent;

	//盒型碰撞尺寸
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditConditionHides, EditCondition = "CollisionShape == ESoundCollisionShape::Box"), Category = "Shape")
	FVector BoxExtent = FVector(32.0f,32.0f,32.0f);

	//圆形碰撞半径
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditConditionHides, EditCondition = "CollisionShape == ESoundCollisionShape::Sphere"), Category = "Shape")
	float SphereRadius = 32.0f;


	//胶囊体高
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditConditionHides, EditCondition = "CollisionShape == ESoundCollisionShape::Capsule"), Category = "Shape")
	float CapsuleHalfHeight = 44.0f;
	//胶囊半径
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditConditionHides, EditCondition = "CollisionShape == ESoundCollisionShape::Capsule"), Category = "Shape")
	float CapsuleRadius = 22.0f;

	//触发重叠的Actor
	UPROPERTY()
	AActor* OverlapActor;
};
