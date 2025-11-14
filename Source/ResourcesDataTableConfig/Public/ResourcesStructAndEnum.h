#pragma once

#include "CoreMinimal.h"
#include <Engine/DataTable.h>
#include "ResourcesConfig.h"
#include <Sound/SoundEnvironmentConfig.h>
#include "ResourcesStructAndEnum.generated.h"

USTRUCT(BlueprintType)
struct FResourceTypeInfo: public FTableRowBase
{
	GENERATED_BODY()
public:
	FResourceTypeInfo(){}
	FResourceTypeInfo(EResourceType _Type,FString _Name)
	{
		Type = _Type;
		Name = _Name;
	}
	FResourceTypeInfo(EResourceType _Type,FString _Name,FText _InitValue)
	{
		Type = _Type;
		Name = _Name;
		InitValue = _InitValue;
	}

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		EResourceType Type = EResourceType::Text;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		FString Name;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		FText InitValue;
};

USTRUCT(BlueprintType)
struct FResourceTypeGroup: public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		TArray<FResourceTypeInfo> PropertyGroup = {FResourceTypeInfo(EResourceType::Text,TEXT("名称")),
FResourceTypeInfo(EResourceType::Text,TEXT("描述")),FResourceTypeInfo(EResourceType::Texture2D,TEXT("图片"))};
};

USTRUCT(BlueprintType)
struct FResourceProperty_Bool: public FTableRowBase
{
	GENERATED_BODY()
public:
	FResourceProperty_Bool(){}
	FResourceProperty_Bool(TMap<FString, bool> _Bool){Bool = _Bool;}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, bool> Bool;
};

USTRUCT(BlueprintType)
struct FResourceProperty_Int32: public FTableRowBase
{
	GENERATED_BODY()
public:
	FResourceProperty_Int32(){}
	FResourceProperty_Int32(TMap<FString, int32> _Int32){Int32 = _Int32;}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, int32> Int32;
};

USTRUCT(BlueprintType)
struct FResourceProperty_Float: public FTableRowBase
{
	GENERATED_BODY()
public:
	FResourceProperty_Float(){}
	FResourceProperty_Float(TMap<FString, float> _Float){Float = _Float;}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, float> Float;
};

USTRUCT(BlueprintType)
struct FResourceProperty_Name: public FTableRowBase
{
	GENERATED_BODY()
public:
	FResourceProperty_Name(){}
	FResourceProperty_Name(TMap<FString, FName> _Name){Name = _Name;}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FName> Name;
};

USTRUCT(BlueprintType)
struct FResourceProperty_String: public FTableRowBase
{
	GENERATED_BODY()
public:
	FResourceProperty_String(){}
	FResourceProperty_String(TMap<FString, FString> _String){String = _String;}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FString> String;
};

USTRUCT(BlueprintType)
struct FResourceProperty_Text: public FTableRowBase
{
	GENERATED_BODY()
public:
	FResourceProperty_Text(){}
	FResourceProperty_Text(TMap<FString, FText> _Text){Text = _Text;}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, FText> Text;
};

USTRUCT(BlueprintType)
struct FResourceProperty_Vector: public FTableRowBase
{
	GENERATED_BODY()
public:
	FResourceProperty_Vector(){}
	FResourceProperty_Vector(TMap<FString, FVector> _Vector){Vector = _Vector;}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FVector> Vector;
};

USTRUCT(BlueprintType)
struct FResourceProperty_Rotator: public FTableRowBase
{
	GENERATED_BODY()
public:
	FResourceProperty_Rotator(){}
	FResourceProperty_Rotator(TMap<FString, FRotator> _Rotator){Rotator = _Rotator;}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FRotator> Rotator;
};

