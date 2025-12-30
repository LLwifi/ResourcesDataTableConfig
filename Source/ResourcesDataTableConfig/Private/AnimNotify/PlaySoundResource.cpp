// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/PlaySoundResource.h"
#include "Audio.h"
#include "ResourceBPFunctionLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Sound/SoundBase.h"
#include "Animation/AnimSequenceBase.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "Components/AudioComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlaySoundResource)			//ue5

#if WITH_EDITOR
#include "Logging/MessageLog.h"
#include "Misc/UObjectToken.h"
#endif
#include <Kismet/KismetMathLibrary.h>

UPlaySoundResource::UPlaySoundResource()
	: Super()
{
	CurPlaySoundResourceIndex = 0;
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(196, 142, 255, 255);
#endif // WITH_EDITORONLY_DATA
}

//UE4
//void UPlaySoundResource::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
//{
//	Super::Notify(MeshComp, Animation);
//
//	// Don't call super to avoid call back in to blueprints
//	if (GetSound() && MeshComp)
//	{
//		if (GetSound()->IsLooping())
//		{
//			UE_LOG(LogAudio, Warning, TEXT("PlaySound notify: Anim %s tried to spawn infinitely looping sound asset %s. Spawning suppressed."), *GetNameSafe(Animation), *GetNameSafe(Sound));
//			return;
//		}
//		UWorld* World = MeshComp->GetWorld();
//#if WITH_EDITORONLY_DATA
//		if (bPreviewIgnoreAttenuation && World && World->WorldType == EWorldType::EditorPreview)
//		{
//			SoundComponent = UGameplayStatics::SpawnSound2D(World, GetSound(), VolumeMultiplier, PitchMultiplier);
//		}
//		else
//#endif
//		{
//			if (bFollow)
//			{
//				if (World && World->HasBegunPlay())
//				{
//					SoundComponent = GetSoundSubsystem()->PlaySound_Attached(MeshComp->GetOwner(), RowName, GetResourceNameOrIndex(), SoundTag, MeshComp, !UseAnimNotifyParameter, AttachName, FVector(ForceInit),FRotator(), EAttachLocation::SnapToTarget, false, VolumeMultiplier, PitchMultiplier);
//				}
//				else
//				{
//					SoundComponent = UGameplayStatics::SpawnSoundAttached(GetSound(), MeshComp, AttachName, FVector(ForceInit), EAttachLocation::SnapToTarget, false, VolumeMultiplier, PitchMultiplier);
//				}
//			}
//			else
//			{
//				if (World && World->HasBegunPlay())
//				{
//					SoundComponent = GetSoundSubsystem()->PlaySound_Location(MeshComp->GetWorld(), MeshComp->GetOwner(), RowName, GetResourceNameOrIndex(), SoundTag, MeshComp->GetComponentLocation(), !UseAnimNotifyParameter, FRotator(), VolumeMultiplier, PitchMultiplier);
//				}
//				else
//				{
//					SoundComponent = UGameplayStatics::SpawnSoundAtLocation(MeshComp->GetWorld(), GetSound(), MeshComp->GetComponentLocation(),FRotator::ZeroRotator, VolumeMultiplier, PitchMultiplier);
//				}
//			}
//		}
//		SetParameter(SoundComponent);
//	}
//}

//ue5
void UPlaySoundResource::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
}

