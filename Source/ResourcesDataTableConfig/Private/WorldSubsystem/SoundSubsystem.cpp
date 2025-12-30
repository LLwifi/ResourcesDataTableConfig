// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldSubsystem/SoundSubsystem.h"
#include "AudioDevice.h"
#include "ResourceBPFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "ActorComponent/SoundComponent.h"
#include "Blueprint/UserWidget.h"
#include "AudioModulationStatics.h"

UAudioComponent* UBGMChannel::PushBGM(FName RowName, FString ResourceNameOrIndex)
{
	FBGMInfo BGMInfo;
	if (UResourceBPFunctionLibrary::GetResourceFromString_BGM(RowName, ResourceNameOrIndex, BGMInfo))
	{
		return PushBGMOfInfo(BGMInfo);
	}
	return nullptr;
}

UAudioComponent* UBGMChannel::PushBGMOfInfo(FBGMInfo PushInfo)
{
	PushBGMInfo = PushInfo;
	//弹出之前的混音器
	for (USoundMix* SoundMix : CurBGMInfo.SetSoundMix)
	{
		if (SoundMix)
		{
			UGameplayStatics::PopSoundMixModifier(this, SoundMix);
		}
	}
	//还原之前影响的其他通道
	if (SoundSubsystem)
	{
		SoundSubsystem->SomeChannelPopBGM(this);
		SoundSubsystem->DisplaySubtitle(nullptr, CurAudioCom, CurBGMInfo.SoundSubtitle);
	}

	//推送新的混音器
	for (USoundMix* SoundMix : PushBGMInfo.SetSoundMix)
	{
		if (SoundMix)
		{
			UGameplayStatics::PushSoundMixModifier(this, SoundMix);
		}
	}

	

	//切换主BGM
	if (!PushBGMInfo.SoundBase.IsNull())
	{
		if (PushBGMInfo.bIsMessagePackage && CurAudioCom &&CurAudioCom->Sound == PushBGMInfo.SoundBase)//是否为一个消息包 消息包有可能不会创建UAudioComponent
		{
			UResourceBPFunctionLibrary::SetParameterFromAudioComponent(CurAudioCom, PushBGMInfo.Parameters);
			RefreshChannelVolume();
		}
		else
		{
			//该值不能大于AbsoluteBeatNum，这里进行处理
			int32 FadeInOutBeatNum = PushBGMInfo.FadeInOutBeatNum = PushBGMInfo.FadeInOutBeatNum > PushBGMInfo.AbsoluteBeatNum ? PushBGMInfo.AbsoluteBeatNum : PushBGMInfo.FadeInOutBeatNum;
			switch (PushBGMInfo.SwitchBMGType) {
			case ESwitchBMGType::Fade:
			{
				//当前有没有音频正在播放
				if (CurAudioCom && CurAudioCom->IsPlaying())
				{
					CurAudioCom->FadeOut(CurBGMInfo.BGMFadeOutTime, 0.0f);
					//新音效淡入之前的等待时间
					float DelayTime = CurBGMInfo.BGMFadeOutTime * PushBGMInfo.DelayTimeScale;
					if (DelayTime > 0.0f)
					{
						GetWorld()->GetTimerManager().SetTimer(PlayNewBMGTimerHandle, this, &UBGMChannel::SetNewBackgroundSound, DelayTime);
					}
					else
					{
						SetNewBackgroundSound();
					}
				}
				else
				{
					SetNewBackgroundSound();
				}
				break;
			}
			case ESwitchBMGType::BPM:
			{
				if (CurAudioCom && CurAudioCom->IsPlaying())
				{
					float Interval = 60.0f / CurBGMInfo.BPM;//一拍的时间
					float AbsoluteBeatInterval = Interval * CurBGMInfo.AbsoluteBeatNum;
					float TimeDifference = UKismetMathLibrary::GenericPercent_FloatFloat(CurBGMPlayTime, AbsoluteBeatInterval);//距离下个绝对节点的时间差
					if (UKismetMathLibrary::NearlyEqual_FloatFloat(AbsoluteBeatInterval - TimeDifference, 0.01, 0.0099))
					{
						SetNewBackgroundSound();
					}
					else
					{
						GetWorld()->GetTimerManager().SetTimer(PlayNewBMGTimerHandle, this, &UBGMChannel::SetNewBackgroundSound, AbsoluteBeatInterval - TimeDifference);
					}
				}
				else
				{
					SetNewBackgroundSound();
				}
				break;
			}
			case ESwitchBMGType::BPM_Fade:
			{
				if (CurAudioCom && CurAudioCom->IsPlaying())
				{
					float Interval = 60.0f / CurBGMInfo.BPM;//一拍的时间
					float AbsoluteBeatInterval = Interval * CurBGMInfo.AbsoluteBeatNum;
					float TimeDifference = UKismetMathLibrary::GenericPercent_FloatFloat(CurBGMPlayTime, AbsoluteBeatInterval);//距离下个绝对节点的时间差
					BPM_Fade_FadeInOutTime = Interval * CurBGMInfo.FadeInOutBeatNum;//当前bgm的混出时间
					float WaitTime = AbsoluteBeatInterval - TimeDifference - BPM_Fade_FadeInOutTime;//减去混合时间 剩下的 等待时间
					if (UKismetMathLibrary::NearlyEqual_FloatFloat(WaitTime, 0.01, 0.0099))//没有混合时间了
					{
						CurAudioCom->SetSound(PushBGMInfo.SoundBase.LoadSynchronous());
						CurAudioCom->Play();
						SetParameter(PushBGMInfo);
					}
					else if (WaitTime < 0)//已经到达混合的节拍区域内了
					{
						SetNewBackgroundSound();
					}
					else//距离下个绝对节点仍有一段时间
					{
						GetWorld()->GetTimerManager().SetTimer(PlayNewBMGTimerHandle, this, &UBGMChannel::SetNewBackgroundSound, WaitTime);
					}
				}
				else
				{
					SetNewBackgroundSound();
				}
				break;
			}
			case ESwitchBMGType::Switch:
			{
				SetNewBackgroundSound();
			}
			}
			RefreshChannelVolume();
		}
	}

	//字幕
	if (SoundSubsystem)
	{
		SoundSubsystem->DisplaySubtitle(nullptr, CurAudioCom, CurBGMInfo.SoundSubtitle);
	}

	return CurAudioCom;
}