USTRUCT(BlueprintType)
struct FResourceProperty_Texture2D: public FTableRowBase
{
	GENERATED_BODY()
public:
	FResourceProperty_Texture2D(){}
	FResourceProperty_Texture2D(TMap<FString, TSoftObjectPtr<UTexture2D>> _Texture2D){Texture2D = _Texture2D;}
	bool Contains(FString Name) { return Texture2D.Contains(Name); };
	TSoftObjectPtr<UTexture2D> operator[](FString Name) { return Contains(Name) ? Texture2D[Name] : nullptr; };
	TSoftObjectPtr<UTexture2D> operator[](int32 Index)
	{
		TArray<TSoftObjectPtr<UTexture2D>> Value;
		Texture2D.GenerateValueArray(Value);
		return Value.IsValidIndex(Index) ? Value[Index] : nullptr;
	};
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, TSoftObjectPtr<UTexture2D>> Texture2D;
};

USTRUCT(BlueprintType)
struct FResourceProperty_Class: public FTableRowBase
{
	GENERATED_BODY()
public:
	FResourceProperty_Class(){}
	FResourceProperty_Class(TMap<FString, TSoftClassPtr<UObject>> _Class){Class = _Class;}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, TSoftClassPtr<UObject>> Class;
};


USTRUCT(BlueprintType)
struct FResourceProperty_Transform: public FTableRowBase
{
	GENERATED_BODY()
public:
	FResourceProperty_Transform(){}
	FResourceProperty_Transform(TMap<FString, FTransform> _Transform){Transform = _Transform;}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FTransform> Transform;
};


USTRUCT(BlueprintType)
struct FSoundParameter_FloatArray : public FTableRowBase
{
	GENERATED_BODY()
public:
	// float参数
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> FloatArray;
};

//因为AudioParameter中TObjectPtr<UObject>相关的参数数据表中看不到，所以这里包裹一层用TSoftObjectPtr<UObject>代替
USTRUCT(BlueprintType)
struct FMyAudioParameter : public FTableRowBase
{
	GENERATED_BODY()

public:
	bool operator==(const FMyAudioParameter& other) const {
		if (AudioParameter.ParamName != other.AudioParameter.ParamName)
		{
			return false;
		}

		switch (AudioParameter.ParamType)
		{
		case EAudioParameterType::None:
		{
			break;
		}
		case EAudioParameterType::Boolean:
		{
			if (AudioParameter.BoolParam != other.AudioParameter.BoolParam)
			{
				return false;
			}
		}
		case EAudioParameterType::Integer:
		{
			if (AudioParameter.IntParam != other.AudioParameter.IntParam)
			{
				return false;
			}
		}
		case EAudioParameterType::Float:
		{
			if (AudioParameter.FloatParam != other.AudioParameter.FloatParam)
			{
				return false;
			}
		}
		case EAudioParameterType::String:
		{
			if (AudioParameter.StringParam != other.AudioParameter.StringParam)
			{
				return false;
			}
		}
		case EAudioParameterType::Object:
		{
			if (AudioParameter.ObjectParam != other.AudioParameter.ObjectParam)
			{
				return false;
			}
		}
		case EAudioParameterType::NoneArray:
		{
			break;
		}
		case EAudioParameterType::BooleanArray:
		{
			if (AudioParameter.ArrayBoolParam != other.AudioParameter.ArrayBoolParam)
			{
				return false;
			}
		}
		case EAudioParameterType::IntegerArray:
		{
			if (AudioParameter.ArrayIntParam != other.AudioParameter.ArrayIntParam)
			{
				return false;
			}
		}
		case EAudioParameterType::FloatArray:
		{
			if (AudioParameter.ArrayFloatParam != other.AudioParameter.ArrayFloatParam)
			{
				return false;
			}
		}
		case EAudioParameterType::StringArray:
		{
			if (AudioParameter.ArrayStringParam != other.AudioParameter.ArrayStringParam)
			{
				return false;
			}
		}
		case EAudioParameterType::ObjectArray:
		{
			if (AudioParameter.ArrayObjectParam != other.AudioParameter.ArrayObjectParam)
			{
				return false;
			}
		}
		case EAudioParameterType::Trigger:
		{
			break;
		}
		case EAudioParameterType::COUNT:
		{
			break;
		}
		default:
			break;
		}

		if (MyObjectParam != other.MyObjectParam)
		{
			return false;
		}
		if (MyArrayObjectParam != other.MyArrayObjectParam)
		{
			return false;
		}
		return true;
	}

