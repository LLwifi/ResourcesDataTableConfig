// Fill out your copyright notice in the Description page of Project Settings.


#include "Dialogue/DialogueManager.h"
#include <WorldSubsystem/SoundSubsystem.h>
#include <Subsystems/SubsystemBlueprintLibrary.h>
#include "Components/AudioComponent.h"
#include <ResourcesConfig.h>
#include <Kismet/KismetSystemLibrary.h>
#include <ActorComponent/SoundComponent.h>

DEFINE_LOG_CATEGORY(Dialogue);

void UDialogueManager::SetSpeaker(TArray<FDialogueSpeaker> Speakers)
{
	AllSpeaker = Speakers;
}

void UDialogueManager::SetSectionDialogueInfo(FSectionDialogueInfo SectionDialogue)
{
	SectionDialogueInfo = SectionDialogue;
}

void UDialogueManager::SetSectionDialogueInfoFromRowName(FName RowName)
{
	DialogueRowName = RowName;
	UDataTable* SectionDialogueInfoDataTable = UResourcesConfig::GetInstance()->SectionDialogueInfoDataTable.LoadSynchronous();
	if(SectionDialogueInfoDataTable)
	{
		FSectionDialogueInfo* DialogueInfo = SectionDialogueInfoDataTable->FindRow<FSectionDialogueInfo>(RowName, TEXT(""));
		if (DialogueInfo)
		{
			SetSectionDialogueInfo(*DialogueInfo);
		}
	}
}

void UDialogueManager::ChangeAutoPlayState(bool IsAutoPlay)
{
	bIsAutoPlayNextDialogue = IsAutoPlay;
}

void UDialogueManager::StartDialogue(int32 StartDialogueIndex/* = 0*/)
{
	//设置对话名称
	for (size_t i = 0; i < AllSpeaker.Num(); i++)
	{
		if (AllSpeaker[i].SpeakerActor && AllSpeaker[i].SpeakerActor->Implements<USoundPlayer>() &&//继承了该接口的Actor
			SectionDialogueInfo.AllSpeakerName.IsValidIndex(i))//对话中配置了这个下标的名称
		{
			ISoundPlayer::Execute_SetDialogueName(AllSpeaker[i].SpeakerActor, SectionDialogueInfo.AllSpeakerName[i]);
		}
	}

	//对话次数记录
	if (DialoguePlayCount.Contains(SectionDialogueInfo.DialogueName))
	{
		DialoguePlayCount[SectionDialogueInfo.DialogueName]++;
	}
	else
	{
		DialoguePlayCount.Add(SectionDialogueInfo.DialogueName,1);
	}

	AActor* OuterActor = Cast<AActor>(GetOuter());
	if (OuterActor)
	{
		bDialogueIsServer = OuterActor->HasAuthority();
	}

	DialogueIndex = StartDialogueIndex - 1;
	if (bDialogueIsServer)//服务器
	{
		if (bIsAutoPlayNextDialogue)//只有自动播放才能一口气计算出整段对话的时长
		{
			float DelayTime = 0.0f;
			for (FOneDialogueInfo& Info : SectionDialogueInfo.SectionDialogueInfo)
			{
				DelayTime += Info.GetTotalDuration();
			}
			GetWorld()->GetTimerManager().SetTimer(ServerDialogueFinishedTimer, this, &UDialogueManager::EndDialogue, DelayTime);
			UE_LOG(Dialogue, Log, TEXT("StartDialogue_Server DelayTime = %f"), DelayTime);
		}
	}
	else//客户端
	{
		NextDialogue();
	}
}

void UDialogueManager::CurDialogueFinished()
{
	DialoguePlayState = EDialoguePlayState::EndPlay;
	OneDialogueEvent.Broadcast(this, DialogueIndex, DialoguePlayState);
	TriggerDialogueSoundEvent();
	UE_LOG(Dialogue, Log, TEXT("CurDialogueFinished CallBack"));
	if(bIsAutoPlayNextDialogue)
	{
		NextDialogue();
	}
}

