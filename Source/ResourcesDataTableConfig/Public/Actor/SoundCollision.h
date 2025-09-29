// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Sound/SoundEvent.h>
#include "Kismet/KismetSystemLibrary.h"
#include <Kismet/KismetMathLibrary.h>
#include "SoundCollision.generated.h"

class UBGMChannel;

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
	LookCheckStop UMETA(DisplayName = "停止摄像机查看检测"),

	TriggerOtherCollisionCheck UMETA(DisplayName = "触发其他音效碰撞的主动检测"),
	TriggerOtherCollisionBeginOverlap UMETA(DisplayName = "触发其他音效碰撞的进入检测"),
	TriggerOtherCollisionEndOverlap UMETA(DisplayName = "触发其他音效碰撞的离开检测")
};

/*
* 声音碰撞的动作
*/
USTRUCT(BlueprintType)
struct FSoundCollisionAction
{
	GENERATED_BODY()

public:
	//声音动作类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	ESoundActionType SoundActionType = ESoundActionType::None;

	//资源标记
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true, EditConditionHides, EditCondition = "SoundActionType == ESoundActionType::PushBGM || SoundActionType == ESoundActionType::Sound2D || SoundActionType == ESoundActionType::Sound3D"), Category = "Sound")
	TArray<FResourceProperty_SoundAssetTag> ResourceProperty;

	//在弹出BGM通道时使用
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true, EditConditionHides, EditCondition = "SoundActionType == ESoundActionType::PopBGMOfChannelName"), Category = "Sound")
	TArray<FName> BGMChannelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true, GetOptions = "SoundEventName", EditConditionHides, EditCondition = "SoundActionType == ESoundActionType::TriggerSoundEvent"), Category = "Sound")
	FName SoundEventName;
	/*额外提供的音效事件参数
	* 音效事件所需的参数会将该参数和动态获取的参数合并
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "Sound")
	FCC_CompareInfo CompareParameter;

	//链接的其他音效碰撞
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true, EditConditionHides, EditCondition = "SoundActionType == ESoundActionType::TriggerOtherCollisionCheck || SoundActionType == ESoundActionType::TriggerOtherCollisionBeginOverlap || SoundActionType == ESoundActionType::TriggerOtherCollisionEndOverlap"))
	TArray<ASoundCollision*> LinkSoundCollision;
};

/*
* 声音碰撞动作信息
*/
USTRUCT(BlueprintType)
struct FSoundCollisionActionInfo
{
	GENERATED_BODY()

public:
	TArray<FSoundCollisionAction> GetAction()
	{
		
		TArray<FSoundCollisionAction> UseSoundCollisionAction;
		TArray<FSoundCollisionAction> AllAction;
		SoundCollisionAction.GenerateValueArray(AllAction);
		switch (PlaySoundResourceType)
		{
		case EPlaySoundResourceType::ArrayRandom://数组随机
		{
			if (AllAction.Num() > 0)
			{
				UseSoundCollisionAction.Add(AllAction[UKismetMathLibrary::RandomIntegerInRange(0, AllAction.Num() - 1)]);
			}
			break;
		}
		case EPlaySoundResourceType::WeightRandom://权重随机
		{
			int32 MaxValue = 0;
			for (int32 i : PlaySoundResourceIntValue)
			{
				MaxValue += i;
			}
			int32 RandomValue = UKismetMathLibrary::RandomIntegerInRange(0, MaxValue);
			int32 LastValue = 0;
			for (int32 i = 0; i < PlaySoundResourceIntValue.Num(); i++)
			{
				if (RandomValue >= LastValue && RandomValue <= LastValue + PlaySoundResourceIntValue[i])
				{
					LastValue = i;
					break;
				}
				LastValue += PlaySoundResourceIntValue[i];
			}
			UseSoundCollisionAction.Add(AllAction[LastValue]);
			break;
		}
		case EPlaySoundResourceType::ArrayIndexLoop://列表循环
		{
			UseSoundCollisionAction.Add(AllAction[CurPlaySoundResourceIndex]);
			if (++CurPlaySoundResourceIndex >= AllAction.Num())
			{
				CurPlaySoundResourceIndex = 0;
			}
			break;
		}
		case EPlaySoundResourceType::CustomIndexLoop://自定义下标循环
		{
			if (AllAction.IsValidIndex(PlaySoundResourceIntValue[CurPlaySoundResourceIndex]))
			{
				UseSoundCollisionAction.Add(AllAction[PlaySoundResourceIntValue[CurPlaySoundResourceIndex]]);
			}
			if (++CurPlaySoundResourceIndex >= PlaySoundResourceIntValue.Num())
			{
				CurPlaySoundResourceIndex = 0;
			}
			break;
		}
		case EPlaySoundResourceType::SimultaneouslyPlay:
		{
			SoundCollisionAction.GenerateValueArray(UseSoundCollisionAction);
			break;
		}
		default:
			break;
		}

		return UseSoundCollisionAction;
	}

public:
	//声音动作数组
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TMap<FString, FSoundCollisionAction> SoundCollisionAction;
	//播放类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	EPlaySoundResourceType PlaySoundResourceType = EPlaySoundResourceType::ArrayRandom;
	/*该值根据播放类型的不同进行不同的配置
	* 权重随机：表示权重
	* 自定义下标循环：表示下标
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (EditConditionHides, EditCondition = "PlaySoundResourceType != EPlaySoundResourceType::ArrayRandom"))
	TArray<int32> PlaySoundResourceIntValue;
	//当前播放的音效资源下标
	UPROPERTY(BlueprintReadWrite)
	int32 CurPlaySoundResourceIndex = 0;
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

	UFUNCTION(BlueprintCallable)
	void SoundActionArray(TArray<FSoundCollisionAction> SoundCollisionActionArray);

public:
	/*碰撞是否在碰撞一次后失效
	* 该值为true时，在【离开】碰撞触发一次后本碰撞将被设置为NoCollision
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	bool bCollisionOnce = false;
	//开始游戏后主动检测碰撞
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	bool bBeginPlayCheckCollision = false;
	//检测碰撞的等待时间 0.0f表示不等待
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bBeginPlayCheckCollision"), Category = "Collision")
	float CheckCollisionDelayTime = 1.0f;
	//碰撞检测类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	TArray<TEnumAsByte<EObjectTypeQuery>> CollisionObjectTypes = { EObjectTypeQuery::ObjectTypeQuery3 };
	//碰撞检测忽略的对象
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	TArray<AActor*> ActorsToIgnore;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType;
	//仅允许通过的Class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	TArray<TSoftClassPtr<AActor>> OnlyPassClassArray;

	//主动检测的声音动作是否要使用进入或离开时的声音动作
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bCheckSoundActionUseBeginOrEnd = true;
	/*使用进入时的声音动作
	* 该值为false时使用离开时的声音动作
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (EditCondition = "bCheckSoundActionUseBeginOrEnd"))
	bool bUseBeginOverlap = true;
	//主动检测的声音动作
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (EditConditionHides, EditCondition = "!bCheckSoundActionUseBeginOrEnd"))
	FSoundCollisionActionInfo SoundCollisionAction_Check;

	//推送BGM时是否要检测该BGM已经存在，如果已经存在则不会再次推送相同的BGM
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	bool bPushBGMCheck = true;

	//进入时的声音动作
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	FSoundCollisionActionInfo SoundCollisionAction_BeginOverlap;

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
	FSoundCollisionActionInfo SoundCollisionAction_EndOverlap;

	UPROPERTY()
	TArray<UBGMChannel*> BGMChannel;
	UPROPERTY()
	TArray<UAudioComponent*> SoundComs;

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

	//触发重叠的Actor
	UPROPERTY()
	AActor* OverlapActor;
};
