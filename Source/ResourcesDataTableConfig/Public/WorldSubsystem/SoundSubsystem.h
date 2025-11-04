// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ResourcesStructAndEnum.h"
#include "Subsystems/WorldSubsystem.h"
//#include "UObject/Interface.h"
#include <Sound/SoundEvent.h>
#include "SoundSubsystem.generated.h"


//字幕控件接口
UINTERFACE(MinimalAPI)
class USubtitleWidgetInteract : public UInterface
{
	GENERATED_BODY()
};

/**
 * 字幕控件接口
 */
class RESOURCESDATATABLECONFIG_API ISubtitleWidgetInteract
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//开始设置字幕(播放者，字幕信息)
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UUserWidget* ShowSubtitleCue(AActor* Player, const TArray<FSubtitleCue>& SubtitleCue);
	virtual UUserWidget* ShowSubtitleCue_Implementation(AActor* Player, const TArray<FSubtitleCue>& SubtitleCue){ return nullptr; }
	//隐藏字幕显示
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void HideSubtitles(UUserWidget* HideUI);
	virtual void HideSubtitles_Implementation(UUserWidget* HideUI) {}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBGMChannelEvent);

//声音通道
UCLASS(Blueprintable, EditInlineNew, BlueprintType)
class UBGMChannel : public UObject
{
	GENERATED_BODY()
public:
	//往通道中推送音效
	UFUNCTION(BlueprintCallable)
		UAudioComponent* PushBGM(FName RowName, FString ResourceNameOrIndex);

	//往通道中推送音效
	UFUNCTION(BlueprintCallable)
		UAudioComponent* PushBGMOfInfo(FBGMInfo PushInfo);

	UFUNCTION()
		void SetNewBackgroundSound();

	UFUNCTION()
		void SetBGMAudioComponent(UAudioComponent* NewAudioComponent);

	//当前正在播放背景音乐的进度回调
	UFUNCTION()
		void CurBGMPlaybackPercent(const USoundWave* SoundWave, const float PlaybackPercent);
	//当前正在播放背景音乐完成的回调
	UFUNCTION()
		void CurBGMFinished();

	//设置参数
	UFUNCTION(BlueprintCallable)
		void SetParameter(FBGMInfo ParameterInfo);

	//通道结束——当通道内的音乐播放完毕时通道会关闭
	UFUNCTION(BlueprintCallable)
		void ChannelEnd();

	/*设置通道内BGM的音量(相乘)
	* OtherChannelName：被哪个通道影响
	*/
	UFUNCTION()
		void SetChannelVolume(FName OtherChannelName, float Volume);

	//刷新通道内BGM的音量
	UFUNCTION()
		void RefreshChannelVolume();

	/*改变BGM通道
	* OtherChannelName：被哪个通道影响
	*/
	UFUNCTION(BlueprintCallable)
	void ChangeBGMChanel(FName OtherChannelName, FChangeBGMChanelInfo ChangeBGMChanelInfo);
		
	/*改变通道暂停播放通道
	* OtherChannelName：被哪个通道影响
	*/
	UFUNCTION(BlueprintCallable)
	void ChangeChannelPauseState(FName OtherChannelName, bool IsPause = true);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ChannelName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAudioComponent* CurAudioCom;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FBGMInfo CurBGMInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FBGMInfo PushBGMInfo;

	//当前的BGM声音文件播放完成音效名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FString> CurBGMPlayFinishedName;

	UPROPERTY(BlueprintReadOnly)
		FTimerHandle PlayNewBMGTimerHandle;

	//BGM播放的时间，用于BPM切换时计算
	UPROPERTY(BlueprintReadOnly)
		float CurBGMPlayTime;

	UPROPERTY(BlueprintReadOnly)
		float BPM_Fade_FadeInOutTime;

	//被其他通道影响的值
	UPROPERTY(BlueprintReadOnly)
		TMap<FName, float> VolumeInfo;

	UPROPERTY(BlueprintReadOnly)
		USoundSubsystem* SoundSubsystem;

	//音频通道的音量变动
	UPROPERTY(BlueprintAssignable)
	FBGMChannelEvent BGMChannelVolumeChange;

	//新的音频推送进来的事件
	UPROPERTY(BlueprintAssignable)
	FBGMChannelEvent PushNewBGM;

	//使该通道暂停的其他通道
	UPROPERTY(BlueprintReadWrite)
	TArray<FName> OtherChannelPause;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSoundSubsystemEvent);

/**
 * 声音子系统，管理游戏中的全部声音
 */
