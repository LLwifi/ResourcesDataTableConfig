// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/AudioVolume.h"
#include <Engine/DataTable.h>
#include "RDTC_AudioVolume.generated.h"

USTRUCT(BlueprintType)
struct FRDTC_AudioVolumeInfo : public FTableRowBase
{
	GENERATED_BODY()
public:
	/*本卷的优先级。在卷重叠的情况下，优先级最高的卷
	* 被选中。如果两个或多个重叠的卷具有相同的优先级，则顺序未定义
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Priority;

	//当前是否启用了此音量并能够影响声音
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsEnabled = true;

	//用于此音量的混响设置
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FReverbSettings Settings;

	//根据声源和播放器是在音量范围内还是在音量范围外，影响在其声音等级中将“应用环境音量”设置为true的声音
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FInteriorSettings AmbientZoneSettings;

	//Submix发送用于此卷的设置。允许音频根据源和听众相对于此音量的位置动态发送到提交
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FAudioVolumeSubmixSendSettings> SubmixSendSettings;

	//子像素效果链覆盖设置。当条件满足时，将覆盖给定提交文件上的效果链
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FAudioVolumeSubmixOverrideSettings> SubmixOverrideSettings;
};

USTRUCT(BlueprintType)
struct FRDTC_AudioVolumeInfoHandle : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RowName;

#if WITH_EDITORONLY_DATA
	//行名称对应的数据表
	UPROPERTY(VisibleAnywhere)
	TSoftObjectPtr<UDataTable> DataTable;
#endif
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class RESOURCESDATATABLECONFIG_API ARDTC_AudioVolume : public AAudioVolume
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

#if WITH_EDITOR
	UFUNCTION()
	static TArray<FName> GetDTAudioVolumeInfoRowNames();

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	//刷新设置信息
	UFUNCTION(BlueprintCallable)
	void RefreshSettingFromInfo(FRDTC_AudioVolumeInfo AudioVolumeInfo);
	//刷新设置信息
	UFUNCTION(BlueprintCallable)
	void RefreshSettingFromRowName(UPARAM(Meta = (GetOptions = "ResourcesDataTableConfig.RDTC_AudioVolume.GetDTAudioVolumeInfoRowNames")) FName RowName);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRDTC_AudioVolumeInfoHandle AudioVolumeInfoHandle;
};