void UBGMChannel::SetNewBackgroundSound()
{
	switch (PushBGMInfo.SwitchBMGType) {
		case ESwitchBMGType::Fade:
		{
			//淡出需要立刻淡出，这里只处理等待x秒之后的新bgm入场
			SetBGMAudioComponent(UGameplayStatics::CreateSound2D(this, PushBGMInfo.SoundBase.LoadSynchronous()));
			if (CurAudioCom)
			{
				CurAudioCom->FadeIn(PushBGMInfo.BGMFadeInTime, 1.0f);
			}
			break;
		}
		case ESwitchBMGType::BPM:
		{
			//BPM在切换的瞬间 = 直接切换，只不过在可能需要经过一段时间的等待再进行切换
			if (CurAudioCom)
			{
				CurAudioCom->Stop();
			}
			SetBGMAudioComponent(UGameplayStatics::CreateSound2D(this, PushBGMInfo.SoundBase.LoadSynchronous()));
			break;
		}
		case ESwitchBMGType::BPM_Fade:
		{
			//同上只处理经过等待的切换
			if (CurAudioCom)
			{
				CurAudioCom->FadeOut(BPM_Fade_FadeInOutTime, 0.0f);
			}
			SetBGMAudioComponent(UGameplayStatics::CreateSound2D(this, PushBGMInfo.SoundBase.LoadSynchronous()));
			if (CurAudioCom)
			{
				CurAudioCom->FadeIn(BPM_Fade_FadeInOutTime);
			}
			break;
		}
		case ESwitchBMGType::Switch://直接切换不会跑到该函数进行处理，该函数只处理延迟播放的bgm
		{
			if (CurAudioCom)
			{
				//如何停止当前的BGM
				if (CurBGMInfo.SwitchBMGType == ESwitchBMGType::Fade)
				{
					CurAudioCom->FadeOut(CurBGMInfo.BGMFadeOutTime, 0.0f);
				}
				else if (CurBGMInfo.SwitchBMGType == ESwitchBMGType::Switch)
				{
					CurAudioCom->Stop();
				}
				else
				{
					CurAudioCom->Stop();
				}
			}
			SetBGMAudioComponent(UGameplayStatics::CreateSound2D(this, PushBGMInfo.SoundBase.LoadSynchronous(), 1.0f, 1.0f));
			break;
		}
	}
	CurBGMInfo = PushBGMInfo;
	PushNewBGM.Broadcast();
}

void UBGMChannel::SetBGMAudioComponent(UAudioComponent* NewAudioComponent)
{
	if (NewAudioComponent)
	{
		FScriptDelegate AudioPlaybackPercent, AudioFinished;
		AudioPlaybackPercent.BindUFunction(this, "CurBGMPlaybackPercent");
		AudioFinished.BindUFunction(this, "CurBGMFinished");
		if (CurAudioCom)//移除之前的绑定
		{
			CurAudioCom->OnAudioPlaybackPercent.Remove(AudioPlaybackPercent);
			CurAudioCom->OnAudioFinished.Remove(AudioFinished);
			CurAudioCom = nullptr;
		}

		//绑定新音效的进度
		NewAudioComponent->OnAudioPlaybackPercent.Add(AudioPlaybackPercent);
		NewAudioComponent->OnAudioFinished.Add(AudioFinished);
		CurAudioCom = NewAudioComponent;
		CurAudioCom->Play();//该函数触发PlaybackPercent事件
		SetParameter(PushBGMInfo);
	}
}

void UBGMChannel::CurBGMPlaybackPercent(const USoundWave* SoundWave, const float PlaybackPercent)
{
	CurBGMPlayTime = SoundWave->Duration/*->GetDuration()*/ * PlaybackPercent;
	//if (PlaybackPercent >= 1.0f)//实际测试下来PlaybackPercent =- 1.0 时会调用两次 最终改用FOnAudioFinished事件
	//{
	//	//这个声音文件要播放的音效次数
	//	CurBGMPlayFinishedName.AddUnique(UKismetSystemLibrary::GetDisplayName(SoundWave));
	//	if (CurBGMPlayFinishedName.Num() >= CurAudioCom->Sound->CurrentPlayCount.Num())
	//	{
	//		ChannelEnd();
	//	}
	//}
}