//UE5
 void UPlaySoundResource::Notify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
 {
	Super::Notify(MeshComp, Animation, EventReference);

 	PRAGMA_DISABLE_DEPRECATION_WARNINGS
     Notify(MeshComp, Animation);
 	PRAGMA_ENABLE_DEPRECATION_WARNINGS

	World = MeshComp->GetWorld();
	//不知道为什么服务器触发下面的逻辑会导致CurPlaySoundResourceIndex被运行2次，这里屏蔽一下服务器的执行
 	if (World && (World->WorldType != EWorldType::EditorPreview ? !UKismetSystemLibrary::IsServer(World) : true))
 	{
		TArray<FResourceProperty_SoundAssetTag> UseSoundAssetTags = GetAllSoundAssetTags();
		if (UseSoundAssetTags.Num() > 0)
		{
			FResourceProperty_SoundAssetTag SoundAssetTag;
			switch (PlaySoundResourceType)
			{
			case EPlaySoundResourceType::ArrayRandom:
			{
				SoundAssetTag = UseSoundAssetTags[UKismetMathLibrary::RandomIntegerInRange(0, UseSoundAssetTags.Num() - 1)];
				UseSoundAssetTags.Empty();
				UseSoundAssetTags.Add(SoundAssetTag);
				break;
			}
			case EPlaySoundResourceType::WeightRandom:
			{
				int32 MaxValue = 0;
				for (int32 i : PlaySoundResourceIntValue)
				{
					MaxValue += i;
				}
				int32 RandomValue = UKismetMathLibrary::RandomIntegerInRange(0, MaxValue);
				int32 LastValue = 0;
				for (int32 i = 0; i < PlaySoundResourceIntValue.Num(); i++)
				{
					if (RandomValue >= LastValue && RandomValue <= LastValue + PlaySoundResourceIntValue[i])
					{
						LastValue = i;
						break;
					}
					LastValue += PlaySoundResourceIntValue[i];
				}
				SoundAssetTag = UseSoundAssetTags[LastValue];
				UseSoundAssetTags.Empty();
				UseSoundAssetTags.Add(SoundAssetTag);
				break;
			}
			case EPlaySoundResourceType::ArrayIndexLoop:
			{
				SoundAssetTag = UseSoundAssetTags[GetCurPlaySoundResourceIndex()];
				AddCurPlaySoundResourceIndex(1);
				if (GetCurPlaySoundResourceIndex() >= UseSoundAssetTags.Num())
				{
					ChangeCurPlaySoundResourceIndex(0);
				}
				UseSoundAssetTags.Empty();
				UseSoundAssetTags.Add(SoundAssetTag);
				break;
			}
			case EPlaySoundResourceType::CustomIndexLoop:
			{
				SoundAssetTag = UseSoundAssetTags[PlaySoundResourceIntValue[GetCurPlaySoundResourceIndex()]];
				AddCurPlaySoundResourceIndex(1);
				if (GetCurPlaySoundResourceIndex() >= PlaySoundResourceIntValue.Num())
				{
					ChangeCurPlaySoundResourceIndex(0);
				}
				UseSoundAssetTags.Empty();
				UseSoundAssetTags.Add(SoundAssetTag);
				break;
			}
			case EPlaySoundResourceType::SimultaneouslyPlay:
			{
				//同时播放无需处理
				break;
			}
			default:
				break;
			}
		}

		//for (UAudioComponent* AudioCom : SoundComponent)
		//{
		//	if (AudioCom)
		//	{
		//		AudioCom->Stop();
		//	}
		//}
		//SoundComponent.Empty();

		//if (bNotifyIsSpawn)
		//{
		//	for (UAudioComponent* AudioCom : SoundComponent)
		//	{
		//		if (AudioCom)
		//		{
		//			AudioCom->Stop();
		//		}
		//	}
		//	SoundComponent.Empty();
		//}

 #if WITH_EDITORONLY_DATA
 		if (World->WorldType == EWorldType::EditorPreview)
 		{
			SoundComponent.Empty();
			USoundConcurrency* ConcurrencySettings = nullptr;//并发性设置
			for (const TSoftObjectPtr<USoundConcurrency>& SoftPtr : PlaySoundSetting.ConcurrencySettings)
			{
				if (USoundConcurrency* LoadedAsset = SoftPtr.LoadSynchronous())//尝试同步加载（确保获取到有效对象）
				{
					ConcurrencySettings = LoadedAsset;
					break; //找到第一个有效的就退出
				}
			}
			for (int32 i = 0; i < UseSoundAssetTags.Num(); i++)
			{
				UAudioComponent* AudioComponent = UGameplayStatics::SpawnSound2D(World, GetSound(UseSoundAssetTags[i].RowName, UseSoundAssetTags[i].ResourceNameOrIndex), PlaySoundSetting.VolumeMultiplier, PlaySoundSetting.PitchMultiplier, PlaySoundSetting.StartTime, ConcurrencySettings, PlaySoundSetting.bPersistAcrossLevelTransition, PlaySoundSetting.bAutoDestroy);
				SoundComponent.Add(AudioComponent);
				SetParameter(AudioComponent, UseSoundAssetTags[i].RowName, UseSoundAssetTags[i].ResourceNameOrIndex);
			}
			//if (bNotifyIsSpawn)
			//{
			//	for (int32 i = 0; i < UseSoundAssetTags.Num(); i++)
			//	{
			//		UAudioComponent* AudioComponent = UGameplayStatics::SpawnSound2D(World, GetSound(UseSoundAssetTags[i].RowName, UseSoundAssetTags[i].ResourceNameOrIndex), VolumeMultiplier, PitchMultiplier);
			//		NewSoundComponent.Add(AudioComponent);
			//		SetParameter(AudioComponent, UseSoundAssetTags[i].RowName, UseSoundAssetTags[i].ResourceNameOrIndex);
			//	}
			//}
			//else
			//{
			//	for (int32 i = 0; i < SoundComponent.Num(); i++)
			//	{
			//		SetParameter(SoundComponent[i], UseSoundAssetTags[i].RowName, UseSoundAssetTags[i].ResourceNameOrIndex);
			//	}
			//}
 		}
 		else
 #endif
 		{
 			if (bFollow)
 			{
				SoundComponent = GetSoundSubsystem()->PlaySound_Attached_Array(MeshComp->GetOwner(), UseSoundAssetTags, SoundTag, MeshComp, PlaySoundSetting, PlaySoundResourceType != EPlaySoundResourceType::SimultaneouslyPlay, !UseAnimNotifyParameter, AttachName, FVector(ForceInit), FRotator(), EAttachLocation::SnapToTarget, false);

			}
 			else
 			{
				SoundComponent = GetSoundSubsystem()->PlaySound_Location_Array(World, MeshComp->GetOwner(), UseSoundAssetTags, SoundTag, MeshComp->GetComponentLocation(), PlaySoundSetting, PlaySoundResourceType != EPlaySoundResourceType::SimultaneouslyPlay, !UseAnimNotifyParameter, FRotator());
 			}
 		}
 	}
 }