void UDialogueManager::CurDialogueDelayFinished()
{
	DialoguePlayState = EDialoguePlayState::StartPlay;
	OneDialogueEvent.Broadcast(this, DialogueIndex, DialoguePlayState);
	TriggerDialogueSoundEvent();
	UE_LOG(Dialogue, Log, TEXT("CurDialogueDelayFinished"));
	EndCurOneDialogue();

	if (GetSoundSubsystem())
	{
		if (AllSpeaker.IsValidIndex(CurOneDialogueInfo.SpeakerIndex))
		{
			if (AllSpeaker[CurOneDialogueInfo.SpeakerIndex].SpeakerActor)
			{
				CurAudioComponent = GetSoundSubsystem()->PlaySound_Attached(AllSpeaker[CurOneDialogueInfo.SpeakerIndex].SpeakerActor, CurOneDialogueInfo.SoundAssetTag.RowName, CurOneDialogueInfo.SoundAssetTag.ResourceNameOrIndex, FGameplayTag(), AllSpeaker[CurOneDialogueInfo.SpeakerIndex].SpeakerActor->GetRootComponent());
			}
			else
			{
				CurAudioComponent = GetSoundSubsystem()->PlaySound_Location(this, nullptr, CurOneDialogueInfo.SoundAssetTag.RowName, CurOneDialogueInfo.SoundAssetTag.ResourceNameOrIndex, FGameplayTag(), AllSpeaker[CurOneDialogueInfo.SpeakerIndex].SpeakerLocation);
			}
			
			UE_LOG(Dialogue, Log, TEXT("PlaySound_Attached"));
		}
		else
		{
			CurAudioComponent = GetSoundSubsystem()->PlaySound_2D(this, nullptr, CurOneDialogueInfo.SoundAssetTag.RowName, CurOneDialogueInfo.SoundAssetTag.ResourceNameOrIndex, FGameplayTag());
			UE_LOG(Dialogue, Log, TEXT("PlaySound_2D"));
		}
	}

	if (CurAudioComponent)
	{
		if (CurOneDialogueInfo.Duraction > 0)
		{
			GetWorld()->GetTimerManager().SetTimer(CurDialogueFinishedTimer, this, &UDialogueManager::CurDialogueFinished, CurOneDialogueInfo.Duraction);
			UE_LOG(Dialogue, Log, TEXT("CurDialogueFinishedTimer = %f"), CurOneDialogueInfo.Duraction);
		}
		else
		{
			FScriptDelegate AudioFinished;
			AudioFinished.BindUFunction(this, "CurDialogueFinished");
			CurAudioComponent->OnAudioFinished.Add(AudioFinished);
			UE_LOG(Dialogue, Log, TEXT("BindFinished"));
		}
	}
}

void UDialogueManager::NextDialogue()
{
	DialogueIndex++;
	DialoguePlayState = EDialoguePlayState::Delay;
	OneDialogueEvent.Broadcast(this, DialogueIndex, DialoguePlayState);
	TriggerDialogueSoundEvent();
	UE_LOG(Dialogue, Log, TEXT("NextDialogue DialogueIndex = %d"), DialogueIndex);
	if (SectionDialogueInfo.GetOneDialogueInfoFromIndex(DialogueIndex, CurOneDialogueInfo))
	{
		if (CurOneDialogueInfo.DelayTime > 0)
		{
			GetWorld()->GetTimerManager().SetTimer(CurDialogueDelayFinishedTimer, this, &UDialogueManager::CurDialogueDelayFinished, CurOneDialogueInfo.DelayTime);
			UE_LOG(Dialogue, Log, TEXT("NextDialogue DelayTime = %f"), CurOneDialogueInfo.DelayTime);
		}
		else
		{
			CurDialogueDelayFinished();
		}
	}
	else
	{
		EndDialogue();
	}
}

void UDialogueManager::EndDialogue()
{
	bDialogueIsEnd = true;
	EndCurOneDialogue();
	DialogueEnd.Broadcast(this);
	UE_LOG(Dialogue, Log, TEXT("EndDialogue"));
}

void UDialogueManager::EndCurOneDialogue()
{
	//停止之前正在播放的对话。如果有的话
	FScriptDelegate AudioFinished;
	AudioFinished.BindUFunction(this, "CurDialogueFinished");
	if (CurAudioComponent)
	{
		CurAudioComponent->Stop();
		if (CurOneDialogueInfo.Duraction > 0)
		{
			GetWorld()->GetTimerManager().ClearTimer(CurDialogueFinishedTimer);
		}
		else
		{
			CurAudioComponent->OnAudioFinished.Remove(AudioFinished);
		}
	}
}

USoundSubsystem* UDialogueManager::GetSoundSubsystem()
{
	if (!SoundSubsystem)
	{
		SoundSubsystem = Cast<USoundSubsystem>(USubsystemBlueprintLibrary::GetWorldSubsystem(this, USoundSubsystem::StaticClass()));
	}
	return SoundSubsystem;
}

FCC_CompareInfo UDialogueManager::GetCurSoundEventCompareInfo()
{
	FCC_CompareInfo CompareInfo = CurOneDialogueInfo.DialogueSoundEventCompareInfo;
	CompareInfo.CompareString.Add(SectionDialogueInfo.DialogueName.ToString());
	CompareInfo.CompareString.Add(FString::FromInt((int32)DialoguePlayState));
	CompareInfo.CompareFloat.Add(DialogueIndex);
	return CompareInfo;
}

void UDialogueManager::TriggerDialogueSoundEvent()
{
	if (GetSoundSubsystem() && !bDialogueIsServer)//客户端才需要触发音效事件 服务器无需播放音效
	{
		TArray<UAudioComponent*> SoundComs;
		TArray<UBGMChannel*> BGMChannels;
		GetSoundSubsystem()->TriggerSoundEvent(FName("DialogueSoundEvent"), GetCurSoundEventCompareInfo(), SoundComs, BGMChannels);
	}
}

bool UDialogueManager::DialogueIsPlaying()
{
	return DialoguePlayState == EDialoguePlayState::StartPlay;
}

bool UDialogueManager::DialogueIsEnd()
{
	return bDialogueIsEnd;
}