void UBGMChannel::CurBGMFinished()
{
	ChannelEnd();
}

void UBGMChannel::SetParameter(FBGMInfo ParameterInfo)
{
	UResourceBPFunctionLibrary::SetParameterFromAudioComponent(CurAudioCom, ParameterInfo.Parameters);
}

void UBGMChannel::ChannelEnd()
{
	//弹出当前的混音器
	for (USoundMix* SoundMix : CurBGMInfo.SetSoundMix)
	{
		if (SoundMix)
		{
			UGameplayStatics::PopSoundMixModifier(this, SoundMix);
		}
	}
	VolumeInfo.Empty();
	if (SoundSubsystem)
	{
		SoundSubsystem->SomeChannelPopBGM(this);
	}
	
	//如果还在播放，结束播放
	if (CurAudioCom && CurAudioCom->IsPlaying())
	{
		//CurAudioCom->Stop();
		CurAudioCom->FadeOut(UResourcesConfig::GetInstance()->BGMFadeOutTime_2D, 0.0f);
	}

	CurBGMPlayTime = 0.0f;
	CurBGMPlayFinishedName.Empty();
	if (CurAudioCom)
	{
		FScriptDelegate ScriptDelegate;
		ScriptDelegate.BindUFunction(this, "CurBGMPlaybackPercent");
		CurAudioCom->OnAudioPlaybackPercent.Remove(ScriptDelegate);
	}
	CurAudioCom = nullptr;
	BGMChannelVolumeChange.Broadcast();
}

void UBGMChannel::SetChannelVolume(FName OtherChannelName, float Volume)
{
	if (VolumeInfo.Contains(OtherChannelName))
	{
		if (Volume == 1.0f)//1.0表示不影响，不影响的在此删除避免多余计算
		{
			VolumeInfo.Remove(OtherChannelName);
		}
		else
		{
			VolumeInfo[OtherChannelName] = Volume;
		}
		
	}
	else
	{
		VolumeInfo.Add(OtherChannelName, Volume);
	}

	RefreshChannelVolume();
}

void UBGMChannel::RefreshChannelVolume()
{
	float NewVolume = 1.0f;

	TArray<float> Volumes;
	VolumeInfo.GenerateValueArray(Volumes);
	for (int32 i = 0; i < Volumes.Num(); i++)
	{
		NewVolume *= Volumes[i];
	}

	if (CurAudioCom)
	{
		//根据被影响的音量决定通道的行为
		if (NewVolume == 0.0f)
		{
			CurAudioCom->SetPaused(true);
		}
		else if (NewVolume < 0.0f)
		{
			ChannelEnd();
		}
		else
		{
			CurAudioCom->SetPaused(false);
			CurAudioCom->SetVolumeMultiplier(NewVolume);
		}
	}
	BGMChannelVolumeChange.Broadcast();
}

void UBGMChannel::ChangeBGMChanel(FName OtherChannelName, FChangeBGMChanelInfo ChangeBGMChanelInfo)
{
	switch (ChangeBGMChanelInfo.ChangeBGMChanelType)
	{
	case EChangeBGMChanelType::Volume:
	{
		SetChannelVolume(OtherChannelName, ChangeBGMChanelInfo.Volume);
		break;
	}
	case EChangeBGMChanelType::Paused:
	{
		ChangeChannelPauseState(OtherChannelName, true);
		break;
	}
	case EChangeBGMChanelType::PopUp:
	{
		ChannelEnd();
		break;
	}
	default:
		break;
	}
}

void UBGMChannel::ChangeChannelPauseState(FName OtherChannelName, bool IsPause/* = true*/)
{
	if (CurAudioCom)
	{
		if (IsPause)
		{
			OtherChannelPause.Add(OtherChannelName);
		}
		else
		{
			OtherChannelPause.Remove(OtherChannelName);
		}
		//只要有一个还在赋予暂停，那么仍然暂停
		CurAudioCom->SetPaused(OtherChannelPause.Num() > 0);
	}
}

void USoundSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

UAudioComponent* USoundSubsystem::PlaySound_2D(const UObject* WorldContextObject, AActor* Player, FName RowName, FString ResourceNameOrIndex, FGameplayTag SoundTag, FRDTC_PlaySoundSetting OverridePlaySoundSetting, bool IsUseParameter/* = true*/)
{
	FResourceProperty_SoundInfo Sound;
	if (ResourceNameOrIndex.IsEmpty())
	{
		ResourceNameOrIndex = "Default";
	}

	if (UResourceBPFunctionLibrary::GetResourceFromString_Sound(RowName,ResourceNameOrIndex,Sound))
	{
		FRDTC_PlaySoundSetting UseSetting = Sound.PlaySoundSetting.Override(OverridePlaySoundSetting);

		float Volume = GetOtherPlayerVolume(Player, UseSetting.VolumeMultiplier);

		USoundConcurrency* ConcurrencySettings = nullptr;//并发性设置
		for (const TSoftObjectPtr<USoundConcurrency>& SoftPtr : UseSetting.ConcurrencySettings)
		{
			if (USoundConcurrency* LoadedAsset = SoftPtr.LoadSynchronous())//尝试同步加载（确保获取到有效对象）
			{
				ConcurrencySettings = LoadedAsset;
				break; //找到第一个有效的就退出
			}
		}

		UAudioComponent* AudioComponent = UGameplayStatics::SpawnSound2D(WorldContextObject, Sound.Sound.LoadSynchronous(), Volume,
			UseSetting.PitchMultiplier, UseSetting.StartTime, ConcurrencySettings, UseSetting.bPersistAcrossLevelTransition, UseSetting.bAutoDestroy);
		AddAudioComponentToPlayer(Player, SoundTag, AudioComponent);
		if (IsUseParameter)
		{
			SetParameter(AudioComponent, Sound);
		}
		DisplaySubtitle(Player, AudioComponent, FRDTC_SoundSubtitle(Sound.IsDisplaySubtitle, Sound.IsUseSoundWaveSubtitleCue, Sound.SubtitleCue));
		return AudioComponent;
	}
	return nullptr;
}

