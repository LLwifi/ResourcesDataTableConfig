#pragma once

#include "CoreMinimal.h"
#include <Engine/DataTable.h>
#include "ResourcesStructAndEnum.h"
#include <ResourceBPFunctionLibrary.h>
#include <CC_StructAndEnum.h>
#include "DialogueStructAndEnum.generated.h"

/*对话者/单位
* 主要承载着3D语音的发声位置的概念
*/
USTRUCT(BlueprintType)
struct FDialogueSpeaker
{
	GENERATED_BODY()
public:
	//演讲Actor
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* SpeakerActor;
	//演讲Actor
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector SpeakerLocation;
};

/*一句对话/台词信息
* 【谁】在说【什么】
*/
USTRUCT(BlueprintType)
struct FOneDialogueInfo
{
	GENERATED_BODY()
public:
	//获取总时长 等待时长 + 音频时长/持续时长
	float GetTotalDuration()
	{
		float ReturnFloat = DelayTime + Duraction;
		if (Duraction > 0)
		{
			return ReturnFloat;
		}
		FResourceProperty_SoundInfo SoundInfo;
		UResourceBPFunctionLibrary::GetResourceFromString_Sound(SoundAssetTag.RowName, SoundAssetTag.ResourceNameOrIndex, SoundInfo);
		USoundWave* SoundWave = SoundInfo.Parameters.GetSoundWaveFromParameter();
		if (SoundWave)
		{
			return DelayTime + SoundWave->GetDuration();
		}
		return ReturnFloat;
	}
public:
	/*演讲者下标
	* 该值为 -1 或 找不到该下标的角色 或 该下标的角色为空时，该台词进行2D播放
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpeakerIndex = -1;

	//播放这句话之前的等待时间
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DelayTime = 0.0f;
	/*对话持续时长——该值>0时生效
	* 若该值> 0 这句对话的时长为该值，否则以音频时长为准（某些音频文件无时长，例如：MetaSound等）
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duraction = -1.0f;

	/*这句话是对谁说的
	* 后期可以用于控制脑袋朝向
	*/
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//FName RoleSign;

	//要播放的声音/台词
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FResourceProperty_SoundAssetTag SoundAssetTag;

	//是否要重载Modulation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bIsOverrideModulationInfo = false;
	/*这句话的Modulation设置信息
	* 会在这句话播放完后，还原该设置
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsOverrideModulationInfo"))
	FAudioModulationInfo OneDialogueOverrideModulationInfo;

	/*音效事件的比对配置标记
	* 该值会在对话触发【音效事件】时被赋予 触发的音效事件名为：DialogueSoundEvent
	* 需要注意的是：即使什么都不填在运行时也会动态获取以下信息：
	* DialogueName 对话标题 标识 名称 会设置到CompareString中
	* DialoguePlayState 对话播放状态 会设置到CompareString中  （0-等待播放 1-开始播放 2-结束播放）
	* DialogueIndex 对话下标 会设置到CompareFloat中
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCC_CompareInfo DialogueSoundEventCompareInfo;
};

/*一段对话信息
* 【谁】对【谁】在说什么
*/
USTRUCT(BlueprintType)
struct FSectionDialogueInfo: public FTableRowBase
{
	GENERATED_BODY()
public:
	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
	{
		FSectionDialogueInfo* SectionDialogue = InDataTable->FindRow<FSectionDialogueInfo>(InRowName, TEXT(""));
		if (SectionDialogue)
		{
			SectionDialogue->DialogueName = InRowName;
		}
	}

	//根据下标获取一句对话
	bool GetOneDialogueInfoFromIndex(int32 Index, FOneDialogueInfo& OneDialogueInfo)
	{
		if(SectionDialogueInfo.IsValidIndex(Index))
		{
			OneDialogueInfo = SectionDialogueInfo[Index];
			return true;
		}
		return false;
	}
public:
	//对话标题 标识 名称
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FName DialogueName;

	//对话优先级
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DialoguePriority = 0;

	//全部演讲者姓名
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FText> AllSpeakerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FOneDialogueInfo> SectionDialogueInfo;

	//是否要重载Modulation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bIsOverrideModulationInfo = false;
	/*这段对话的Modulation设置信息
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsOverrideModulationInfo"))
	FAudioModulationInfo SectionOverrideDialogueModulationInfo;
};


