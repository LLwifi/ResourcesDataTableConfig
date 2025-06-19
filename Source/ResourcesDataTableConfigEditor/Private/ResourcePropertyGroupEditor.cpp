// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourcePropertyGroupEditor.h"
#include "FileHelpers.h"
#include <ResourcesDataTableConfig/Public/ResourcesConfig.h>

FName UResourcePropertyGroupEditor::GetRowName_Implementation()
{
	return FName();
}

void UResourcePropertyGroupEditor::SaveToDataTable(FResourceTypeGroup PropertyTypeGroup)
{
	if (!GetRowName().IsNone() && GetResourceTypeGroup())
	{
		GetResourceTypeGroup()->AddRow(GetRowName(),PropertyTypeGroup);
		UPackage* Package = FindPackage(nullptr, *FPackageName::FilenameToLongPackageName(GetResourceTypeGroup()->GetPathName()));
		if (Package)
		{
			Package->SetDirtyFlag(true);
			FEditorFileUtils::SaveDirtyPackages(false, false, true);
		}
	}
}

UDataTable* UResourcePropertyGroupEditor::GetResourceTypeGroup()
{
	return UResourcesConfig::GetInstance()->ResourceTypeGroup.LoadSynchronous();
}