UAudioComponent* USoundSubsystem::PlaySound_Location(const UObject* WorldContextObject, AActor* Player, FName RowName, FString ResourceNameOrIndex, FGameplayTag SoundTag, FVector Location, FRDTC_PlaySoundSetting OverridePlaySoundSetting, bool IsUseParameter, FRotator Rotation)
{
	FResourceProperty_SoundInfo Sound;
	if (ResourceNameOrIndex.IsEmpty())
	{
		ResourceNameOrIndex = "Default";
	}
	if (UResourceBPFunctionLibrary::GetResourceFromString_Sound(RowName,ResourceNameOrIndex,Sound))
	{
		FRDTC_PlaySoundSetting UseSetting = Sound.PlaySoundSetting.Override(OverridePlaySoundSetting);

		float Volume = GetOtherPlayerVolume(Player, UseSetting.VolumeMultiplier);

		USoundConcurrency* ConcurrencySettings = nullptr;//并发性设置
		for (const TSoftObjectPtr<USoundConcurrency>& SoftPtr : UseSetting.ConcurrencySettings)
		{
			if (USoundConcurrency* LoadedAsset = SoftPtr.LoadSynchronous())//尝试同步加载（确保获取到有效对象）
			{
				ConcurrencySettings = LoadedAsset;
				break; //找到第一个有效的就退出
			}
		}

		UAudioComponent* AudioComponent = UGameplayStatics::SpawnSoundAtLocation(WorldContextObject,Sound.Sound.LoadSynchronous(),Location,Rotation, Volume,
			UseSetting.PitchMultiplier, UseSetting.StartTime, UseSetting.AttenuationSettings.LoadSynchronous(),ConcurrencySettings, UseSetting.bAutoDestroy);

		AddAudioComponentToPlayer(Player, SoundTag, AudioComponent);
		if (IsUseParameter)
		{
			SetParameter(AudioComponent, Sound);
		}
		DisplaySubtitle(Player, AudioComponent, FRDTC_SoundSubtitle(Sound.IsDisplaySubtitle, Sound.IsUseSoundWaveSubtitleCue, Sound.SubtitleCue));
		return AudioComponent;
	}
	return nullptr;
}

UAudioComponent* USoundSubsystem::PlaySound_Attached(AActor* Player, FName RowName, FString ResourceNameOrIndex, FGameplayTag SoundTag,  USceneComponent* AttachToComponent, FRDTC_PlaySoundSetting OverridePlaySoundSetting
, bool IsUseParameter, FName AttachPointName, FVector Location, FRotator Rotation, EAttachLocation::Type LocationType,bool bStopWhenAttachedToDestroyed)
{
	FResourceProperty_SoundInfo Sound;
	if (ResourceNameOrIndex.IsEmpty())
	{
		ResourceNameOrIndex = "Default";
	}
	if (UResourceBPFunctionLibrary::GetResourceFromString_Sound(RowName,ResourceNameOrIndex,Sound))
	{
		FRDTC_PlaySoundSetting UseSetting = Sound.PlaySoundSetting.Override(OverridePlaySoundSetting);

		float Volume = GetOtherPlayerVolume(Player, UseSetting.VolumeMultiplier);

		USoundConcurrency* ConcurrencySettings = nullptr;//并发性设置
		for (const TSoftObjectPtr<USoundConcurrency>& SoftPtr : UseSetting.ConcurrencySettings)
		{
			if (USoundConcurrency* LoadedAsset = SoftPtr.LoadSynchronous())//尝试同步加载（确保获取到有效对象）
			{
				ConcurrencySettings = LoadedAsset;
				break; //找到第一个有效的就退出
			}
		}

		UAudioComponent* AudioComponent = UGameplayStatics::SpawnSoundAttached(Sound.Sound.LoadSynchronous(),AttachToComponent,AttachPointName,Location,Rotation,LocationType,
		bStopWhenAttachedToDestroyed, Volume, UseSetting.PitchMultiplier, UseSetting.StartTime, UseSetting.AttenuationSettings.LoadSynchronous(),ConcurrencySettings, UseSetting.bAutoDestroy);
		AddAudioComponentToPlayer(Player, SoundTag, AudioComponent);
		if (IsUseParameter)
		{
			SetParameter(AudioComponent, Sound);
		}
		DisplaySubtitle(Player, AudioComponent, FRDTC_SoundSubtitle(Sound.IsDisplaySubtitle, Sound.IsUseSoundWaveSubtitleCue, Sound.SubtitleCue));
		return AudioComponent;
	}
	return nullptr;
}