	FAudioParameter GetAudioParameter()
	{
		//给两个Object类型进行赋值
		if (AudioParameter.ParamType == EAudioParameterType::Object && !AudioParameter.ObjectParam)
		{
			AudioParameter.ObjectParam = MyObjectParam.LoadSynchronous();
		}
		else if (AudioParameter.ParamType == EAudioParameterType::ObjectArray && AudioParameter.ArrayObjectParam.Num() == 0)
		{
			for (TSoftObjectPtr<UObject>& Obj : MyArrayObjectParam)
			{
				TObjectPtr<UObject> ObjTemp = Obj.LoadSynchronous();
				if (IsValid(ObjTemp))
				{
					AudioParameter.ArrayObjectParam.Add(ObjTemp);
				}
			}
		}

		return AudioParameter;
	}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FAudioParameter AudioParameter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<UObject> MyObjectParam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<TSoftObjectPtr<UObject>> MyArrayObjectParam;
};

USTRUCT(BlueprintType)
struct FMyAudioParameterMap : public FTableRowBase
{
	GENERATED_BODY()
public:
	bool operator==(const FMyAudioParameterMap& other) const {
		TArray<FName> Keys;
		TArray<FMyAudioParameter> Values;
		other.Name_Parameter.GenerateKeyArray(Keys);
		other.Name_Parameter.GenerateValueArray(Values);
		int32 i = 0;
		for (TPair<FName, FMyAudioParameter> pair : Name_Parameter)
		{
			if (pair.Key != Keys[i] || pair.Value != Values[i++])
			{
				return false;
			}
		}
		return true;
	}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FMyAudioParameter> Name_Parameter;
};

//动态设置 SoundCue参数/MetaSound输入参数 的结构体
USTRUCT(BlueprintType)
struct FSoundParameters : public FTableRowBase
{
	GENERATED_BODY()

public:
	void SetAudioParameterParamNameOfMapKey()
	{
		for (TPair<FName, FMyAudioParameter>& pair : MyAudioParameter)
		{
			pair.Value.AudioParameter.ParamName = pair.Key;
		}


		for (TPair<FName, FMyAudioParameterMap>& pair: CustomAudioParameter)
		{
			for (TPair<FName, FMyAudioParameter>& pair2 : pair.Value.Name_Parameter)
			{
				pair2.Value.AudioParameter.ParamName = pair2.Key;
			}
		}
	}

	TArray<FAudioParameter> GetAllAudioParameter()
	{
		TArray<FAudioParameter> AudioParameter;
		for (TPair< FName, FMyAudioParameter >& pair : MyAudioParameter)
		{
			if (pair.Value.AudioParameter.ParamName.IsNone())
			{
				pair.Value.AudioParameter.ParamName = pair.Key;
			}
			AudioParameter.Add(pair.Value.GetAudioParameter());
		}

		for (TPair< FName, FMyAudioParameterMap >& pairMap : CustomAudioParameter)
		{
			TArray<FMyAudioParameter> AudioParameters;
			pairMap.Value.Name_Parameter.GenerateValueArray(AudioParameters);

			for (TPair< FName, FMyAudioParameter >& pair : pairMap.Value.Name_Parameter)
			{
				if (pair.Value.AudioParameter.ParamName.IsNone())
				{
					pair.Value.AudioParameter.ParamName = pair.Key;
				}
				AudioParameter.Add(pair.Value.GetAudioParameter());
			}
		}
		return AudioParameter;
	}

	//获取参数中的第一个SoundWave
	USoundWave* GetSoundWaveFromParameter()
	{
		for (FAudioParameter& AudioParameter : GetAllAudioParameter())
		{
			USoundWave* SoundWave = Cast<USoundWave>(AudioParameter.ObjectParam);
			if (SoundWave)
			{
				return SoundWave;
			}

			for (TObjectPtr<UObject>& Obj : AudioParameter.ArrayObjectParam)
			{
				SoundWave = Cast<USoundWave>(Obj);
				if (SoundWave)
				{
					return SoundWave;
				}
			}
		}
		return nullptr;
	}

public:
	//默认音效参数配置<影响的参数名称，值>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FMyAudioParameter> MyAudioParameter;//DefaultAudioParameter;

