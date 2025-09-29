// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceBPFunctionLibrary.h"
#include "Components/AudioComponent.h"

FTransform UResourceBPFunctionLibrary::StringToTransform(FString StringValue)
{
	FTransform Transform;
	/*UE字符格式格式
	(Rotation=(X=0.000000,Y=0.000000,Z=0.000000,W=1.000000),
	Translation=(X=0.000000,Y=0.000000,Z=0.000000),
	Scale3D=(X=1.000000,Y=1.000000,Z=1.000000))
	 */
	TArray<FString> ArrayString;
	StringValue.ParseIntoArray(ArrayString,TEXT("),"));
	FString L,R;
	if (ArrayString.Num() == 3)
	{
		for(int i = 0; i < ArrayString.Num(); i++)
		{
			ArrayString[i].Split("=(",&L,&R);
			ArrayString[i] = R;
			if (i == ArrayString.Num() - 1)
			{
				ArrayString[i].Split(")",&L,&R);
				ArrayString[i] = L;
			}
		}
		
		TArray<FString> SValue;
		TArray<float> FValue;
		//Rotation
		ArrayString[0].ParseIntoArray(SValue,TEXT(","));
		for(int i = 0; i < SValue.Num(); i++)
		{
			SValue[i].Split("=",&L,&R);
			FValue.Add(FCString::Atof(*R));
		}
		FQuat InRotation = FQuat(FValue[0],FValue[1],FValue[2],FValue[3]);
		SValue.Empty();
		FValue.Empty();

		//Translation
		ArrayString[1].ParseIntoArray(SValue,TEXT(","));
		for(int i = 0; i < SValue.Num(); i++)
		{
			SValue[i].Split("=",&L,&R);
			FValue.Add(FCString::Atof(*R));
		}
		FVector Translation = FVector(FValue[0],FValue[1],FValue[2]);
		SValue.Empty();
		FValue.Empty();

		//Scale3D
		ArrayString[2].ParseIntoArray(SValue,TEXT(","));
		for(int i = 0; i < SValue.Num(); i++)
		{
			SValue[i].Split("=",&L,&R);
			FValue.Add(FCString::Atof(*R));
		}
		FVector InScale3D = FVector(FValue[0],FValue[1],FValue[2]);
		Transform = FTransform(InRotation,Translation,InScale3D);
	}

	return Transform;
}

FVector UResourceBPFunctionLibrary::StringToVector(FString StringValue)
{
	FVector Vector;
	/*UE字符格式格式
	(X=1020.000000,Y=1350.000000,Z=96.000000)
	*/
	StringValue = StringValue.Left(1);
	StringValue = StringValue.Right(1);

	TArray<FString> ArrayString;
	StringValue.ParseIntoArray(ArrayString, TEXT(","));
	FString L, R;
	if (ArrayString.Num() == 3)
	{
		TArray<float> FValue;
		for (int i = 0; i < ArrayString.Num(); i++)
		{
			ArrayString[i].Split("=", &L, &R);
			FValue.Add(FCString::Atof(*R));
		}
		Vector.X = FValue[0];
		Vector.Y = FValue[1];
		Vector.Z = FValue[2];
	}

	return Vector;
}

FRotator UResourceBPFunctionLibrary::StringToRotator(FString StringValue)
{
	FRotator Rotator;
	/*UE字符格式格式
	(Pitch=0.000000,Yaw=0.000000,Roll=0.000000)
	*/
	StringValue = StringValue.Left(1);
	StringValue = StringValue.Right(1);

	TArray<FString> ArrayString;
	StringValue.ParseIntoArray(ArrayString, TEXT(","));
	FString L, R;
	if (ArrayString.Num() == 3)
	{
		TArray<float> FValue;
		for (int i = 0; i < ArrayString.Num(); i++)
		{
			ArrayString[i].Split("=", &L, &R);
			FValue.Add(FCString::Atof(*R));
		}
		Rotator.Pitch = FValue[0];
		Rotator.Yaw = FValue[1];
		Rotator.Roll = FValue[2];
	}

	return Rotator;
}

