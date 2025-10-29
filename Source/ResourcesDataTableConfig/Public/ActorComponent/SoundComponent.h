// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Runtime/GameplayTags/Classes/GameplayTagContainer.h>
#include "Components/AudioComponent.h"
#include <Sound/SoundEvent.h>
#include "SoundComponent.generated.h"

UINTERFACE(MinimalAPI)
class USoundPlayer : public UInterface
{
	GENERATED_BODY()
};

/**
 * 声音播放者相关接口
 */
class RESOURCESDATATABLECONFIG_API ISoundPlayer
{
	GENERATED_BODY()
public:
	//获取播放者的名称
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FText GetSoundPlayerName();
	virtual FText GetSoundPlayerName_Implementation(){ return FText(); };

};

/*
* 同类型UAudioComponent
* 可以将该结构体当作Array容器使用
*/
USTRUCT(BlueprintType)
struct FSameAudioComponent
{
	GENERATED_BODY()

public:
	FSameAudioComponent() {};
	FSameAudioComponent(UAudioComponent* AudioComponent) { AddAudioComponent(AudioComponent); };
	UAudioComponent* operator[](int32 Index) { return IsValidIndex(Index) ? AllAudioComponent[Index] : nullptr; };
	int32 AudioComponentNum() { return AllAudioComponent.Num(); };
	void AddAudioComponent(UAudioComponent* AudioComponent) { AllAudioComponent.Add(AudioComponent); };
	void RemoveAudioComponent(int32 Index){	AllAudioComponent.RemoveAt(Index);};
	void RemoveAudioComponent(UAudioComponent* AudioComponent) { AllAudioComponent.Remove(AudioComponent); };
	bool Contains(UAudioComponent* AudioComponent) { return AllAudioComponent.Contains(AudioComponent); };
	bool IsValidIndex(int32 Index) { return AllAudioComponent.IsValidIndex(Index); };
public:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		TArray<UAudioComponent*> AllAudioComponent;
};

/*声音组件
* 在Actor上添加该组件后，使用USoundSubsystem播放声音时会储存同一个Player（播放者）播放的声音
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESOURCESDATATABLECONFIG_API USoundComponent : public UActorComponent, public ISoundPlayer
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USoundComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
		void AddPlayAudioComponent(FGameplayTag SoundTag, UAudioComponent* AudioComponent);

	UFUNCTION(BlueprintCallable)
		void StopAudioComponentFromTag(FGameplayTag SoundTag, bool IsExactMatch = true, float FadeOutDuration = 0.0f, float FadeVolumeLevel = 0.0f, const EAudioFaderCurve FadeCurve = EAudioFaderCurve::Linear);

	//获取播放者的名称
	virtual FText GetSoundPlayerName_Implementation() override;
public:
	//播放者的名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText PlayerName;

	//播放过的声音
	UPROPERTY(BlueprintReadOnly)
		TMap<FGameplayTag, FSameAudioComponent> AllPlayAudioComponent;
};