	//自定义分类音效参数配置<自定义分组名称，影响的参数组>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FMyAudioParameterMap> CustomAudioParameter;
};

//声音字幕
USTRUCT(BlueprintType)
struct FRDTC_SoundSubtitle
{
	GENERATED_BODY()
public:
	FRDTC_SoundSubtitle() {}
	FRDTC_SoundSubtitle(bool IsDisplaySubtitle, bool IsUseSoundWaveSubtitleCue, TArray<FSubtitleCue> Subtitle)
	{
		bIsDisplaySubtitle = IsDisplaySubtitle;
		bIsUseSoundWaveSubtitleCue = IsUseSoundWaveSubtitleCue;
		SubtitleCue = Subtitle;
	}

public:
	//是否开启字幕功能
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDisplaySubtitle = false;
	//是否使用SoundWave音源的字幕配置
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "IsDisplaySubtitle"))
	bool bIsUseSoundWaveSubtitleCue = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "IsDisplaySubtitle"))
	TArray<FSubtitleCue> SubtitleCue;
};

//声音信息
USTRUCT(BlueprintType)
struct FResourceProperty_SoundInfo : public FTableRowBase
{
	GENERATED_BODY()
public:
	FResourceProperty_SoundInfo() {}
	FResourceProperty_SoundInfo(TSoftObjectPtr<USoundBase> _Sound) { Sound = _Sound; }

public:
	//默认音效
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<USoundBase> Sound;

	//是否要重载音效的并发性设置 后续扩展成数组
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundConcurrency> SoundConcurrency;

	//是否要重载音效的衰减
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundAttenuation> SoundAttenuation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FSoundParameters Parameters;

	//是否开启字幕功能
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsDisplaySubtitle = false;
	//是否使用SoundWave音源的字幕配置
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "IsDisplaySubtitle"))
		bool IsUseSoundWaveSubtitleCue = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "IsDisplaySubtitle"))
		TArray<FSubtitleCue> SubtitleCue;

	//相关资源
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UObject> ReferenceObject;
};

USTRUCT(BlueprintType)
struct FResourceProperty_Sound: public FTableRowBase
{
	GENERATED_BODY()
public:
	FResourceProperty_Sound(){}
	FResourceProperty_Sound(TMap<FString, FResourceProperty_SoundInfo> _Sound){Sound = _Sound;}
	bool Contains(FString Name) { return Sound.Contains(Name); };
	FResourceProperty_SoundInfo operator[](FString Name) { return Contains(Name) ? Sound[Name] : FResourceProperty_SoundInfo(); };
	FResourceProperty_SoundInfo operator[](int32 Index)
	{
		TArray<FResourceProperty_SoundInfo> Value;
		Sound.GenerateValueArray(Value);
		return Value.IsValidIndex(Index) ? Value[Index] : FResourceProperty_SoundInfo();
	};

	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
	{
		FResourceProperty_Sound* ResourceProperty_Sound = InDataTable->FindRow<FResourceProperty_Sound>(InRowName, TEXT(""));
		if (ResourceProperty_Sound)
		{
			for (TPair<FString, FResourceProperty_SoundInfo>& pair : ResourceProperty_Sound->Sound)
			{
				if (pair.Value.Parameters.MyAudioParameter.Num() == 0)
				{
					pair.Value.Parameters.MyAudioParameter = DefaultParameters.MyAudioParameter;
				}
				if (pair.Value.Parameters.CustomAudioParameter.Num() == 0)
				{
					pair.Value.Parameters.CustomAudioParameter = DefaultParameters.CustomAudioParameter;
				}

				pair.Value.Parameters.SetAudioParameterParamNameOfMapKey();
			}
		}
		DefaultParameters.SetAudioParameterParamNameOfMapKey();
	}
public:
	//配置用备注
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Tip;

