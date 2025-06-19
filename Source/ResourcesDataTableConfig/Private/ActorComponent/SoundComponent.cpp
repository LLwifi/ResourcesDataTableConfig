// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/SoundComponent.h"

// Sets default values for this component's properties
USoundComponent::USoundComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USoundComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USoundComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USoundComponent::AddPlayAudioComponent(FGameplayTag SoundTag, UAudioComponent* AudioComponent)
{
	if (SoundTag.IsValid() && AudioComponent)
	{
		if (AllPlayAudioComponent.Contains(SoundTag))
		{
			AllPlayAudioComponent[SoundTag].AddAudioComponent(AudioComponent);
		}
		else
		{
			AllPlayAudioComponent.Add(SoundTag, AudioComponent);
		}
	}
}

void USoundComponent::StopAudioComponentFromTag(FGameplayTag SoundTag, bool IsExactMatch, float FadeOutDuration, float FadeVolumeLevel, const EAudioFaderCurve FadeCurve)
{
	if (IsExactMatch)//精准比对
	{
		if (AllPlayAudioComponent.Contains(SoundTag))
		{
			for (UAudioComponent* AudioComponent : AllPlayAudioComponent[SoundTag].AllAudioComponent)
			{
				if (AudioComponent)
				{
					AudioComponent->FadeOut(FadeOutDuration, FadeVolumeLevel, FadeCurve);
				}
			}
		}
	}
	else
	{
		for (TPair<FGameplayTag, FSameAudioComponent> pair : AllPlayAudioComponent)
		{
			if (pair.Key.MatchesTag(SoundTag))
			{
				for (UAudioComponent* AudioComponent : AllPlayAudioComponent[pair.Key].AllAudioComponent)
				{
					if (AudioComponent)
					{
						AudioComponent->FadeOut(FadeOutDuration, FadeVolumeLevel, FadeCurve);
					}
				}
			}
		}
	}
}