TArray<UAudioComponent*> USoundSubsystem::PlaySound_2D_Array(const UObject* WorldContextObject, AActor* Player, TArray<FResourceProperty_SoundAssetTag> SoundAssetTags, FGameplayTag SoundTag, FRDTC_PlaySoundSetting OverridePlaySoundSetting, bool IsRandomPlayOneSound/* = true*/, bool IsUseParameter/* = true*/)
{
	TArray<UAudioComponent*> AudioComponents;
	if (SoundAssetTags.Num() > 0)
	{
		if (IsRandomPlayOneSound)
		{
			FResourceProperty_SoundAssetTag SoundAssetTag = SoundAssetTags[UKismetMathLibrary::RandomIntegerInRange(0, SoundAssetTags.Num() - 1)];
			SoundAssetTags.Empty();
			SoundAssetTags.Add(SoundAssetTag);
		}

		for (int32 i = 0; i < SoundAssetTags.Num(); i++)
		{
			AudioComponents.Add(PlaySound_2D(WorldContextObject, Player, SoundAssetTags[i].RowName, SoundAssetTags[i].ResourceNameOrIndex, SoundTag, OverridePlaySoundSetting, IsUseParameter));
		}
	}
	return AudioComponents;
}

TArray<UAudioComponent*> USoundSubsystem::PlaySound_Location_Array(const UObject* WorldContextObject, AActor* Player, TArray<FResourceProperty_SoundAssetTag> SoundAssetTags, FGameplayTag SoundTag, FVector Location, FRDTC_PlaySoundSetting OverridePlaySoundSetting, bool IsRandomPlayOneSound, bool IsUseParameter, FRotator Rotation)
{
	TArray<UAudioComponent*> AudioComponents;
	if (SoundAssetTags.Num() > 0)
	{
		if (IsRandomPlayOneSound)
		{
			FResourceProperty_SoundAssetTag SoundAssetTag = SoundAssetTags[UKismetMathLibrary::RandomIntegerInRange(0, SoundAssetTags.Num() - 1)];
			SoundAssetTags.Empty();
			SoundAssetTags.Add(SoundAssetTag);
		}

		for (int32 i = 0; i < SoundAssetTags.Num(); i++)
		{
			AudioComponents.Add(PlaySound_Location(WorldContextObject, Player, SoundAssetTags[i].RowName, SoundAssetTags[i].ResourceNameOrIndex, SoundTag, Location, OverridePlaySoundSetting, IsUseParameter));
		}
	}
	return AudioComponents;
}

TArray<UAudioComponent*> USoundSubsystem::PlaySound_Attached_Array(AActor* Player, TArray<FResourceProperty_SoundAssetTag> SoundAssetTags, FGameplayTag SoundTag, USceneComponent* AttachToComponent, FRDTC_PlaySoundSetting OverridePlaySoundSetting, bool IsRandomPlayOneSound, bool IsUseParameter, FName AttachPointName, FVector Location, FRotator Rotation, EAttachLocation::Type LocationType, bool bStopWhenAttachedToDestroyed)
{
	TArray<UAudioComponent*> AudioComponents;
	if (SoundAssetTags.Num() > 0)
	{
		if (IsRandomPlayOneSound)
		{
			FResourceProperty_SoundAssetTag SoundAssetTag = SoundAssetTags[UKismetMathLibrary::RandomIntegerInRange(0, SoundAssetTags.Num() - 1)];
			SoundAssetTags.Empty();
			SoundAssetTags.Add(SoundAssetTag);
		}

		for (int32 i = 0; i < SoundAssetTags.Num(); i++)
		{
			AudioComponents.Add(PlaySound_Attached(Player, SoundAssetTags[i].RowName, SoundAssetTags[i].ResourceNameOrIndex, SoundTag,AttachToComponent, OverridePlaySoundSetting, IsUseParameter,AttachPointName,Location,Rotation,LocationType,bStopWhenAttachedToDestroyed));
		}
	}
	return AudioComponents;
}

FBGMInfo USoundSubsystem::GetBGMInfoFromSoundAssetTag(FResourceProperty_SoundAssetTag SoundAssetTag)
{
	FBGMInfo* BGMInfo = nullptr;
	UResourceBPFunctionLibrary::GetResourceFromString_BGM(SoundAssetTag.RowName, SoundAssetTag.ResourceNameOrIndex, *BGMInfo);
	return *BGMInfo;
}

FResourceProperty_SoundInfo USoundSubsystem::GetSoundInfoFromSoundAssetTag(FResourceProperty_SoundAssetTag SoundAssetTag)
{
	FResourceProperty_SoundInfo* SoundInfo = nullptr;
	UResourceBPFunctionLibrary::GetResourceFromString_Sound(SoundAssetTag.RowName, SoundAssetTag.ResourceNameOrIndex, *SoundInfo);
	return *SoundInfo;
}

float USoundSubsystem::GetOtherPlayerVolume(AActor* Player, float CurVolumeMultiplier)
{
	float Volume = CurVolumeMultiplier;
	if (Player && Player->GetLocalRole() != ENetRole::ROLE_AutonomousProxy)//其他玩家播放的音效
	{
		Volume *= UResourcesConfig::GetInstance()->OtherPlayerSoundVolume;
	}
	return Volume;
}