void UResourceBPFunctionLibrary::SetParameterFromAudioComponent(UAudioComponent* AudioComponent, FSoundParameters AudioParameter)
{
	//MetaSound & SoundCue
	if (AudioComponent)
	{
		TArray<FMyAudioParameter> AudioParameters;
		AudioParameter.MyAudioParameter.GenerateValueArray(AudioParameters);

		for (TPair< FName, FMyAudioParameter >& pair : AudioParameter.MyAudioParameter)
		{
			if (pair.Value.AudioParameter.ParamName.IsNone())
			{
				pair.Value.AudioParameter.ParamName = pair.Key;
			}

			//给两个Object类型进行赋值
			if (pair.Value.AudioParameter.ParamType == EAudioParameterType::Object && !pair.Value.AudioParameter.ObjectParam)
			{
				pair.Value.AudioParameter.ObjectParam = pair.Value.MyObjectParam.LoadSynchronous();
			}
			else if (pair.Value.AudioParameter.ParamType == EAudioParameterType::ObjectArray && pair.Value.AudioParameter.ArrayObjectParam.Num() == 0)
			{
				for (int32 i = 0; i < pair.Value.MyArrayObjectParam.Num(); i++)
				{
					pair.Value.AudioParameter.ArrayObjectParam.Add(pair.Value.MyArrayObjectParam[i].LoadSynchronous());
				}
			}
			AudioComponent->SetParameter(MoveTemp(pair.Value.AudioParameter));
			//数组版本---------
			//AudioComponent->SetParameters(MoveTemp(AudioParameters)); //MoveTemp
			//AudioComponent->SetParameters_Blueprint(AudioParameters);
		}


		for (TPair< FName, FMyAudioParameterMap > pair : AudioParameter.CustomAudioParameter)
		{
			SetParameterFromAudioComponent_MyAudioParameterMap(AudioComponent, pair.Value);
		}
	}
}

void UResourceBPFunctionLibrary::SetParameterFromAudioComponent_MyAudioParameterMap(UAudioComponent* AudioComponent, FMyAudioParameterMap MyAudioParameterMap)
{
	if (AudioComponent)
	{
		TArray<FMyAudioParameter> AudioParameters;
		MyAudioParameterMap.Name_Parameter.GenerateValueArray(AudioParameters);

		for (TPair< FName, FMyAudioParameter > pair : MyAudioParameterMap.Name_Parameter)
		{
			if (pair.Value.AudioParameter.ParamName.IsNone())
			{
				pair.Value.AudioParameter.ParamName = pair.Key;
			}

			if (pair.Value.AudioParameter.ParamType == EAudioParameterType::Object && !pair.Value.AudioParameter.ObjectParam)
			{
				pair.Value.AudioParameter.ObjectParam = pair.Value.MyObjectParam.LoadSynchronous();
			}
			else if (pair.Value.AudioParameter.ParamType == EAudioParameterType::ObjectArray && pair.Value.AudioParameter.ArrayObjectParam.Num() == 0)
			{
				for (int32 i = 0; i < pair.Value.MyArrayObjectParam.Num(); i++)
				{
					pair.Value.AudioParameter.ArrayObjectParam.Add(pair.Value.MyArrayObjectParam[i].LoadSynchronous());
				}
			}
			AudioComponent->SetParameter(MoveTemp(pair.Value.AudioParameter));
			//数组版本---------
			//AudioComponent->SetParameters(MoveTemp(AudioParameters)); //MoveTemp
			//AudioComponent->SetParameters_Blueprint(AudioParameters);
		}
	}
}

bool UResourceBPFunctionLibrary::GetResourceProperty_BGM(FName RowName, FResourceProperty_BGM& Value)
{
	if (UResourcesConfig::GetInstance()->TypeMaping.Contains(EResourceType::BGM))
	{
		UDataTable* DT = UResourcesConfig::GetInstance()->TypeMaping[EResourceType::BGM].LoadSynchronous();
		if (DT && DT->GetRowNames().Contains(RowName))
		{
			Value = *DT->FindRow<FResourceProperty_BGM>(RowName, TEXT(""));
			return true;
		}
	}
	return false;
}

bool UResourceBPFunctionLibrary::GetResourceArray_BGM(FName RowName, TArray<FBGMInfo>& Value)
{
	FResourceProperty_BGM ResourceProperty_BGM;
	if (GetResourceProperty_BGM(RowName, ResourceProperty_BGM))
	{
		ResourceProperty_BGM.BGM.GenerateValueArray(Value);
		return true;
	}
	return false;
}

