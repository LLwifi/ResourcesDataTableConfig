#pragma once

#include "CoreMinimal.h"
#include <Engine/DataTable.h>
#include "GameplayTagContainer.h"
//#include "SoundEnvironmentConfig.generated.h"

////切换BGM的处理方式
//UENUM(BlueprintType)
//enum class ESwitchBMGType :uint8
//{
//	Fade = 0 UMETA(DisplayName = "淡入淡出"),
//	BPM UMETA(DisplayName = "BPM速度切换"),
//	BPM_Fade UMETA(DisplayName = "淡入淡出+BPM速度切换"),
//	Switch UMETA(DisplayName = "直接切换")
//};
//
////背景音乐的信息
//USTRUCT(BlueprintType)
//struct FBGMInfo: public FTableRowBase
//{
//	GENERATED_BODY()
//public:
//	//该BGM要推向的通道名称
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Channel")
//	FName BGMChannelName = "Main";
//	//推送该BGM时影响的其他通道音量（相乘）
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Channel")
//		TMap<FName, float> OtherChannelVolume;
//
//	//播放类型
//	UPROPERTY(EditAnywhere,BlueprintReadWrite)
//	ESwitchBMGType SwitchBMGType;
//	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Sound")
//	TSoftObjectPtr<USoundBase> SoundBase;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	FSoundParameters Parameters;
//	// float参数
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
//		TMap<FName, float> FloatParameter;
//	// SoundWave参数
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
//		TMap<FName, USoundWave*> WaveParameter;
//	// bool参数
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
//		TMap<FName, bool> BoolParameter;
//	// int参数
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
//		TMap<FName, int32> IntParameter;
//
//	//每分钟节拍数
//	UPROPERTY(EditAnywhere,BlueprintReadWrite)
//	float BPM;
//	//切换的绝对拍子数
//	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta=(ClampMin=1,EditCondition = "SwitchBMGType == ESwitchBMGType::BPM || SwitchBMGType == ESwitchBMGType::BPM_Fade"))
//	int32 AbsoluteBeatNum = 1;
//	//淡入的相对拍子数(该值范围在1~AbsoluteBeatNum，并且只有SwitchBMGType = 淡入淡出+BPM速度切换时才生效)
//	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta=(ClampMin=1,EditCondition = "SwitchBMGType == ESwitchBMGType::BPM_Fade"))
//	int32 FadeInOutBeatNum = 1;
//
//	//背景音乐淡出时间
//	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition = "SwitchBMGType == ESwitchBMGType::Fade"))
//	float BGMFadeOutTime = UResourcesConfig::GetInstance()->BGMFadeOutTime_2D;
//	//背景音乐淡入时间
//	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition = "SwitchBMGType == ESwitchBMGType::Fade"))
//	float BGMFadeInTime = UResourcesConfig::GetInstance()->BGMFadeInTime_2D;
//	//淡入淡出的等待时间混合比例。根据通过上一段音乐混出时间换算
//	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 0, ClampMax = 1,EditCondition = "SwitchBMGType == ESwitchBMGType::Fade"))
//	float DelayTimeScale = 0.5f;
//
//	//该环境下要设置的混合
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<USoundMix*> SetSoundMix;
//};
//
//USTRUCT(BlueprintType)
//struct FResourceProperty_BGM: public FTableRowBase
//{
//	GENERATED_BODY()
//public:
//	FResourceProperty_BGM(){}
//	FResourceProperty_BGM(TMap<FString, FBGMInfo> _BGM){BGM = _BGM;}
//	bool Contains(FString Name) { return BGM.Contains(Name); };
//	FBGMInfo operator[](FString Name) { return Contains(Name) ? BGM[Name] : FBGMInfo(); };
//	FBGMInfo operator[](int32 Index)
//	{
//		TArray<FBGMInfo> Value;
//		BGM.GenerateValueArray(Value);
//		return Value.IsValidIndex(Index) ? Value[Index] : FBGMInfo();
//	};
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	TMap<FString, FBGMInfo> BGM;
//};