FString UPlaySoundResource::GetNotifyName_Implementation() const
{
	if (SoundAssetTags.Num() > 0)
	{
		FString Name;
		for (FResourceProperty_SoundAssetTag AssetTag : SoundAssetTags)
		{
			Name += AssetTag.RowName.ToString() + "-" + AssetTag.ResourceNameOrIndex + "|";
		}
		return Name;
	}
	else
	{
		FResourceProperty_SoundInfo SoftSound;
		FString FindName = NameOrIndex.IsEmpty() ? ResourceNameOrIndex : NameOrIndex;
		if (UResourceBPFunctionLibrary::GetResourceFromString_Sound(RowName, FindName, SoftSound))
		{
			USoundBase* SoundBase = SoftSound.Sound.LoadSynchronous();
			if (SoundBase)
			{
				return SoundBase->GetName();
			}
		}
	}
	return Super::GetNotifyName_Implementation();
}

#if WITH_EDITOR
void UPlaySoundResource::ValidateAssociatedAssets()
{
	//ue4
	//static const FName NAME_AssetCheck("AssetCheck");

	//if ((Sound != nullptr) && (Sound->IsLooping()))
	//{
	//	UObject* ContainingAsset = GetContainingAsset();

	//	FMessageLog AssetCheckLog(NAME_AssetCheck);

	//	const FText MessageLooping = FText::Format(
	//		NSLOCTEXT("AnimNotify", "Sound_ShouldNotLoop", "Sound {0} used in anim notify for asset {1} is set to looping, but the slot is a one-shot (it won't be played to avoid leaking an instance per notify)."),
	//		FText::AsCultureInvariant(Sound->GetPathName()),
	//		FText::AsCultureInvariant(ContainingAsset->GetPathName()));
	//	AssetCheckLog.Warning()
	//		->AddToken(FUObjectToken::Create(ContainingAsset))
	//		->AddToken(FTextToken::Create(MessageLooping));

	//	if (GIsEditor)
	//	{
	//		AssetCheckLog.Notify(MessageLooping, EMessageSeverity::Warning, /*bForce=*/ true);
	//	}
	//}

	//ue5
	static const FName NAME_AssetCheck("AssetCheck");
	
	if (GetSound() != nullptr && !GetSound()->IsOneShot())
	{
	UObject* ContainingAsset = GetContainingAsset();
	
	FMessageLog AssetCheckLog(NAME_AssetCheck);
	
	const FText MessageLooping = FText::Format(
	 	NSLOCTEXT("AnimNotify", "Sound_ShouldNotLoop", "Sound {0} used in anim notify for asset {1} is set to looping, but the slot is a one-shot (it won't be played to avoid leaking an instance per notify)."),
	 	FText::AsCultureInvariant(GetSound()->GetPathName()),
	 	FText::AsCultureInvariant(ContainingAsset->GetPathName()));
	AssetCheckLog.Warning()
	 	->AddToken(FUObjectToken::Create(ContainingAsset))
	 	->AddToken(FTextToken::Create(MessageLooping));
	
	if (GIsEditor)
	{
	 	AssetCheckLog.Notify(MessageLooping, EMessageSeverity::Warning, /*bForce=*/ true);
	}
	}
}