	//相关资源
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSoftObjectPtr<UObject>> ReferencesObjects;

	//默认参数结构
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoundParameters DefaultParameters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FResourceProperty_SoundInfo> Sound = {{"Default",FResourceProperty_SoundInfo()}};
};

//声音资源的类型
UENUM(BlueprintType)
enum class ESoundAssetType : uint8
{
	Sound UMETA(DisplayName = "音效"),
	BGM UMETA(DisplayName = "背景音乐")
};

//声音资产标记
USTRUCT(BlueprintType)
struct FResourceProperty_SoundAssetTag : public FTableRowBase
{
	GENERATED_BODY()
public:
	FResourceProperty_SoundAssetTag(){}
	FResourceProperty_SoundAssetTag(FName _RowName, FString _ResourceNameOrIndex)
	{
		RowName = _RowName;
		ResourceNameOrIndex = _ResourceNameOrIndex;
	}
public:
	//声音资产的类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESoundAssetType SoundAssetType = ESoundAssetType::Sound;
	//所属行名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RowName;
	//资产名称或下标
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ResourceNameOrIndex;

#if WITH_EDITORONLY_DATA
	//行名称对应的数据表
	UPROPERTY(VisibleAnywhere)
	TSoftObjectPtr<UDataTable> DataTable;

	//对应的音效资源
	UPROPERTY(VisibleAnywhere)
	TSoftObjectPtr<USoundBase> SoundBase;

	//设置的引用资源
	UPROPERTY(VisibleAnywhere)
	TSoftObjectPtr<UObject> ReferenceObject;
#endif
};

//改变BGM通道的类型
UENUM(BlueprintType)
enum class EChangeBGMChanelType :uint8
{
	Volume = 0 UMETA(DisplayName = "修改音量（相乘）"),
	Paused UMETA(DisplayName = "暂停"),
	PopUp UMETA(DisplayName = "弹出")
};

//改变BGM通道信息
USTRUCT(BlueprintType)
struct FChangeBGMChanelInfo
{
	GENERATED_BODY()
public:
	//改变BGM通道的类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EChangeBGMChanelType ChangeBGMChanelType;

	//影响的其他通道的音量（相乘）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "ChangeBGMChanelType == EChangeBGMChanelType::Volume"))
	float Volume = 1.0f;
};

//切换BGM的处理方式
UENUM(BlueprintType)
enum class ESwitchBMGType :uint8
{
	Fade = 0 UMETA(DisplayName = "淡入淡出"),
	BPM UMETA(DisplayName = "BPM速度切换"),
	BPM_Fade UMETA(DisplayName = "淡入淡出+BPM速度切换"),
	Switch UMETA(DisplayName = "直接切换")
};

//背景音乐的信息
USTRUCT(BlueprintType)
struct FBGMInfo : public FTableRowBase
{
	GENERATED_BODY()
public:

