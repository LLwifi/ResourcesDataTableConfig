// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourcePropertyView.h"
#include "FileHelpers.h"
#include <ResourcesDataTableConfig/Public/ResourceBPFunctionLibrary.h>
#include <ResourcesDataTableConfig/Public/ResourcesConfig.h>

FName UResourcePropertyView::GetRowName_Implementation()
{
	return FName();
}

void UResourcePropertyView::ClearAllMap()
{
	Bool.Empty();
	Int32.Empty();
	Float.Empty();
	Name.Empty();
	String.Empty();
	Text.Empty();
	Vector.Empty();
	Rotator.Empty();
	Texture2D.Empty();
	Transform.Empty();
	Class.Empty();
	Sound.Empty();
}

void UResourcePropertyView::ShowProperty(FResourceTypeGroup PropertyTypeGroup)
{
	for (FResourceTypeInfo Info : PropertyTypeGroup.PropertyGroup)
	{
		if (!GetRowName().IsNone() && GetTypeMaping().Contains(Info.Type) && GetTypeMaping()[Info.Type] && GetTypeMaping()[Info.Type]->GetRowNames().Contains(GetRowName()))//存储值
		{
			switch (Info.Type) {
			case EResourceType::Bool:
				{
					Bool = GetTypeMaping()[Info.Type]->FindRow<FResourceProperty_Bool>(GetRowName(),TEXT(""))->Bool;
					break;
				}
			case EResourceType::Int32:
				{
					Int32 = GetTypeMaping()[Info.Type]->FindRow<FResourceProperty_Int32>(GetRowName(),TEXT(""))->Int32;
					break;
				}
			case EResourceType::Float:
				{
					Float = GetTypeMaping()[Info.Type]->FindRow<FResourceProperty_Float>(GetRowName(),TEXT(""))->Float;
					break;
				}
			case EResourceType::Name:
				{
					Name = GetTypeMaping()[Info.Type]->FindRow<FResourceProperty_Name>(GetRowName(),TEXT(""))->Name;
					break;
				}
			case EResourceType::String:
				{
					String = GetTypeMaping()[Info.Type]->FindRow<FResourceProperty_String>(GetRowName(),TEXT(""))->String;
					break;
				}
			case EResourceType::Text:
				{
					Text = GetTypeMaping()[Info.Type]->FindRow<FResourceProperty_Text>(GetRowName(),TEXT(""))->Text;
					break;
				}
			case EResourceType::Vector:
				{
					Vector = GetTypeMaping()[Info.Type]->FindRow<FResourceProperty_Vector>(GetRowName(),TEXT(""))->Vector;
					break;
				}
			case EResourceType::Rotator:
				{
					Rotator = GetTypeMaping()[Info.Type]->FindRow<FResourceProperty_Rotator>(GetRowName(),TEXT(""))->Rotator;
					break;
				}
			case EResourceType::Texture2D:
				{
					Texture2D = GetTypeMaping()[Info.Type]->FindRow<FResourceProperty_Texture2D>(GetRowName(),TEXT(""))->Texture2D;
					break;
				}
			case EResourceType::Class:
				{
					Class = GetTypeMaping()[Info.Type]->FindRow<FResourceProperty_Class>(GetRowName(),TEXT(""))->Class;
					break;
				}
			case EResourceType::Transform:
				{
					Transform = GetTypeMaping()[Info.Type]->FindRow<FResourceProperty_Transform>(GetRowName(),TEXT(""))->Transform;
					break;
				}
			case EResourceType::Sound:
				{
					Sound = GetTypeMaping()[Info.Type]->FindRow<FResourceProperty_Sound>(GetRowName(),TEXT(""))->Sound;
					break;
				}
			}
		}
		else//初始值
		{
			switch (Info.Type) {
			case EResourceType::Bool:
			{
				Bool.Add(Info.Name, Info.InitValue.ToString().Equals("True",ESearchCase::IgnoreCase) ? true : false);
				break;
			}
			case EResourceType::Int32:
			{
				Int32.Add(Info.Name, FCString::Atoi(*Info.InitValue.ToString()));
				break;
			}
			case EResourceType::Float:
			{
				Float.Add(Info.Name, FCString::Atof(*Info.InitValue.ToString()));
				break;
			}
			case EResourceType::Name:
			{
				Name.Add(Info.Name, FName(*Info.InitValue.ToString()));
				break;
			}
			case EResourceType::String:
			{
				String.Add(Info.Name, Info.InitValue.ToString());
				break;
			}
			case EResourceType::Text:
				{
					Text.Add(Info.Name,Info.InitValue);
					break;
				}
			case EResourceType::Vector:
				{
					Vector.Add(Info.Name, UResourceBPFunctionLibrary::StringToVector(Info.InitValue.ToString()));
					break;
				}
			case EResourceType::Rotator:
				{
					Rotator.Add(Info.Name, UResourceBPFunctionLibrary::StringToRotator(Info.InitValue.ToString()));
					break;
				}
			case EResourceType::Texture2D:
				{
					Texture2D.Add(Info.Name,TSoftObjectPtr<UTexture2D>(FSoftObjectPath(Info.InitValue.ToString())));
					break;
				}
			case EResourceType::Class:
				{
					Class.Add(Info.Name, TSoftClassPtr<UObject>(FSoftClassPath(Info.InitValue.ToString())));
					break;
				}
			case EResourceType::Transform:
				{
					Transform.Add(Info.Name, UResourceBPFunctionLibrary::StringToTransform(Info.InitValue.ToString()));
					break;
				}
			case EResourceType::Sound:
				{
					Sound.Add(Info.Name,TSoftObjectPtr<USoundBase>(FSoftObjectPath(Info.InitValue.ToString())));
					break;
				}
			}
		}
	}
}