TArray<FString> UPlaySoundResource::GetSoundRowName()
{
	TArray<FString> SoundRowName = {"None"};
	UDataTable* DT = UResourcesConfig::GetInstance()->TypeMaping[EResourceType::Sound].LoadSynchronous();
	if (DT)
	{
		for (FName Row : DT->GetRowNames())
		{
			SoundRowName.Add(Row.ToString());
		}
	}
	return SoundRowName;
}

TArray<FString> UPlaySoundResource::GetResourceName()
{
	TArray<FString> SoundResourceName = {"None"};
	UDataTable* DT = UResourcesConfig::GetInstance()->TypeMaping[EResourceType::Sound].LoadSynchronous();
	if (DT)
	{
		FResourceProperty_Sound* ResourceProperty_Sound = DT->FindRow<FResourceProperty_Sound>(RowName,TEXT(""));
		if (ResourceProperty_Sound)
		{
			ResourceProperty_Sound->Sound.GenerateKeyArray(SoundResourceName);
		}
	}
	return SoundResourceName;
}
#endif

//USoundBase* UPlaySoundResource::GetSound()//ue4
TObjectPtr<USoundBase> UPlaySoundResource::GetSound()//ue5
{

	return GetSound(RowName, GetResourceNameOrIndex());

	//FName SoundRowName = RowName;
	//FString SoundResourceNameOrIndex = GetResourceNameOrIndex();

	//if (SoundAssetTags.Num() > 0)
	//{
	//	if (IsRandomPlayOneSound)
	//	{
	//		FResourceProperty_SoundAssetTag SoundAssetTag = SoundAssetTags[UKismetMathLibrary::RandomIntegerInRange(0, SoundAssetTags.Num() - 1)];
	//		SoundRowName = SoundAssetTag.RowName;
	//		SoundResourceNameOrIndex = SoundAssetTag.ResourceNameOrIndex;
	//	}
	//}

	//FResourceProperty_SoundInfo SoftSound;
	//if (UResourceBPFunctionLibrary::GetResourceFromString_Sound(SoundRowName, SoundResourceNameOrIndex,SoftSound))
	//{
	//	Sound = SoftSound.Sound.LoadSynchronous();
	//	return Sound;
	//}
	//else
	//{
	//	Sound = nullptr;
	//	return nullptr;
	//}
}

