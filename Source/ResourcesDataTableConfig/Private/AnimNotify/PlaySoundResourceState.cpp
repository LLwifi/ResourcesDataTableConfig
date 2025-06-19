// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/PlaySoundResourceState.h"
#include <Subsystems/SubsystemBlueprintLibrary.h>
#include "WorldSubsystem/SoundSubsystem.h"
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "Components/AudioComponent.h"

FString UPlaySoundResourceState::GetNotifyName_Implementation() const
{

	if (SoundAssetTag.Num() > 0)
	{
		FString Name;
		for (FResourceProperty_SoundAssetTag AssetTag : SoundAssetTag)
		{
			Name += AssetTag.RowName.ToString() + "-" + AssetTag.ResourceNameOrIndex + "|";
		}
		return Name;
	}
	return Super::GetNotifyName_Implementation();
}

void UPlaySoundResourceState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	//Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	AnimOwner = MeshComp->GetOwner();
	World = MeshComp->GetWorld();
#if WITH_EDITORONLY_DATA
	if (World && World->WorldType == EWorldType::EditorPreview)
	{
		Play(MeshComp);//编辑器始终能听见声音
	}
	else
#endif
	{
		if (!bUseActorVelocityCheck)//如果不检测速度，那么直接播放
		{
			Play(MeshComp);
		}
	}
}

void UPlaySoundResourceState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	if (bUseActorVelocityCheck && AnimOwner)
	{
		float CurVelocity = UKismetMathLibrary::VSize(AnimOwner->GetVelocity());
		if (bVelocityGreaterThan)
		{
			if (CurVelocity > ActorVelocity && !bIsPlay_Tick)//达到条件 && 还没开始播放声音
			{
				Play(MeshComp);
				bIsPlay_Tick = true;
			}
			else if (CurVelocity < ActorVelocity && bIsPlay_Tick)
			{
				EndPlay();
			}
		}
	}
}

void UPlaySoundResourceState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	EndPlay();
}

USoundSubsystem* UPlaySoundResourceState::GetSoundSubsystem()
{
	if (!SoundSubsystem)
	{
		SoundSubsystem = Cast<USoundSubsystem>(USubsystemBlueprintLibrary::GetWorldSubsystem(GWorld, USoundSubsystem::StaticClass()));
	}
	return SoundSubsystem;
}

TObjectPtr<USoundBase> UPlaySoundResourceState::GetSound(FName SoundRowName, FString SoundResourceNameOrIndex)
{
	FResourceProperty_SoundInfo SoftSound;
	if (UResourceBPFunctionLibrary::GetResourceFromString_Sound(SoundRowName, SoundResourceNameOrIndex, SoftSound))
	{
		return SoftSound.Sound.LoadSynchronous();
	}
	return nullptr;
}

void UPlaySoundResourceState::SetParameter(UAudioComponent* AudioComponent, FName SoundRowName, FString SoundResourceNameOrIndex)
{
	if (AudioComponent)
	{
		if (bUseAnimNotifyParameter)
		{
			UResourceBPFunctionLibrary::SetParameterFromAudioComponent(AudioComponent, SoundParameters);
		}
		else
		{
			FResourceProperty_SoundInfo SoundInfo;
			if (UResourceBPFunctionLibrary::GetResourceFromString_Sound(SoundRowName, SoundResourceNameOrIndex, SoundInfo))
			{
				UResourceBPFunctionLibrary::SetParameterFromAudioComponent(AudioComponent, SoundInfo.Parameters);
			}
		}
	}
}

