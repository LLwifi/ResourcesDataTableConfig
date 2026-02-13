// Copyright Epic Games, Inc. All Rights Reserved.

#include "ResourcesDataTableConfigEditor.h"
#include "PropertyTypeCustomization/SoundAssetTagCustomization.h"


void FResourcesDataTableConfigEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	// 加载 PropertyEditor 模块
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	//将结构体FUI_PrefabOperation 和 IUI_PrefabPropertyTypeCustom 关联起来
	PropertyEditorModule.RegisterCustomPropertyTypeLayout(FName("ResourceProperty_SoundAssetTag"), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&ISoundAssetTagCustomization::MakeInstance));
	PropertyEditorModule.RegisterCustomPropertyTypeLayout(FName("RDTC_AudioVolumeInfoHandle"), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&IRDTC_AudioVolumeInfoHandleCustomization::MakeInstance));
	PropertyEditorModule.NotifyCustomizationModuleChanged();
}

void FResourcesDataTableConfigEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout("ResourceProperty_SoundAssetTag");
		PropertyModule.UnregisterCustomPropertyTypeLayout("RDTC_AudioVolumeInfoHandle");
		PropertyModule.NotifyCustomizationModuleChanged();
	}
}

IMPLEMENT_MODULE(FResourcesDataTableConfigEditorModule, ResourcesDataTableConfigEditor)