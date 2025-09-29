// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DialogueStructAndEnum.h"
#include "DialogueManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(Dialogue, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDialogueEvent, UDialogueManager*, DialogueManager);

/**对话管理
 * 使用时配置一个对话表，然后创建该类进行设置（设置对话内容和对话角色（Actor）），设置完成后调用StartDialogue开始对话
 */
UCLASS(Blueprintable, BlueprintType, EditInlineNew)
class RESOURCESDATATABLECONFIG_API UDialogueManager : public UObject
{
	GENERATED_BODY()

public:
	//设置对话角色
	UFUNCTION(BlueprintCallable)
	void SetSpeaker(TArray<AActor*> Speakers);

	//设置对话内容
	UFUNCTION(BlueprintCallable)
	void SetSectionDialogueInfo(FSectionDialogueInfo SectionDialogue);

	//通过行名称设置对话内容
	UFUNCTION(BlueprintCallable)
	void SetSectionDialogueInfoFromRowName(FName RowName);

	//改变自动播放的状态
	UFUNCTION(BlueprintCallable)
	void ChangeAutoPlayState(bool IsAutoPlay);

	//开始对话
	UFUNCTION(BlueprintCallable)
	void StartDialogue();

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
	
public:
	//全部演讲者
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	TArray<AActor*> AllSpeaker;

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

	UPROPERTY(BlueprintAssignable)
	FDialogueEvent DialogueEnd;
};
