// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include <ResourceBPFunctionLibrary.h>
#include "PlaySoundResourceState.generated.h"

class USoundSubsystem;

/**
 * 播放音效资产的State版本
 */
UCLASS()
class RESOURCESDATATABLECONFIG_API UPlaySoundResourceState : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	// Begin UAnimNotify interface
	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference);
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference);
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);

	UFUNCTION()
	USoundSubsystem* GetSoundSubsystem();

	TObjectPtr<USoundBase> GetSound(FName SoundRowName, FString SoundResourceNameOrIndex);

	UFUNCTION(BlueprintCallable)
	void SetParameter(UAudioComponent* AudioComponent, FName SoundRowName, FString SoundResourceNameOrIndex);

	UFUNCTION()
	void Play(USkeletalMeshComponent* MeshComp);

	UFUNCTION()
	void EndPlay();

public:
	//是否随机播放一个声音，该值为flase时将尝试播放SoundAssetTags中的全部声音资产
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	bool IsRandomPlayOneSound = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	TArray<FResourceProperty_SoundAssetTag> SoundAssetTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true, Categories = "Sound"))
	FGameplayTag SoundTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	float VolumeMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	float PitchMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	uint32 bFollow : 1 = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bFollow", ExposeOnSpawn = true))
	FName AttachName;

	// 是否使用动画通知设置的参数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	bool bUseAnimNotifyParameter = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bUseAnimNotifyParameter", ExposeOnSpawn = true))
	FSoundParameters SoundParameters;

	//是否要使用Actor的速度检查
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	bool bUseActorVelocityCheck = false;
	/*速度的判断方式
	* 该值为true为大于判断
	* 该值为false为小于判断
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bUseActorVelocityCheck", ExposeOnSpawn = true))
	bool bVelocityGreaterThan = true;
	//声音播放是否要被“速度”限制
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bUseActorVelocityCheck", ExposeOnSpawn = true))
	float ActorVelocity = 0.0f;

	//Tick中的音效是否已经开始播放了
	UPROPERTY(BlueprintReadWrite)
	bool bIsPlay_Tick = false;

	//是否开启音效渐入
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	bool bIsFadeIn = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bIsFadeIn", ExposeOnSpawn = true))
	float FadeInTime = UResourcesConfig::GetInstance()->BGMFadeInTime_2D;
	//是否开启音效渐出
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	bool bIsFadeOut = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bIsFadeOut", ExposeOnSpawn = true))
	float FadeOutTime = UResourcesConfig::GetInstance()->BGMFadeOutTime_2D;;

	UPROPERTY(BlueprintReadWrite)
	AActor* AnimOwner;
	UPROPERTY(BlueprintReadWrite)
	UWorld* World;

	UPROPERTY()
	USoundSubsystem* SoundSubsystem;

	UPROPERTY(BlueprintReadWrite, Category = "AnimNotify")
	TArray<UAudioComponent*> SoundComponent;
};