TObjectPtr<USoundBase> UPlaySoundResource::GetSound(FName SoundRowName, FString SoundResourceNameOrIndex)
{
	FResourceProperty_SoundInfo SoftSound;
	if (UResourceBPFunctionLibrary::GetResourceFromString_Sound(SoundRowName, SoundResourceNameOrIndex, SoftSound))
	{
		Sound = SoftSound.Sound.LoadSynchronous();
		return Sound;
	}
	else
	{
		Sound = nullptr;
		return nullptr;
	}
}

USoundSubsystem* UPlaySoundResource::GetSoundSubsystem()
{
	if (!SoundSubsystem)
	{
		SoundSubsystem = Cast<USoundSubsystem>(USubsystemBlueprintLibrary::GetWorldSubsystem(World, USoundSubsystem::StaticClass()));
	}
	return SoundSubsystem;
}

FString UPlaySoundResource::GetResourceNameOrIndex()
{
	return NameOrIndex.IsEmpty()? ResourceNameOrIndex: NameOrIndex;
}

void UPlaySoundResource::SetRowName(FName NewRowName)
{
	RowName = NewRowName;
}

void UPlaySoundResource::SetResourceNameOrIndex(FString NewNameOrIndex)
{
	ResourceNameOrIndex = NewNameOrIndex;
}

void UPlaySoundResource::SetParameter(UAudioComponent* AudioComponent, FName SoundRowName, FString SoundResourceNameOrIndex)
{
	if (AudioComponent)
	{
		if (UseAnimNotifyParameter)
		{
			for (TPair<FName, float> pair : FloatParameter)
			{
				AudioComponent->SetFloatParameter(pair.Key, pair.Value);
			}
			for (TPair<FName, USoundWave*> pair : WaveParameter)
			{
				AudioComponent->SetWaveParameter(pair.Key, pair.Value);
			}
			for (TPair<FName, bool> pair : BoolParameter)
			{
				AudioComponent->SetBoolParameter(pair.Key, pair.Value);
			}
			for (TPair<FName, int32> pair : IntParameter)
			{
				AudioComponent->SetIntParameter(pair.Key, pair.Value);
			}

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

TArray<FResourceProperty_SoundAssetTag> UPlaySoundResource::GetAllSoundAssetTags()
{
	TArray<FResourceProperty_SoundAssetTag> ReturnSoundAssetTags = SoundAssetTags;
	if (!RowName.IsNone())
	{
		ReturnSoundAssetTags.Add(FResourceProperty_SoundAssetTag(RowName, GetResourceNameOrIndex()));
	}
	return ReturnSoundAssetTags;
}

int32 UPlaySoundResource::GetCurPlaySoundResourceIndex()
{
#if WITH_EDITORONLY_DATA
	return CurPlaySoundResourceIndex_Editor;
#else
	return CurPlaySoundResourceIndex;
#endif
}

int32 UPlaySoundResource::ChangeCurPlaySoundResourceIndex(int32 ChangeValue)
{
#if WITH_EDITORONLY_DATA
	CurPlaySoundResourceIndex_Editor = ChangeValue;
#else
	CurPlaySoundResourceIndex = ChangeValue;
#endif
	return ChangeValue;
}

int32 UPlaySoundResource::AddCurPlaySoundResourceIndex(int32 AddValue)
{
#if WITH_EDITORONLY_DATA
	return CurPlaySoundResourceIndex_Editor += AddValue;
#else
	return CurPlaySoundResourceIndex += AddValue;
#endif
}