void USoundSubsystem::AddAudioComponentToPlayer(AActor* Player, FGameplayTag SoundTag, UAudioComponent* AudioComponent)
{
	if (Player)
	{
		USoundComponent* SoundComponent = Cast<USoundComponent>(Player->GetComponentByClass(USoundComponent::StaticClass()));
		if (SoundComponent)
		{
			SoundComponent->AddPlayAudioComponent(SoundTag, AudioComponent);
		}
	}
}

void USoundSubsystem::LoadBGMSound(TArray<FName> RowName)
{
	bIsLoadBGMCompleted = false;
	TArray<FSoftObjectPath> AllPath;
	
	FResourceProperty_BGM BGM;
	for (FName Row : RowName)
	{
		if (UResourceBPFunctionLibrary::GetResourceProperty_BGM(Row,BGM))
		{
			for (TPair<FString, FBGMInfo> pair : BGM.BGM)
			{
				AllPath.Add(pair.Value.SoundBase.ToSoftObjectPath());
			}
		}
	}
	UAssetManager* AssetManager = UAssetManager::GetIfInitialized(); //UAssetManager::GetIfValid();//UE4
	AssetManager->GetStreamableManager().RequestAsyncLoad(AllPath, FStreamableDelegate::CreateUObject(this, &USoundSubsystem::LoadBGMCompleted));
}

void USoundSubsystem::LoadBGMCompleted()
{
	bIsLoadBGMCompleted = true;
	if (bIsLoadSoundompleted)
	{
		LoadCompleted();
	}
}

void USoundSubsystem::LoadSound(TArray<UDataTable*> DataTable)
{
	bIsLoadSoundompleted = false;
	TArray<FSoftObjectPath> AllPath;

	for (UDataTable* DT : DataTable)
	{
		if (DT)
		{
			TArray<FResourceProperty_Sound*> AllSound;
			DT->GetAllRows<FResourceProperty_Sound>(TEXT("All"),AllSound);
			for (FResourceProperty_Sound* Sound : AllSound)
			{
				for (TPair<FString, FResourceProperty_SoundInfo> pair : Sound->Sound)
				{
					AllPath.Add(pair.Value.Sound.ToSoftObjectPath());
				}
			}
		}
	}
	UAssetManager* AssetManager = UAssetManager::GetIfInitialized(); //UAssetManager::GetIfValid();//UE4
	AssetManager->GetStreamableManager().RequestAsyncLoad(AllPath, FStreamableDelegate::CreateUObject(this, &USoundSubsystem::LoadSoundCompleted));
}

void USoundSubsystem::LoadSoundCompleted()
{
	bIsLoadSoundompleted = true;
	if (bIsLoadBGMCompleted)
	{
		LoadCompleted();
	}
}

void USoundSubsystem::LoadCompleted()
{
	
}

void USoundSubsystem::SetParameter(UAudioComponent* AudioComponent, FResourceProperty_SoundInfo SoundInfo)
{
	//MetaSound & SoundCue UE5
	UResourceBPFunctionLibrary::SetParameterFromAudioComponent(AudioComponent, SoundInfo.Parameters);
}

void USoundSubsystem::DisplaySubtitle(AActor* Player, UAudioComponent* AudioComponent, FRDTC_SoundSubtitle SoundSubtitle)
{
	//AudioComponent->IsPlaying() 某些情况下的设置可能导致音频无法播放（例如设置了某个音频最多发声源），不播放的音频不应该出现字幕
	if (AudioComponent && AudioComponent->IsPlaying() && SoundSubtitle.bIsDisplaySubtitle)
	{
		TArray<FSubtitleCue> SubtitleCue;
		if (SoundSubtitle.bIsUseSoundWaveSubtitleCue)
		{
			AudioComponent->OnQueueSubtitles.BindUFunction(this,"GetSubtitles");
			CurSubtitlesPlayer = Player;
			CurSubtitlesAudioComponent = AudioComponent;
		}
		else
		{
			SubtitleCue = SoundSubtitle.SubtitleCue;
		}

		if (SubtitleCue.Num() > 0)
		{
			AudioComponent->OnAudioFinishedNative.AddUObject(this, &USoundSubsystem::AudioFinished);
			if (GetSubtitlesPanel() && GetSubtitlesPanel()->Implements<USubtitleWidgetInteract>())
			{
				SubtitlesAudioComponent.Add(AudioComponent, ISubtitleWidgetInteract::Execute_ShowSubtitleCue(GetSubtitlesPanel(), Player, SubtitleCue));
			}
		}
	}
}

void USoundSubsystem::AudioFinished(UAudioComponent* AudioComponent)
{
	if (SubtitlesAudioComponent.Contains(AudioComponent) && SubtitlesAudioComponent[AudioComponent] &&
		GetSubtitlesPanel() && GetSubtitlesPanel()->Implements<USubtitleWidgetInteract>())
	{
		ISubtitleWidgetInteract::Execute_HideSubtitles(GetSubtitlesPanel(), SubtitlesAudioComponent[AudioComponent]);
		SubtitlesAudioComponent.Remove(AudioComponent);
	}
}