UCLASS()
class RESOURCESDATATABLECONFIG_API USoundSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	virtual void Deinitialize() override;
public:
	//新增选择表格参数
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", AdvancedDisplay = "5", DefaultToSelf = "Player"))
	UAudioComponent* PlaySound_2D(const UObject* WorldContextObject, AActor* Player, FName RowName, FString ResourceNameOrIndex, FGameplayTag SoundTag, bool IsUseParameter = true, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, USoundConcurrency* ConcurrencySettings = nullptr, bool bPersistAcrossLevelTransition = false, bool bAutoDestroy = true);
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", AdvancedDisplay = "6", DefaultToSelf = "Player"))
	UAudioComponent* PlaySound_Location(const UObject* WorldContextObject, AActor* Player, FName RowName, FString ResourceNameOrIndex, FGameplayTag SoundTag, FVector Location, bool IsUseParameter = true, FRotator Rotation = FRotator::ZeroRotator, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, class USoundAttenuation* AttenuationSettings = nullptr, USoundConcurrency* ConcurrencySettings = nullptr, bool bAutoDestroy = true);

	UFUNCTION(BlueprintCallable, meta = (AdvancedDisplay = "5", DefaultToSelf = "Player"))
	UAudioComponent* PlaySound_Attached(AActor* Player, FName RowName, FString ResourceNameOrIndex, FGameplayTag SoundTag, USceneComponent* AttachToComponent, bool IsUseParameter = true, FName AttachPointName = NAME_None, FVector Location = FVector(ForceInit), FRotator Rotation = FRotator::ZeroRotator, EAttachLocation::Type LocationType = EAttachLocation::KeepRelativeOffset, bool bStopWhenAttachedToDestroyed = false, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, USoundAttenuation* AttenuationSettings = nullptr, USoundConcurrency* ConcurrencySettings = nullptr, bool bAutoDestroy = true);

	//Array版本的函数出现主要是为了应对MetaSound中识别不到Subtile字幕的问题，通过将同个对话不同语速的声音拆分成不同的资源标记在数据表中可以单独为这些声音配置字幕
	//IsRandomPlayOneSound : 是否随机播放一个声音，该值为flase时将尝试播放SoundAssetTags中的全部声音资产
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", AdvancedDisplay = "5", DefaultToSelf = "Player"))
	TArray<UAudioComponent*> PlaySound_2D_Array(const UObject* WorldContextObject, AActor* Player, TArray<FResourceProperty_SoundAssetTag> SoundAssetTags, FGameplayTag SoundTag, bool IsRandomPlayOneSound = true, bool IsUseParameter = true, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, USoundConcurrency* ConcurrencySettings = nullptr, bool bPersistAcrossLevelTransition = false, bool bAutoDestroy = true);
	//IsRandomPlayOneSound : 是否随机播放一个声音，该值为flase时将尝试播放SoundAssetTags中的全部声音资产
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", AdvancedDisplay = "6", DefaultToSelf = "Player"))
	TArray<UAudioComponent*> PlaySound_Location_Array(const UObject* WorldContextObject, AActor* Player, TArray<FResourceProperty_SoundAssetTag> SoundAssetTags, FGameplayTag SoundTag, FVector Location, bool IsRandomPlayOneSound = true, bool IsUseParameter = true, FRotator Rotation = FRotator::ZeroRotator, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, class USoundAttenuation* AttenuationSettings = nullptr, USoundConcurrency* ConcurrencySettings = nullptr, bool bAutoDestroy = true);
	//IsRandomPlayOneSound : 是否随机播放一个声音，该值为flase时将尝试播放SoundAssetTags中的全部声音资产
	UFUNCTION(BlueprintCallable, meta = (AdvancedDisplay = "5", DefaultToSelf = "Player"))
	TArray<UAudioComponent*> PlaySound_Attached_Array(AActor* Player, TArray<FResourceProperty_SoundAssetTag> SoundAssetTags, FGameplayTag SoundTag, USceneComponent* AttachToComponent, bool IsRandomPlayOneSound = true, bool IsUseParameter = true, FName AttachPointName = NAME_None, FVector Location = FVector(ForceInit), FRotator Rotation = FRotator::ZeroRotator, EAttachLocation::Type LocationType = EAttachLocation::KeepRelativeOffset, bool bStopWhenAttachedToDestroyed = false, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, USoundAttenuation* AttenuationSettings = nullptr, USoundConcurrency* ConcurrencySettings = nullptr, bool bAutoDestroy = true);

	//通过资产标记获得对应BGM的信息
	UFUNCTION(BlueprintCallable)
	FBGMInfo GetBGMInfoFromSoundAssetTag(FResourceProperty_SoundAssetTag SoundAssetTag);
	//通过资产标记获得对应Sound的信息
	UFUNCTION(BlueprintCallable)
	FResourceProperty_SoundInfo GetSoundInfoFromSoundAssetTag(FResourceProperty_SoundAssetTag SoundAssetTag);

	//获得其他播放者的音量大小参数
	UFUNCTION()
		float GetOtherPlayerVolume(AActor* Player, float CurVolumeMultiplier);

	//尝试添加声音组件到播放者身上
	UFUNCTION()
		void AddAudioComponentToPlayer(AActor* Player, FGameplayTag SoundTag, UAudioComponent* AudioComponent);

	//加载背景音乐声音资源
	UFUNCTION(BlueprintCallable)
	void LoadBGMSound(TArray<FName> RowName);
	//背景音乐加载完成
	UFUNCTION()
	void LoadBGMCompleted();
	
	//加载音效声音资源
	UFUNCTION(BlueprintCallable)
	void LoadSound(TArray<UDataTable*> DataTable);
	//背景音乐加载完成
	UFUNCTION()
	void LoadSoundCompleted();

	UFUNCTION()
		void LoadCompleted();

	//设置参数
	UFUNCTION(BlueprintCallable)
		void SetParameter(UAudioComponent* AudioComponent, FResourceProperty_SoundInfo SoundInfo);

	//处理字幕
	UFUNCTION(BlueprintCallable)
		void DisplaySubtitle(AActor* Player, UAudioComponent* AudioComponent, FRDTC_SoundSubtitle SoundSubtitle);

	//音效播放完成
	UFUNCTION()
		void AudioFinished(UAudioComponent* AudioComponent);

	//截断原生字幕
	UFUNCTION()
		void GetSubtitles(const TArray<FSubtitleCue>& Subtitles, float CueDuration);

	UFUNCTION(BlueprintCallable)
		UUserWidget* GetSubtitlesPanel();

	//推送BGM通道
	UFUNCTION(BlueprintCallable)
		UBGMChannel* PushBGMToChannel(FName RowName, FString ResourceNameOrIndex);

	//推送BGM通道_自定义通道名称
	UFUNCTION(BlueprintCallable)
		UBGMChannel* PushBGMToChannel_CustomChannelName(FName ChannelName, FName RowName, FString ResourceNameOrIndex);

	//推送BGM通道
	UFUNCTION(BlueprintCallable)
		UBGMChannel* PushBGMToChannelOfInfo(FBGMInfo PushInfo);

	//推送BGM通道_自定义通道名称
	UFUNCTION(BlueprintCallable)
		UBGMChannel* PushBGMToChannelOfInfo_CustomChannelName(FName ChannelName, FBGMInfo PushInfo);

	//弹出/关闭某个通道
	UFUNCTION(BlueprintCallable)
		void PopBGMChannel(FName ChannelName);

	//弹出/关闭某个通道
	UFUNCTION(BlueprintCallable)
		void PopBGMChannelOfChannel(UBGMChannel* Channel);

	//某个通道弹出BGM：1.该通道结束了	2.该通道有新的信息推送了，之前的信息影响的内容需要还原
	//主要是为了还原他影响的其他通道音量
	UFUNCTION(BlueprintCallable)
	void SomeChannelPopBGM(UBGMChannel* Channel);

	//触发声音事件
	UFUNCTION(BlueprintCallable)
	void TriggerSoundEvent(FName EventName, FCC_CompareInfo CompareParameter, TArray<UAudioComponent*>& SoundComs, TArray<UBGMChannel*>& BGMChannels);

	//声音事件的处理
	UFUNCTION(BlueprintCallable)
	void SoundEventProcess(FSoundEventProcess& ProcessInfo, TArray<UAudioComponent*>& SoundComs, TArray<UBGMChannel*>& BGMChannels);

	//声音事件的调制处理
	UFUNCTION(BlueprintCallable)
	void SoundEventAudioModulation(FSoundEventAudioModulationInfo& ModulationInfo);

