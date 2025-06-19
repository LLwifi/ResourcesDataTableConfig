// Fill out your copyright notice in the Description page of Project Settings.


#include "Sound/SoundEvent.h"

bool USoundCompare::CompareResult_Implementation(FSoundCompareInfo ThisCompareInfo, FSoundCompareParameter CompareParameter)
{
	return true;
}

FSoundCompareParameter ISoundEventInteract::GetSoundEventCompareParameter_Implementation(FName SoundEventName)
{
	return FSoundCompareParameter();
}
