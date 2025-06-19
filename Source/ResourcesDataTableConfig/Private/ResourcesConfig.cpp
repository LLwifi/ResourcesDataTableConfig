// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourcesConfig.h"

UResourcesConfig* UResourcesConfig::GetInstance()
{
	return GetMutableDefault<UResourcesConfig>();
}