public:

	//BGM通道<通道名称,通道>
	UPROPERTY(BlueprintReadOnly)
		TMap<FName, UBGMChannel*> AllBGMChannel;

	//BGM播放的时间，用于BPM切换时计算
	UPROPERTY(BlueprintReadOnly)
	float CurMainBGMPlayTime;
	UPROPERTY(BlueprintReadOnly)
	float CurMainBGMStartPlayTime;
	UPROPERTY(BlueprintReadOnly)
	float BPM_Fade_FadeInOutTime;
	UPROPERTY(BlueprintReadOnly)
	FTimerHandle PlayNewBMGTimerHandle;

	UPROPERTY(BlueprintReadOnly)
	FBGMInfo CurMainBGMInfo;
	UPROPERTY(BlueprintReadOnly)
	FBGMInfo NewBGMInfo;

	//是否加载完成了BGM
	UPROPERTY(BlueprintReadOnly)
	bool bIsLoadBGMCompleted;
	//是否加载完了音效资源
	UPROPERTY(BlueprintReadOnly)
	bool bIsLoadSoundompleted;

	//音频通道变动
	UPROPERTY(BlueprintAssignable)
	FSoundSubsystemEvent BGMChannelChange;

public:
	//字幕的UI
	UPROPERTY(BlueprintReadOnly)
	UUserWidget* SubtitlesPanel;

	//当前正在处理字幕的AudioComponent
	UPROPERTY(BlueprintReadOnly)
	TMap<UAudioComponent*, UUserWidget*> SubtitlesAudioComponent;

	//当前播放声音的播放者——主要是GetSubtitles函数截断原生字幕时需要该参数
	UPROPERTY()
	AActor* CurSubtitlesPlayer;
	//当前播放声音的音频组件——主要是GetSubtitles函数截断原生字幕时需要该参数
	UPROPERTY()
	UAudioComponent* CurSubtitlesAudioComponent;
};
