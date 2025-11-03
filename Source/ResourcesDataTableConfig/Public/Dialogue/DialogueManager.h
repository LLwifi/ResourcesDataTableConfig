// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DialogueStructAndEnum.h"
#include "DialogueManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(Dialogue, Log, All);

//对话播放状态
UENUM(BlueprintType)
enum class EDialoguePlayState :uint8
{
	Delay = 0 UMETA(DisplayName = "等待播放"),
	StartPlay UMETA(DisplayName = "开始播放"),
	EndPlay UMETA(DisplayName = "结束播放")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDialogueEvent, UDialogueManager*, DialogueManager);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOneDialogueEvent, UDialogueManager*, DialogueManager, int32, DialogueIndex, EDialoguePlayState, PlayState);

/**对话管理
 * 使用时配置一个对话表，然后创建该类进行设置（设置对话内容和对话角色（Actor）），设置完成后调用StartDialogue开始对话
 * bDialogueIsServer : 该对话在客户端还是服务器产生
 * 在服务器产生时：会计算每个音频的播放时长 最终设置一个timer模拟时长 并不会实际的播放音效
 * 在客户端产生时：根据配置播放音效 并且根据音效本身的完成回调进行处理
 */
UCLASS(Blueprintable, BlueprintType, EditInlineNew)
class RESOURCESDATATABLECONFIG_API UDialogueManager : public UObject
{
	GENERATED_BODY()

public:
	/*设置对话角色
	* 这一步的主要目的是让3D声音在对应的Actor位置进行播放
	*/
	UFUNCTION(BlueprintCallable)
	void SetSpeaker(TArray<FDialogueSpeaker> Speakers);

	//设置对话内容
	UFUNCTION(BlueprintCallable)
	void SetSectionDialogueInfo(FSectionDialogueInfo SectionDialogue);

	//通过行名称设置对话内容
	UFUNCTION(BlueprintCallable)
	void SetSectionDialogueInfoFromRowName(FName RowName);

	//改变自动播放的状态
	UFUNCTION(BlueprintCallable)
	void ChangeAutoPlayState(bool IsAutoPlay);

	//开始对话 0-表示从头播放
	UFUNCTION(BlueprintCallable)
	void StartDialogue(int32 StartDialogueIndex = 0);

	//一句对话的完成回调
	UFUNCTION()
	void CurDialogueFinished();

	//一句对话的等待完成回调
	UFUNCTION()
	void CurDialogueDelayFinished();

	//下个对话
	UFUNCTION(BlueprintCallable)
	void NextDialogue();

	//结束整段对话（可以用于提前结束对话）
	UFUNCTION(BlueprintCallable)
	void EndDialogue();

	//结束当前这一句的对话
	UFUNCTION(BlueprintCallable)
	void EndCurOneDialogue();

	UFUNCTION(BlueprintPure)
	USoundSubsystem* GetSoundSubsystem();

	//获取此刻的【音效事件】对比信息
	UFUNCTION(BlueprintPure)
	FCC_CompareInfo GetCurSoundEventCompareInfo();

	UFUNCTION(BlueprintCallable)
	void TriggerDialogueSoundEvent();

	/*当前是否正在播放语音/对话（有声音）
	* 该判断只有在客户端才是准确的，因为服务器不播放音效
	*/
	UFUNCTION(BlueprintPure)
	bool DialogueIsPlaying();

	/*对话是否结束
	* 客户端服务器均能判断
	*/
	UFUNCTION(BlueprintPure)
	bool DialogueIsEnd();
public:
	//全部演讲者
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	TArray<FDialogueSpeaker> AllSpeaker;

	//当前使用的RowName
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DialogueRowName;
	//对话内容
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FSectionDialogueInfo SectionDialogueInfo;

	//当前播放对话的音频组件
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAudioComponent* CurAudioComponent;
	//当前这句对话的信息结构
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FOneDialogueInfo CurOneDialogueInfo;
	UPROPERTY(BlueprintReadWrite)
	FTimerHandle CurDialogueFinishedTimer;
	UPROPERTY(BlueprintReadWrite)
	FTimerHandle CurDialogueDelayFinishedTimer;

	UPROPERTY(BlueprintReadWrite)
	FTimerHandle ServerDialogueFinishedTimer;

	/*对话下标
	* 用来记录当前对话进行到哪一步了
	*/
	UPROPERTY(BlueprintReadWrite)
	int32 DialogueIndex = -1;

	//当前对话的播放状态
	UPROPERTY(BlueprintReadWrite)
	EDialoguePlayState DialoguePlayState;

	//对话是否结束——是否播放完了最后一句对话
	UPROPERTY(BlueprintReadWrite)
	bool bDialogueIsEnd = false;

	/*是否自动播放下一句对话
	* 该值开启后一句对话完成 会自动播放后续对话
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	bool bIsAutoPlayNextDialogue = true;

	/*该对话在客户端还是服务器
	* 如果该Object 的 Outer为Actor，则会通过Outer判断是否在服务器上，否则通过该变量判断
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	bool bDialogueIsServer = false;

	/*作为Mgr对象的标识*/
	UPROPERTY(BlueprintReadOnly)
    int32 MgrID = 0;
	
	UPROPERTY(BlueprintAssignable)
	FDialogueEvent DialogueEnd;

	UPROPERTY(BlueprintAssignable)
	FOneDialogueEvent OneDialogueEvent;

	UPROPERTY(BlueprintReadWrite)
	USoundSubsystem* SoundSubsystem;
	
};
