// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class IAssetTypeActions;

class FResourcesDataTableConfigModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};



/*
* BGM和sound字幕单独出去结构体，并考虑富文本问题
* 
*/