bool UResourceBPFunctionLibrary::GetResourceFromString_BGM(FName RowName, FString ResourceNameOrIndex, FBGMInfo& Value)
{
	FResourceProperty_BGM ResourceProperty_BGM;
	if (GetResourceProperty_BGM(RowName, ResourceProperty_BGM))
	{
		if (ResourceProperty_BGM.Contains(ResourceNameOrIndex))
		{
			Value = ResourceProperty_BGM[ResourceNameOrIndex];
			return true;
		}
		else if (ResourceNameOrIndex.IsNumeric())
		{
			Value = ResourceProperty_BGM[FCString::Atoi(*ResourceNameOrIndex)];
			return true;
		}
	}
	return false;
}

bool UResourceBPFunctionLibrary::GetResourceProperty_Texture2D(FName RowName, FResourceProperty_Texture2D& Value)
{
	if (UResourcesConfig::GetInstance()->TypeMaping.Contains(EResourceType::Texture2D))
	{
		UDataTable* DT = UResourcesConfig::GetInstance()->TypeMaping[EResourceType::Texture2D].LoadSynchronous();
		if (DT && DT->GetRowNames().Contains(RowName))
		{
			Value = DT->FindRow<FResourceProperty_Texture2D>(RowName, TEXT(""))->Texture2D;
			return true;
		}
	}
	return false;
}

bool UResourceBPFunctionLibrary::GetResourceArray_Texture2D(FName RowName, TArray<TSoftObjectPtr<UTexture2D>>& Value)
{
	FResourceProperty_Texture2D ResourceProperty_Texture2D;
	if (GetResourceProperty_Texture2D(RowName, ResourceProperty_Texture2D))
	{
		ResourceProperty_Texture2D.Texture2D.GenerateValueArray(Value);
		return true;
	}
	return false;
}

bool UResourceBPFunctionLibrary::GetResourceFromString_Texture2D(FName RowName, FString ResourceNameOrIndex, TSoftObjectPtr<UTexture2D>& Value)
{
	FResourceProperty_Texture2D ResourceProperty_Texture2D;
	if (GetResourceProperty_Texture2D(RowName, ResourceProperty_Texture2D))
	{
		if (ResourceProperty_Texture2D.Contains(ResourceNameOrIndex))
		{
			Value = ResourceProperty_Texture2D[ResourceNameOrIndex];
			return true;
		}
		else if (ResourceNameOrIndex.IsNumeric())
		{
			Value = ResourceProperty_Texture2D[FCString::Atoi(*ResourceNameOrIndex)];
			return true;
		}

	}
	return false;
}

bool UResourceBPFunctionLibrary::GetResourceProperty_Sound(FName RowName, FResourceProperty_Sound& Value)
{
	if (UResourcesConfig::GetInstance()->TypeMaping.Contains(EResourceType::Sound)) 
	{
		UDataTable* DT = UResourcesConfig::GetInstance()->TypeMaping[EResourceType::Sound].LoadSynchronous();
		if (DT && DT->GetRowNames().Contains(RowName))
		{
			Value = DT->FindRow<FResourceProperty_Sound>(RowName, TEXT(""))->Sound;
			return true;
		}
	}
	return false;
}

bool UResourceBPFunctionLibrary::GetResourceArray_Sound(FName RowName, TArray<FResourceProperty_SoundInfo>& Value)
{
	FResourceProperty_Sound ResourceProperty_Sound;
	if (GetResourceProperty_Sound(RowName, ResourceProperty_Sound))
	{
		ResourceProperty_Sound.Sound.GenerateValueArray(Value);
		return true;
	}
	return false;
}

bool UResourceBPFunctionLibrary::GetResourceFromString_Sound(FName RowName, FString ResourceNameOrIndex, FResourceProperty_SoundInfo& Value)
{
	FResourceProperty_Sound ResourceProperty_Sound;
	if (GetResourceProperty_Sound(RowName, ResourceProperty_Sound))
	{
		if(ResourceProperty_Sound.Contains(ResourceNameOrIndex))
		{
			Value = ResourceProperty_Sound[ResourceNameOrIndex];
			return true;
		}
		else if (ResourceNameOrIndex.IsNumeric())
		{
			Value = ResourceProperty_Sound[FCString::Atoi(*ResourceNameOrIndex)];
			return true;
		}
	}
	return false;
}

USoundWave* UResourceBPFunctionLibrary::GetSoundWaveFromParameters(FSoundParameters SoundParameters)
{
	return SoundParameters.GetSoundWaveFromParameter();
}