void USoundSubsystem::GetSubtitles(const TArray<FSubtitleCue>& Subtitles, float CueDuration)
{
	if (Subtitles.Num() > 0 && CurSubtitlesAudioComponent)
	{
		CurSubtitlesAudioComponent->OnAudioFinishedNative.AddUObject(this, &USoundSubsystem::AudioFinished);
		if (GetSubtitlesPanel() && GetSubtitlesPanel()->Implements<USubtitleWidgetInteract>())
		{
			SubtitlesAudioComponent.Add(CurSubtitlesAudioComponent, ISubtitleWidgetInteract::Execute_ShowSubtitleCue(GetSubtitlesPanel(), CurSubtitlesPlayer, Subtitles));
		}
	}
}

UUserWidget* USoundSubsystem::GetSubtitlesPanel()
{
	if (!SubtitlesPanel)
	{
		TSubclassOf<UUserWidget> UIClass = UResourcesConfig::GetInstance()->SubtitlesWidgetClass.LoadSynchronous();
		if (UIClass)
		{
			UWorld* world = GWorld;//直接使用GetWorld()外部通过const_cast<USoundSubsystem*>(GetDefault<USoundSubsystem>());获取时有几率获取不到导致字幕UI创建不成功
			SubtitlesPanel = CreateWidget(world, UIClass);
			if (SubtitlesPanel)
			{
				SubtitlesPanel->AddToViewport(UResourcesConfig::GetInstance()->SubtitlesWidgetZOrder);
			}
		}
	}
	return SubtitlesPanel;
}

UBGMChannel* USoundSubsystem::PushBGMToChannel(FName RowName, FString ResourceNameOrIndex)
{
	FBGMInfo BGMInfo;
	if (UResourceBPFunctionLibrary::GetResourceFromString_BGM(RowName, ResourceNameOrIndex, BGMInfo))
	{
		return PushBGMToChannelOfInfo(BGMInfo);
	}
	return nullptr;
}

UBGMChannel* USoundSubsystem::PushBGMToChannel_CustomChannelName(FName ChannelName, FName RowName, FString ResourceNameOrIndex)
{
	FBGMInfo BGMInfo;
	if (UResourceBPFunctionLibrary::GetResourceFromString_BGM(RowName, ResourceNameOrIndex, BGMInfo))
	{
		return PushBGMToChannelOfInfo_CustomChannelName(ChannelName, BGMInfo);
	}
	return nullptr;
}

UBGMChannel* USoundSubsystem::PushBGMToChannelOfInfo(FBGMInfo PushInfo)
{
	UBGMChannel* BGMChannel;
	if (AllBGMChannel.Num() == 0)//初始创建BGM通道
	{
		for (FName& BGMChannelName : UResourcesConfig::GetInstance()->InitCreateBGMChannelNames)
		{
			BGMChannel = NewObject<UBGMChannel>(this);
			if (BGMChannel)
			{
				AllBGMChannel.Add(BGMChannelName, BGMChannel);
				BGMChannel->ChannelName = BGMChannelName;
				BGMChannel->SoundSubsystem = this;
			}
		}
	}

	if (AllBGMChannel.Contains(PushInfo.BGMChannelName))
	{
		AllBGMChannel[PushInfo.BGMChannelName]->PushBGMOfInfo(PushInfo);
		BGMChannel = AllBGMChannel[PushInfo.BGMChannelName];
	}
	else
	{
		BGMChannel = NewObject<UBGMChannel>(this);
		if (BGMChannel)
		{
			BGMChannel->ChannelName = PushInfo.BGMChannelName;
			BGMChannel->SoundSubsystem = this;
			BGMChannel->PushBGMOfInfo(PushInfo);
			AllBGMChannel.Add(PushInfo.BGMChannelName, BGMChannel);
		}
	}

	//该通道影响的其他通道
	if (BGMChannel)
	{
		if (PushInfo.bChangeAllChannel)//是否影响当前的全部通道
		{
			for (TPair<FName, UBGMChannel*>& Pair : AllBGMChannel)
			{
				if (Pair.Key != PushInfo.BGMChannelName)//剔除自身的通道
				{
					Pair.Value->ChangeBGMChanel(PushInfo.BGMChannelName, PushInfo.ChangeAllBGMChanelInfo);
				}
			}
		}
		else
		{
			for (TPair<FName, FChangeBGMChanelInfo>& Pair : PushInfo.ChannelOtherBGMChanelInfo)
			{
				if (AllBGMChannel.Contains(Pair.Key))
				{
					AllBGMChannel[Pair.Key]->ChangeBGMChanel(PushInfo.BGMChannelName, Pair.Value);
				}
			}
		}
	}
	BGMChannelChange.Broadcast();
	return BGMChannel;
}

UBGMChannel* USoundSubsystem::PushBGMToChannelOfInfo_CustomChannelName(FName ChannelName, FBGMInfo PushInfo)
{
	UBGMChannel* BGMChannel;
	if (AllBGMChannel.Contains(ChannelName))
	{
		BGMChannel = AllBGMChannel[ChannelName];
		BGMChannel->PushBGMOfInfo(PushInfo);
	}
	else
	{
		BGMChannel = NewObject<UBGMChannel>(this);
		if (BGMChannel)
		{
			BGMChannel->PushBGMOfInfo(PushInfo);
			AllBGMChannel.Add(ChannelName, BGMChannel);
		}
	}
	return BGMChannel;
}

