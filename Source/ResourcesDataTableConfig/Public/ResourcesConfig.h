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

	/*一段对话信息表
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = Dialogue)
		TSoftObjectPtr<UDataTable> SectionDialogueInfoDataTable;

	//初始创建的BGM通道
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = Sound)
	TArray<FName> InitCreateBGMChannelNames;
};