void UResourcePropertyView::SaveToDataTable(FResourceTypeGroup PropertyTypeGroup)
{
	if (!GetRowName().IsNone())
	{
		for (FResourceTypeInfo Info : PropertyTypeGroup.PropertyGroup)
		{
			if (GetTypeMaping().Contains(Info.Type))
			{
				switch (Info.Type) {
				case EResourceType::Bool:
					{
						GetTypeMaping()[Info.Type]->AddRow(GetRowName(),FResourceProperty_Bool(Bool));
						break;
					}
				case EResourceType::Int32:
					{
						GetTypeMaping()[Info.Type]->AddRow(GetRowName(),FResourceProperty_Int32(Int32));
						break;
					}
				case EResourceType::Float:
					{
						GetTypeMaping()[Info.Type]->AddRow(GetRowName(),FResourceProperty_Float(Float));
						break;
					}
				case EResourceType::Name:
					{
						GetTypeMaping()[Info.Type]->AddRow(GetRowName(),FResourceProperty_Name(Name));
						break;
					}
				case EResourceType::String:
					{
						GetTypeMaping()[Info.Type]->AddRow(GetRowName(),FResourceProperty_String(String));
						break;
					}
				case EResourceType::Text:
					{
						GetTypeMaping()[Info.Type]->AddRow(GetRowName(),FResourceProperty_Text(Text));
						break;
					}
				case EResourceType::Vector:
					{
						GetTypeMaping()[Info.Type]->AddRow(GetRowName(),FResourceProperty_Vector(Vector));
						break;
					}
				case EResourceType::Rotator:
					{
						GetTypeMaping()[Info.Type]->AddRow(GetRowName(),FResourceProperty_Rotator(Rotator));
						break;
					}
				case EResourceType::Texture2D:
					{
						GetTypeMaping()[Info.Type]->AddRow(GetRowName(),FResourceProperty_Texture2D(Texture2D));
						break;
					}
				case EResourceType::Class:
					{
						GetTypeMaping()[Info.Type]->AddRow(GetRowName(), FResourceProperty_Class(Class));
						break;
					}
				case EResourceType::Transform:
					{
						GetTypeMaping()[Info.Type]->AddRow(GetRowName(),FResourceProperty_Transform(Transform));
						break;
					}
				}
				UPackage* Package = FindPackage(nullptr, *FPackageName::FilenameToLongPackageName(GetTypeMaping()[Info.Type]->GetPathName()));
				if (Package)
				{
					Package->SetDirtyFlag(true);
					FEditorFileUtils::SaveDirtyPackages(false, false, true);
				}
			}
		}
	}
}

TMap<EResourceType, UDataTable*> UResourcePropertyView::GetTypeMaping()
{
	TMap<EResourceType, UDataTable*> TypeMaping;

	for (TPair<EResourceType, TSoftObjectPtr<UDataTable>> Pair : UResourcesConfig::GetInstance()->TypeMaping)
	{
		TypeMaping.Add(Pair.Key, Pair.Value.LoadSynchronous());
	}

	return TypeMaping;
}

UDataTable* UResourcePropertyView::GetResourceTypeGroup()
{
	return UResourcesConfig::GetInstance()->ResourceTypeGroup.LoadSynchronous();
}
