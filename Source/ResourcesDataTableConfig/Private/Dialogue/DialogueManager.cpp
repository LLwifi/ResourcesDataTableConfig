// Fill out your copyright notice in the Description page of Project Settings.


#include "Dialogue/DialogueManager.h"
#include <WorldSubsystem/SoundSubsystem.h>
#include <Subsystems/SubsystemBlueprintLibrary.h>
#include "Components/AudioComponent.h"
#include <ResourcesConfig.h>
#include <Kismet/KismetSystemLibrary.h>

DEFINE_LOG_CATEGORY(Dialogue);

void UDialogueManager::SetSpeaker(TArray<AActor*> Speakers)
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

void UDialogueManager::StartDialogue()
{
	AActor* OuterActor = Cast<AActor>(GetOuter());
	if (OuterActor)
	{
		bDialogueIsServer = OuterActor->HasAuthority();
	}

	DialogueIndex = -1;
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
	UE_LOG(Dialogue, Log, TEXT("CurDialogueFinished CallBack"));
	if(bIsAutoPlayNextDialogue)
	{
		NextDialogue();
	}
}

void UDialogueManager::CurDialogueDelayFinished()
{
	UE_LOG(Dialogue, Log, TEXT("CurDialogueDelayFinished"));
	EndCurOneDialogue();

	USoundSubsystem* SoundSubsystem = Cast<USoundSubsystem>(USubsystemBlueprintLibrary::GetWorldSubsystem(this, USoundSubsystem::StaticClass()));
	if (SoundSubsystem)
	{
		if (AllSpeaker.IsValidIndex(CurOneDialogueInfo.SpeakerIndex))
		{
			CurAudioComponent = SoundSubsystem->PlaySound_Attached(AllSpeaker[CurOneDialogueInfo.SpeakerIndex], CurOneDialogueInfo.SoundAssetTag.RowName, CurOneDialogueInfo.SoundAssetTag.ResourceNameOrIndex, FGameplayTag(), AllSpeaker[CurOneDialogueInfo.SpeakerIndex]->GetRootComponent());
			UE_LOG(Dialogue, Log, TEXT("PlaySound_Attached"));
		}
		else
		{
			CurAudioComponent = SoundSubsystem->PlaySound_2D(this, nullptr, CurOneDialogueInfo.SoundAssetTag.RowName, CurOneDialogueInfo.SoundAssetTag.ResourceNameOrIndex, FGameplayTag());
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