void UPlaySoundResourceState::Play(USkeletalMeshComponent* MeshComp)
{
	TArray<FResourceProperty_SoundAssetTag> UseSoundAssetTags = SoundAssetTag;
	if (IsRandomPlayOneSound && UseSoundAssetTags.Num() > 1)
	{
		FResourceProperty_SoundAssetTag OneSoundAssetTag = UseSoundAssetTags[UKismetMathLibrary::RandomIntegerInRange(0, UseSoundAssetTags.Num() - 1)];
		UseSoundAssetTags.Empty();
		UseSoundAssetTags.Add(OneSoundAssetTag);
	}
	TArray<UAudioComponent*> NewSoundComponent;

#if WITH_EDITORONLY_DATA
	if (World && World->WorldType == EWorldType::EditorPreview)
	{
		for (int32 i = 0; i < UseSoundAssetTags.Num(); i++)
		{
			NewSoundComponent.Add(UGameplayStatics::SpawnSound2D(World, GetSound(UseSoundAssetTags[i].RowName, UseSoundAssetTags[i].ResourceNameOrIndex), VolumeMultiplier, PitchMultiplier));
		}
	}
	else
#endif
	{
		if (bFollow)
		{
			if (World && World->HasBegunPlay())
			{
				NewSoundComponent = GetSoundSubsystem()->PlaySound_Attached_Array(MeshComp->GetOwner(), UseSoundAssetTags, SoundTag, MeshComp, IsRandomPlayOneSound, !bUseAnimNotifyParameter, AttachName, FVector(ForceInit), FRotator(), EAttachLocation::SnapToTarget, false, VolumeMultiplier, PitchMultiplier);
			}
			else
			{
				for (int32 i = 0; i < UseSoundAssetTags.Num(); i++)
				{
					NewSoundComponent.Add(UGameplayStatics::SpawnSoundAttached(GetSound(UseSoundAssetTags[i].RowName, UseSoundAssetTags[i].ResourceNameOrIndex), MeshComp, AttachName, FVector(ForceInit), EAttachLocation::SnapToTarget, false, VolumeMultiplier, PitchMultiplier));
				}
			}
		}
		else
		{
			if (World && World->HasBegunPlay())
			{
				NewSoundComponent = GetSoundSubsystem()->PlaySound_Location_Array(MeshComp->GetWorld(), MeshComp->GetOwner(), UseSoundAssetTags, SoundTag, MeshComp->GetComponentLocation(), IsRandomPlayOneSound, !bUseAnimNotifyParameter, FRotator(), VolumeMultiplier, PitchMultiplier);
			}
			else
			{
				for (int32 i = 0; i < UseSoundAssetTags.Num(); i++)
				{
					NewSoundComponent.Add(UGameplayStatics::SpawnSoundAtLocation(MeshComp->GetWorld(), GetSound(UseSoundAssetTags[i].RowName, UseSoundAssetTags[i].ResourceNameOrIndex), MeshComp->GetComponentLocation(), FRotator::ZeroRotator, VolumeMultiplier, PitchMultiplier));
				}
			}
		}
	}

	bool NewSoundComponentIsNull = true;
	for (int32 i = 0; i < NewSoundComponent.Num(); i++)
	{
		if (NewSoundComponent[i])
		{
			NewSoundComponentIsNull = false;
			SetParameter(NewSoundComponent[i], UseSoundAssetTags[i].RowName, UseSoundAssetTags[i].ResourceNameOrIndex);
			if (bIsFadeIn)
			{
				NewSoundComponent[i]->FadeIn(FadeInTime);
			}
		}
	}

	if (!NewSoundComponentIsNull)
	{
		for (int32 i = 0; i < SoundComponent.Num(); i++)
		{
			if (SoundComponent[i])
			{
				if (bIsFadeOut)
				{
					SoundComponent[i]->FadeOut(FadeOutTime, 0.0f);
				}
				else
				{
					SoundComponent[i]->Stop();
				}
			}
		}
		SoundComponent.Empty();
		SoundComponent = NewSoundComponent;
	}
}

void UPlaySoundResourceState::EndPlay()
{
	for (int32 i = 0; i < SoundComponent.Num(); i++)
	{
		if (SoundComponent[i])
		{
			if (bIsFadeOut)
			{
				SoundComponent[i]->FadeOut(FadeOutTime, 0.0f);
			}
			else
			{
				SoundComponent[i]->Stop();
			}
		}
	}
	SoundComponent.Empty();
	bIsPlay_Tick = false;
}
