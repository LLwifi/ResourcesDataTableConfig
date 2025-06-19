#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <ResourcesStructAndEnum.h>
#include "UObject/Interface.h"
#include "SoundEvent.generated.h"


class USoundControlBusMix;
class USoundModulationParameter;

USTRUCT(BlueprintType)
struct FSoundEventAudioModulationInfo : public FTableRowBase
{
	GENERATED_BODY()
public:
	//失活全部
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deactivate")
	bool bIsDeactivateAllBusMixes = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "!bIsDeactivateAllBusMixes"), Category = "Deactivate")
	TSoftObjectPtr<USoundControlBusMix> DeactivateMix;

	//调制总线
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activate")
	TSoftObjectPtr<USoundControlBusMix> Mix;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activate")
	FString AddressFilter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activate")
	TSubclassOf<USoundModulationParameter> ParamClassFilter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activate")
	TSoftObjectPtr <USoundModulationParameter> ParamFilter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activate")
	float Value;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activate")
	float FadeTime;
};

//声音事件的处理
USTRUCT(BlueprintType)
struct FSoundEventProcess
{
	GENERATED_BODY()
public:

	//声音资产标记
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FResourceProperty_SoundAssetTag> SoundAssetTag;

	//推送的BGM资产标记
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FResourceProperty_SoundAssetTag> BGMAssetTag;

	//BGM通道处理-要弹出的通道
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> AllPopBGMChannelName;

	//调制信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoundEventAudioModulationInfo AudioModulationInfo;
};

/*音效事件的对比对照结构体
* 该结构体将常见的比较情况进行了汇总
*/
USTRUCT(BlueprintType)
struct FSoundCompareParameter
{
	GENERATED_BODY()
public:
	//对照Tag
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer CompareTag;
	//对照Class 会判断是否等于该类或该类的子类
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UObject> CompareClass;
	/*对照自定义信息 自定义字符比对
	* 某些不至于使用UObject但是Class不足以判断内容时，可以选择使用该内容进行判断
	* 例如：击杀的目标是否携带某种状态也可以通过定于该值进行判断：例如用Fire代表燃烧；Ice代表冰冻
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CompareString;
	//对照自定义信息 自定义数值比对
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CompareNum;

	/*对照Obejct信息
	* 该值用来判断一些更加细致的具体事项（例如复数信息），例如击杀的目标是否携带某种状态，交互的单位身价是否超过某个数值
	*/
	UPROPERTY(BlueprintReadWrite)
	UObject* CompareObject;

	/*如果比对通过是否要覆盖调制信息已经播放声音信息
	* 如果外部有播放的需求，只要有一个判断通过了就结束这次Triiger并且进行外部的处理
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsOverrideSoundEventProcess = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bIsoverrideSoundEventProcess"))
	FSoundEventProcess SoundEventProcess;
};

/*音效事件的对比对照结构体
* 该结构体将常见的比较情况进行了汇总
*/
USTRUCT(BlueprintType)
struct FSoundCompareInfo
{
	GENERATED_BODY()
public:
	//是否使用外部比对类
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsUseOuterCompare = false;

	//外部比对类
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "IsUseOuterCompare"))
	TSoftClassPtr<USoundCompare> OuterCompareClass;

	/*与外部进行tag比对时的决策
	* 该值为Flase时，外部满足任意一个Tag即可
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "!IsUseOuterCompare"), Category = "Tag")
	bool CompareTagIsAllMatch = true;
	/*与外部进行tag比对时的精准决策
	* 该值为Flase时，外部Tag包含父类即可
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "!IsUseOuterCompare"), Category = "Tag")
	bool CompareTagIsExactMatch = true;

	//是否增加数值比对
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "!IsUseOuterCompare"), Category = "Num")
	bool IsCompareNum = false;
	//数值比对是大于比对吗 该值为false时是小于比对
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "!IsUseOuterCompare && IsCompareNum"), Category = "Num")
	bool CompareNumIsGreater = true;
	//数值比对包含等于吗
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "!IsUseOuterCompare && IsCompareNum"), Category = "Num")
	bool CompareNumIsequal = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoundCompareParameter SoundCompareParameter;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USoundEventInteract : public UInterface
{
	GENERATED_BODY()
};

/**
 * 音效事件接口
 */
class RESOURCESDATATABLECONFIG_API ISoundEventInteract
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/*获取音效事件的对比参数
	* SoundCollision的碰撞触发事件时若bCompareParameterIsDynamic_BeginOverlap 为true 会调用该函数获取比对参数
	*/
	UFUNCTION(BlueprintNativeEvent)
	FSoundCompareParameter GetSoundEventCompareParameter(FName SoundEventName);
		virtual FSoundCompareParameter GetSoundEventCompareParameter_Implementation(FName SoundEventName);
};


/**
 * 任务对照/比较基类类
 * 过于定制化的对比需求使用该类进行实现
 */
UCLASS(Blueprintable, BlueprintType)
class USoundCompare : public UObject
{
	GENERATED_BODY()

public:
	//比较结果
	UFUNCTION(BlueprintNativeEvent)
	bool CompareResult(FSoundCompareInfo ThisCompareInfo, FSoundCompareParameter CompareParameter);
	virtual bool CompareResult_Implementation(FSoundCompareInfo ThisCompareInfo, FSoundCompareParameter CompareParameter);
};

USTRUCT(BlueprintType)
struct FSoundEventInfo: public FTableRowBase
{
	GENERATED_BODY()
public:
	//尝试通过属性修改行名称
	//virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
	//{
	//	FSoundEventInfo* SoundEventInfo = InDataTable->FindRow<FSoundEventInfo>(InRowName, TEXT(""));
	//	if (SoundEventTag.ToString() != InRowName.ToString())
	//	{
	//		InDataTable->RemoveRow(InRowName);
	//	}
	//	TMap<FName, uint8*>& DTMap = InDataTable->GetAllRows();
	//	if (SoundEventInfo)
	//	{
	//		//
	//	}
	//}

public:
	//事件提示/备注文本
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString EventTip;
	//该事件是否需要有比对信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsCompare = false;
	//最大允许播放(比对通过)的次数，-1表示不限制
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bIsCompare"))
	int32 MaxPlaySoundCount = -1;
	//声音比对信息<Tip,Data>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bIsCompare"))
	TMap<FString, FSoundCompareInfo> SoundCompareInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoundEventProcess SoundEventProcess;
};