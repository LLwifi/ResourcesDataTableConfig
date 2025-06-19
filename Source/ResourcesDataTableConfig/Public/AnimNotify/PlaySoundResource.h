// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "WorldSubsystem/SoundSubsystem.h"
#include "PlaySoundResource.generated.h"

/**
 * 
 */
UCLASS()
class RESOURCESDATATABLECONFIG_API UPlaySoundResource : public UAnimNotify
{
	GENERATED_BODY()
public:

	UPlaySoundResource();

	// Begin UAnimNotify interface
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;//UE4 ue5
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override; //UE5
#if WITH_EDITOR
	virtual void ValidateAssociatedAssets() override;

	UFUNCTION()
	TArray<FString> GetSoundRowName();
	UFUNCTION()
	TArray<FString> GetResourceName();
	
#endif
	// End UAnimNotify interface
	
	//USoundBase* GetSound();//ue4
	TObjectPtr<USoundBase> GetSound();//ue5
	TObjectPtr<USoundBase> GetSound(FName SoundRowName, FString SoundResourceNameOrIndex);

	USoundSubsystem* GetSoundSubsystem();

	UFUNCTION(BlueprintPure)
	FString GetResourceNameOrIndex();

	UFUNCTION(BlueprintCallable)
	void SetRowName(FName NewRowName);

	UFUNCTION(BlueprintCallable)
	void SetResourceNameOrIndex(FString NewNameOrIndex);

	UFUNCTION(BlueprintCallable)
	void SetParameter(UAudioComponent* AudioComponent, FName SoundRowName, FString SoundResourceNameOrIndex);

	//同时包含SoundAssetTags 和RowName+NameOrIndex
	UFUNCTION(BlueprintCallable)
	TArray<FResourceProperty_SoundAssetTag> GetAllSoundAssetTags();

	UPROPERTY()
	USoundSubsystem* SoundSubsystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="AnimNotify", meta=(ExposeOnSpawn = true))
	TObjectPtr<USoundBase> Sound;//ue5
	//USoundBase* Sound;//ue4

	UPROPERTY(BlueprintReadWrite, Category = "AnimNotify")
	TArray<UAudioComponent*> SoundComponent;

	// Sound to Play
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AnimNotify", meta=(ExposeOnSpawn = true, GetOptions="GetSoundRowName"))
	FName RowName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AnimNotify", meta=(ExposeOnSpawn = true, GetOptions="GetResourceName"))
	FString ResourceNameOrIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	TArray<FResourceProperty_SoundAssetTag> SoundAssetTags;
	//是否随机播放一个声音，该值为flase时将尝试播放SoundAssetTags中的全部声音资产
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	bool IsRandomPlayOneSound = true;

	//最终真正应用的名称或者下标，当该值为空时，会使用ResourceNameOrIndex的值
	//本质上是多一个可以手动输入的版本
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	FString NameOrIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true, Categories = "Sound"))
	FGameplayTag SoundTag;
	
	// Volume Multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AnimNotify", meta=(ExposeOnSpawn = true))
	float VolumeMultiplier;

	// Pitch Multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AnimNotify", meta=(ExposeOnSpawn = true))
	float PitchMultiplier;

	// If this sound should follow its owner
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	uint32 bFollow:1 = true;

	// Socket or bone name to attach sound to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AnimNotify", meta=(EditCondition="bFollow", ExposeOnSpawn = true))
		FName AttachName;

	// 是否使用动画通知设置的参数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
		bool UseAnimNotifyParameter = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (EditConditionHides, EditCondition = "UseAnimNotifyParameter", ExposeOnSpawn = true))
		FSoundParameters SoundParameters;
	// float参数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (EditConditionHides, EditCondition = "UseAnimNotifyParameter", ExposeOnSpawn = true))
		TMap<FName,float> FloatParameter;
	// SoundWave参数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (EditConditionHides, EditCondition = "UseAnimNotifyParameter", ExposeOnSpawn = true))
		TMap<FName, USoundWave*> WaveParameter;
	// bool参数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (EditConditionHides, EditCondition = "UseAnimNotifyParameter", ExposeOnSpawn = true))
		TMap<FName, bool> BoolParameter;
	// int参数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (EditConditionHides, EditCondition = "UseAnimNotifyParameter", ExposeOnSpawn = true))
		TMap<FName, int32> IntParameter;
};
