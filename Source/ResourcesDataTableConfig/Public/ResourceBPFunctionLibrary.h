// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ResourcesStructAndEnum.h"
#include "ResourceBPFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class RESOURCESDATATABLECONFIG_API UResourceBPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure)
		static FTransform StringToTransform(FString StringValue);
	UFUNCTION(BlueprintPure)
		static FVector StringToVector(FString StringValue);
	UFUNCTION(BlueprintPure)
		static FRotator StringToRotator(FString StringValue);

	UFUNCTION(BlueprintCallable)
		static void SetParameterFromAudioComponent(UAudioComponent* AudioComponent, FSoundParameters AudioParameter);

	UFUNCTION(BlueprintCallable)
		static void SetParameterFromAudioComponent_MyAudioParameterMap(UAudioComponent* AudioComponent, FMyAudioParameterMap MyAudioParameterMap);

	UFUNCTION(BlueprintPure)
	static bool GetResourceProperty_BGM(FName RowName, FResourceProperty_BGM& Value);//Map
	UFUNCTION(BlueprintPure)
	static bool GetResourceArray_BGM(FName RowName, TArray<FBGMInfo>& Value);
	UFUNCTION(BlueprintPure)
	static bool GetResourceFromString_BGM(FName RowName, FString ResourceNameOrIndex, FBGMInfo& Value);

	UFUNCTION(BlueprintPure)
		static bool GetResourceProperty_Texture2D(FName RowName, FResourceProperty_Texture2D& Value);//Map
	UFUNCTION(BlueprintPure)
		static bool GetResourceArray_Texture2D(FName RowName, TArray<TSoftObjectPtr<UTexture2D>>& Value);
	UFUNCTION(BlueprintPure)
		static bool GetResourceFromString_Texture2D(FName RowName, FString ResourceNameOrIndex, TSoftObjectPtr<UTexture2D>& Value);

	UFUNCTION(BlueprintPure)
	static bool GetResourceProperty_Sound(FName RowName, FResourceProperty_Sound& Value);//Map
	UFUNCTION(BlueprintPure)
	static bool GetResourceArray_Sound(FName RowName, TArray<FResourceProperty_SoundInfo>& Value);
	UFUNCTION(BlueprintPure)
	static bool GetResourceFromString_Sound(FName RowName, FString ResourceNameOrIndex, FResourceProperty_SoundInfo& Value);

	//根据参数获取一个SoundWave
	UFUNCTION(BlueprintPure)
	static USoundWave* GetSoundWaveFromParameters(FSoundParameters SoundParameters);
};