	bool operator==(const FBGMInfo& other) const {
		
		if (RowName == other.RowName && ResourceNameOrIndex == other.ResourceNameOrIndex)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FName RowName;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString ResourceNameOrIndex;

	//该BGM要推向的通道名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Channel")
	FName BGMChannelName = "Main";

	//是否同时处理其他的全部通道
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bChangeAllChannel = false;
	//同时影响效果
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bChangeAllChannel"))
	FChangeBGMChanelInfo ChangeAllBGMChanelInfo;
	//推送该BGM时影响的其他通道
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Channel", meta = (EditConditionHides, EditCondition = "!bChangeAllChannel"))
	TMap<FName, FChangeBGMChanelInfo> ChannelOtherBGMChanelInfo;

	//播放类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESwitchBMGType SwitchBMGType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TSoftObjectPtr<USoundBase> SoundBase;

	/*是否仅作为一个消息包发送
	* 当该值为true时 Push该BGM时，会先在该通道寻找同SoundBase类型一致的目标去修改它的参数
	* 在没找到的情况下，才会创建一个UAudioComponent去播放BGM
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	bool bIsMessagePackage = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoundParameters Parameters;

	//每分钟节拍数
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BPM;
	//切换的绝对拍子数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 1, EditConditionHides, EditCondition = "SwitchBMGType == ESwitchBMGType::BPM || SwitchBMGType == ESwitchBMGType::BPM_Fade"))
	int32 AbsoluteBeatNum = 1;
	//淡入的相对拍子数(该值范围在1~AbsoluteBeatNum，并且只有SwitchBMGType = 淡入淡出+BPM速度切换时才生效)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 1, EditConditionHides, EditCondition = "SwitchBMGType == ESwitchBMGType::BPM_Fade"))
	int32 FadeInOutBeatNum = 1;

	//背景音乐淡出时间
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditConditionHides, EditCondition = "SwitchBMGType == ESwitchBMGType::Fade"))
	float BGMFadeOutTime = UResourcesConfig::GetInstance()->BGMFadeOutTime_2D;
	//背景音乐淡入时间
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditConditionHides, EditCondition = "SwitchBMGType == ESwitchBMGType::Fade"))
	float BGMFadeInTime = UResourcesConfig::GetInstance()->BGMFadeInTime_2D;
	//淡入淡出的等待时间混合比例。根据通过上一段音乐混出时间换算
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0, ClampMax = 1, EditConditionHides, EditCondition = "SwitchBMGType == ESwitchBMGType::Fade"))
	float DelayTimeScale = 0.5f;

	//字幕
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRDTC_SoundSubtitle SoundSubtitle;

	//该环境下要设置的混合
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USoundMix*> SetSoundMix;

	//相关资源
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UObject> ReferenceObject;
};

USTRUCT(BlueprintType)
struct FResourceProperty_BGM : public FTableRowBase
{
	GENERATED_BODY()
public:
	FResourceProperty_BGM() {}
	FResourceProperty_BGM(TMap<FString, FBGMInfo> _BGM) { BGM = _BGM; }
	bool Contains(FString Name) { return BGM.Contains(Name); };
	FBGMInfo operator[](FString Name) { return Contains(Name) ? BGM[Name] : FBGMInfo(); };
	FBGMInfo operator[](int32 Index)
	{
		TArray<FBGMInfo> Value;
		BGM.GenerateValueArray(Value);
		return Value.IsValidIndex(Index) ? Value[Index] : FBGMInfo();
	};

	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
	{
		FResourceProperty_BGM* ResourceProperty_BGM = InDataTable->FindRow<FResourceProperty_BGM>(InRowName, TEXT(""));
		if (ResourceProperty_BGM)
		{
			for (TPair<FString, FBGMInfo>& pair : ResourceProperty_BGM->BGM)
			{
				if (pair.Value.Parameters.MyAudioParameter.Num() == 0)
				{
					pair.Value.Parameters.MyAudioParameter = DefaultParameters.MyAudioParameter;
				}
				if (pair.Value.Parameters.CustomAudioParameter.Num() == 0)
				{
					pair.Value.Parameters.CustomAudioParameter = DefaultParameters.CustomAudioParameter;
				}

				pair.Value.Parameters.SetAudioParameterParamNameOfMapKey();
				pair.Value.RowName = InRowName;
				pair.Value.ResourceNameOrIndex = pair.Key;
			}

			DefaultParameters.SetAudioParameterParamNameOfMapKey();
		}
	}
public:
	//配置用备注
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Tip;

	//相关资源
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSoftObjectPtr<UObject>> ReferencesObjects;

	//默认参数结构
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoundParameters DefaultParameters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FBGMInfo> BGM;
};

//播放资产的类型
UENUM(BlueprintType)
enum class EPlaySoundResourceType :uint8
{
	ArrayRandom = 0 UMETA(DisplayName = "数组随机"),
	WeightRandom UMETA(DisplayName = "权重随机"),
	ArrayIndexLoop UMETA(DisplayName = "列表循环"),
	CustomIndexLoop UMETA(DisplayName = "自定义下标循环"),
	SimultaneouslyPlay UMETA(DisplayName = "同时触发")
};

