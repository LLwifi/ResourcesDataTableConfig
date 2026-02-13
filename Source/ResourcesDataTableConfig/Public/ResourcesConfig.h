// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include <Engine/DataTable.h>
#include "ResourcesConfig.generated.h"

UENUM(BlueprintType)
enum class EResourceType :uint8
{
	Bool = 0,
	Int32,
	Float,
	Name,
	String,
	Text,
	Vector,
	Rotator,
	Texture2D,
	Class,
	Sound,

	
	Transform = 50,
	SoundEnvironment,
	BGM
};

class USoundControlBusMix;
class USoundModulationParameter;

USTRUCT(BlueprintType)
struct FAudioModulationInfo : public FTableRowBase
{
	GENERATED_BODY()
public:
	//失活全部
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDeactivateAllBusMixes = false;
	//要停用的总线
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "!bIsDeactivateAllBusMixes"))
	TSoftObjectPtr<USoundControlBusMix> DeactivateMix;

	//是否要激活总线
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bIsActivateBusMix = false;
	//调制总线
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsActivateBusMix"))
	TSoftObjectPtr<USoundControlBusMix> Mix;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bIsActivateBusMix"))
	FString AddressFilter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bIsActivateBusMix"))
	TSubclassOf<USoundModulationParameter> ParamClassFilter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bIsActivateBusMix"))
	TSoftObjectPtr <USoundModulationParameter> ParamFilter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bIsActivateBusMix"))
	float Value;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bIsActivateBusMix"))
	float FadeTime;
};

/**
 * 
 */
UCLASS(config = ResourcesConfig, defaultconfig)
class RESOURCESDATATABLECONFIG_API UResourcesConfig : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	virtual FName GetCategoryName() const final override
	{
		return FName("GameEditorConfig");
	}
	static UResourcesConfig* GetInstance();
	UFUNCTION(BlueprintPure, BlueprintCallable)
		static UResourcesConfig* GetResourcesConfig() { return GetInstance(); }

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
		TMap<EResourceType, TSoftObjectPtr<UDataTable>> TypeMaping;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
		TSoftObjectPtr<UDataTable> ResourceTypeGroup;

	//2D背景音乐默认淡出时间
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = Sound)
		float BGMFadeOutTime_2D = 1.0f;
	//2D背景音乐默认淡入时间
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = Sound)
		float BGMFadeInTime_2D = 1.0f;

	//其他玩家声音音量调整记录
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = Sound, meta = (ClampMin = 0, ClampMax = 1))
		float OtherPlayerSoundVolume = 0.5f;

	//是否显示字幕
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = Sound)
		bool bIsShowSubtitles = true;
	//创建字幕的UI类
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = Sound)
		TSoftClassPtr<UUserWidget> SubtitlesWidgetClass;
	//字幕UI所处的ZOrder
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = Sound)
		int32 SubtitlesWidgetZOrder = 0;

	//BGM通道显示信息UI类
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = Sound)
		TSoftClassPtr<UUserWidget> BGMChannelInfoWidgetClass;

	//声音事件表
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = Sound)
		TSoftObjectPtr<UDataTable> SoundEventDataTable;

	/*全部音效表
	* 主要用于FResourceProperty_SoundAssetTag根据行名称寻找确定的音效表
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = Sound)
		TArray<TSoftObjectPtr<UDataTable>> AllSoundDataTable;

	/*全部BGM表
	* 主要用于FResourceProperty_SoundAssetTag根据行名称寻找确定的音效表
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = Sound)
		TArray<TSoftObjectPtr<UDataTable>> AllBGMDataTable;

	//初始创建的BGM通道
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = Sound)
		TArray<FName> InitCreateBGMChannelNames;

	/*一段对话信息表
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = Dialogue)
		TSoftObjectPtr<UDataTable> SectionDialogueInfoDataTable;
	/*对话期间默认的Modulation设置信息
	* 
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = Dialogue)
		FAudioModulationInfo DefaultDialogueModulationInfo;

	/*AudioVolume配置表
	* 通过该表的配置对ARDTC_AudioVolume进行设置
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = AudioVolume)
		TSoftObjectPtr<UDataTable> AudioVolumeInfoDataTable;
};
