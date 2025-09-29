#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <ResourcesStructAndEnum.h>
#include "UObject/Interface.h"
#include "../../../../../CommonCompare/Source/CommonCompare/Public/CC_StructAndEnum.h"
#include "SoundEvent.generated.h"



class USoundControlBusMix;
class USoundModulationParameter;

USTRUCT(BlueprintType)
struct FSoundEventAudioModulationInfo : public FTableRowBase
{
	GENERATED_BODY()
public:
	//失活全部
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deactivate", meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bIsDeactivateAllBusMixes = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "!bIsDeactivateAllBusMixes"), Category = "Deactivate")
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

	//声音/BGM资产标记
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FResourceProperty_SoundAssetTag> SoundAssetTag;

	//BGM通道处理-要弹出的通道
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> AllPopBGMChannelName;

	//调制信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoundEventAudioModulationInfo AudioModulationInfo;
};

/*音效事件的对比对照结构体
*/
USTRUCT(BlueprintType)
struct FSoundCompareInfo
{
	GENERATED_BODY()
public:
	//对比对照信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCC_BeCompareInfo BeCompareInfo;

	/*如果比对通过是否要覆盖调制信息以及播放声音信息
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bIsOverrideSoundEventProcess = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsoverrideSoundEventProcess"))
	FSoundEventProcess OverrideSoundEventProcess;
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
	FCC_CompareInfo GetSoundEventCompareParameter(FName SoundEventName);
		virtual FCC_CompareInfo GetSoundEventCompareParameter_Implementation(FName SoundEventName);
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
	FString Tip;
	//最大允许播放(比对通过)的次数，-1表示不限制
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxPlaySoundCount = -1;

	//该事件是否需要有比对信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bIsCompare = false;
	//声音比对信息<Tip,Data>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsCompare"))
	TMap<FString, FSoundCompareInfo> SoundCompareInfo = {{"",FSoundCompareInfo()}};

	//默认音效事件的处理
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoundEventProcess DefaultSoundEventProcess;
};