void USoundSubsystem::PopBGMChannel(FName ChannelName)
{
	if (AllBGMChannel.Contains(ChannelName))
	{
		PopBGMChannelOfChannel(AllBGMChannel[ChannelName]);
	}
}

void USoundSubsystem::PopBGMChannelOfChannel(UBGMChannel* Channel)
{
	if (Channel)
	{
		Channel->ChannelEnd();
		BGMChannelChange.Broadcast();
	}
}

void USoundSubsystem::SomeChannelPopBGM(UBGMChannel* Channel)
{
	//某个推送结束了：1.该通道结束了	2.该通道有新的信息推送了，之前的信息影响的内容需要还原
	if (Channel)
	{
		for (TPair<FName, FChangeBGMChanelInfo> Pair : Channel->CurBGMInfo.ChannelOtherBGMChanelInfo)
		{
			if (AllBGMChannel.Contains(Pair.Key))//是否有被影响的通道
			{
				switch (Pair.Value.ChangeBGMChanelType)
				{
				case EChangeBGMChanelType::Volume:
				{
					AllBGMChannel[Pair.Key]->SetChannelVolume(Channel->ChannelName, 1.0f);
					break;
				}
				case EChangeBGMChanelType::Paused:
				{
					AllBGMChannel[Pair.Key]->ChangeChannelPauseState(Channel->ChannelName, false);
					break;
				}
				case EChangeBGMChanelType::PopUp:
				{
					//弹出无法复原
					break;
				}
				default:
					break;
				}
			}
		}
	}
}

void USoundSubsystem::TriggerSoundEvent(FName EventName, FCC_CompareInfo CompareParameter, TArray<UAudioComponent*>& SoundComs, TArray<UBGMChannel*>& BGMChannels)
{
	UDataTable* SoundEventDataTable = UResourcesConfig::GetInstance()->SoundEventDataTable.LoadSynchronous();
	if (SoundEventDataTable)
	{
		FSoundEventInfo* SoundEventInfo = SoundEventDataTable->FindRow<FSoundEventInfo>(EventName, TEXT(""));
		if (SoundEventInfo)
		{
			if (SoundEventInfo->bIsCompare)
			{
				FText FailText;
				int32 MaxPlayCount = SoundEventInfo->MaxPlaySoundCount;
				for (TPair<FString, FSoundCompareInfo>& SoundCompareInfo : SoundEventInfo->SoundCompareInfo)
				{
					if (MaxPlayCount != 0 && SoundCompareInfo.Value.BeCompareInfo.CompareResult(CompareParameter, FailText))
					{
						MaxPlayCount--;
						//声音事件的处理是否重载了
						if (SoundCompareInfo.Value.bIsOverrideSoundEventProcess)
						{
							SoundEventProcess(SoundCompareInfo.Value.OverrideSoundEventProcess, SoundComs, BGMChannels);
						}
						else
						{
							SoundEventProcess(SoundEventInfo->DefaultSoundEventProcess, SoundComs, BGMChannels);
						}
					}
				}
			}
			else
			{
				SoundEventProcess(SoundEventInfo->DefaultSoundEventProcess, SoundComs, BGMChannels);
			}
		}
	}
}

void USoundSubsystem::SoundEventProcess(FSoundEventProcess& ProcessInfo, TArray<UAudioComponent*>& SoundComs, TArray<UBGMChannel*>& BGMChannels)
{
	for (FName& BGMChannelName : ProcessInfo.AllPopBGMChannelName)
	{
		PopBGMChannel(BGMChannelName);
	}

	for (FResourceProperty_SoundAssetTag SoundAssetTag : ProcessInfo.SoundAssetTag)
	{
		switch (SoundAssetTag.SoundAssetType)
		{
		case ESoundAssetType::Sound:
		{
			SoundComs.Add(PlaySound_2D(this, UGameplayStatics::GetPlayerPawn(this, 0), SoundAssetTag.RowName, SoundAssetTag.ResourceNameOrIndex, FGameplayTag(), FRDTC_PlaySoundSetting()));
			break;
		}
		case ESoundAssetType::BGM:
		{
			BGMChannels.Add(PushBGMToChannel(SoundAssetTag.RowName, SoundAssetTag.ResourceNameOrIndex));
			break;
		}
		default:
			break;
		}
	}

	SoundEventAudioModulation(ProcessInfo.AudioModulationInfo);
}

void USoundSubsystem::SoundEventAudioModulation(FSoundEventAudioModulationInfo& ModulationInfo)
{
	if (ModulationInfo.bIsDeactivateAllBusMixes)
	{
		UAudioModulationStatics::DeactivateAllBusMixes(this);
	}
	else if (ModulationInfo.DeactivateMix)
	{
		UAudioModulationStatics::DeactivateBusMix(this, ModulationInfo.DeactivateMix.LoadSynchronous());
	}

	if (ModulationInfo.Mix)
	{
		USoundControlBusMix* SoundControlBusMix = ModulationInfo.Mix.LoadSynchronous();
		UAudioModulationStatics::ActivateBusMix(this, SoundControlBusMix);
		UAudioModulationStatics::UpdateMixByFilter(this, SoundControlBusMix, ModulationInfo.AddressFilter, ModulationInfo.ParamClassFilter, ModulationInfo.ParamFilter.LoadSynchronous(), ModulationInfo.Value, ModulationInfo.FadeTime);
	}
}


