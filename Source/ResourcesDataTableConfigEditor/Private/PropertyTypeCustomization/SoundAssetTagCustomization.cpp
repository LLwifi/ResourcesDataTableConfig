// Fill out your copyright notice in the Description page of Project Settings.


#include "PropertyTypeCustomization/SoundAssetTagCustomization.h"
#include <IDetailChildrenBuilder.h>
#include <Widgets/Input/SEditableTextBox.h>
#include <Misc/MessageDialog.h>
#include <Misc/PackageName.h>
#include <FileHelpers.h>
#include <DetailWidgetRow.h>
#include "SSearchableComboBox.h"
#include <PropertyCustomizationHelpers.h>
//#include <Editor/PropertyEditor/Private/UserInterface/PropertyEditor/SPropertyEditorCombo.h>

#define LOCTEXT_NAMESPACE "SoundAssetTagCustomization"

void ISoundAssetTagCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
    HeaderRow
        .NameContent()
        [
            PropertyHandle->CreatePropertyNameWidget()
        ];
}

void ISoundAssetTagCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
    //通过名称拿结构体变量
    //TSharedPtr<IPropertyHandle> SoundAssetTypeHandle = PropertyHandle->GetChildHandle("SoundAssetType");
    //ESoundAssetType CurSoundAssetType = ESoundAssetType::Sound;
    //FProperty* Property = SoundAssetTypeHandle->GetProperty();
    //FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property);

    RowNameHandle = PropertyHandle->GetChildHandle("RowName");
    ResourceNameOrIndexHandle = PropertyHandle->GetChildHandle("ResourceNameOrIndex");
    SoundTypeHandle = PropertyHandle->GetChildHandle("SoundAssetType");
    //FName CurRowName;
    //RowNameHandle->GetValue(CurRowName);
    //TSharedPtr<IPropertyHandle> ResourceNameOrIndexHandle = PropertyHandle->GetChildHandle("ResourceNameOrIndex");
    //FString CurResourceNameOrIndex;
    //ResourceNameOrIndexHandle->GetValue(CurResourceNameOrIndex);

    //check(SoundAssetTypeHandle.IsValid()/* && WidgetHandle.IsValid()*/);

    void* ValuePtr;
    PropertyHandle->GetValueData(ValuePtr);
    if (ValuePtr != nullptr)
    {
        SoundAssetTag = (FResourceProperty_SoundAssetTag*)ValuePtr;
    }

    //TSharedPtr<FPropertyEditor> cc;

    Refresh(RowNames, ResourceNameOrIndexs);


    UEnum* EnumPtr = StaticEnum<ESoundAssetType>();
    if (EnumPtr) 
    {
        for (int32 i = 0; i < EnumPtr->GetMaxEnumValue(); ++i)
        {
            FString DisplayName = EnumPtr->GetNameStringByValue(i);
            ESoundAssetType SoundAssetType = (ESoundAssetType)EnumPtr->GetValueByIndex(i);
            Name_Type.Add(DisplayName, SoundAssetType);
            SoundTypes.Add(MakeShareable(new FString(DisplayName)));
        }
    }
       

    //slate
    ChildBuilder.AddCustomRow(FText())
        [
            SNew(SVerticalBox)          
            + SVerticalBox::Slot()
            [
                SAssignNew(SearchableComboBox_SoundType, SSearchableComboBox)
                    .OptionsSource(&SoundTypes)//所有选项
                    .OnGenerateWidget(this, &ISoundAssetTagCustomization::OnGenerateWidget_SoundType)//每个下拉选项的样式通过函数构造
                    .OnSelectionChanged(this, &ISoundAssetTagCustomization::OnSelectionChanged_SoundType)//改变选择的回调
                    [
                        SAssignNew(ComboBox_SoundType_Text, STextBlock)
                            .Text(FText::FromString(EnumPtr->GetNameStringByValue((int32)SoundAssetTag->SoundAssetType)))
                    ]
            ]
            + SVerticalBox::Slot()
            [
                SAssignNew(SearchableComboBox_RowName, SSearchableComboBox)
                    .OptionsSource(&RowNames)//所有选项
                    .OnGenerateWidget(this, &ISoundAssetTagCustomization::OnGenerateWidget_RowName)//每个下拉选项的样式通过函数构造
                    .OnSelectionChanged(this, &ISoundAssetTagCustomization::OnSelectionChanged_RowName)//改变选择的回调
                    [
                        SAssignNew(ComboBox_Name_Text, STextBlock)
                            .Text(FText::FromString(SoundAssetTag->RowName.ToString()))
                    ]
            ]
            + SVerticalBox::Slot()
            [
                SAssignNew(SearchableComboBox_RNOI, SSearchableComboBox)
                    .OptionsSource(&ResourceNameOrIndexs)//所有选项
                    .OnGenerateWidget(this, &ISoundAssetTagCustomization::OnGenerateWidget_RNOI)//每个下拉选项的样式通过函数构造
                    .OnSelectionChanged(this, &ISoundAssetTagCustomization::OnSelectionChanged_RNOI)//改变选择的回调
                    [
                        SAssignNew(ComboBox_RNOI_Text, STextBlock)
                            .Text(FText::FromString(SoundAssetTag->ResourceNameOrIndex))
                    ]
            ]
        ];
}

void ISoundAssetTagCustomization::Refresh(TArray<TSharedPtr<FString>>& AllRowName, TArray<TSharedPtr<FString>>& AllResourceNameOrIndex)
{
    AllRowName.Empty();
    AllResourceNameOrIndex.Empty();

    UDataTable* DT = nullptr;
    TArray<FName> DTRowName;
    TArray<FString> SoundResourceName;
    switch (SoundAssetTag->SoundAssetType)
    {
    case ESoundAssetType::Sound:
    {
        DT = UResourcesConfig::GetInstance()->TypeMaping[EResourceType::Sound].LoadSynchronous();
        if (DT)
        {
            FResourceProperty_Sound* ResourceProperty_Sound = DT->FindRow<FResourceProperty_Sound>(SoundAssetTag->RowName, TEXT(""));
            if (ResourceProperty_Sound)
            {
                ResourceProperty_Sound->Sound.GenerateKeyArray(SoundResourceName);
            }
            DTRowName = DT->GetRowNames();
        }
        break;
    }
    case ESoundAssetType::BGM:
    {
        DT = UResourcesConfig::GetInstance()->TypeMaping[EResourceType::BGM].LoadSynchronous();
        if (DT)
        {
            FResourceProperty_BGM* ResourceProperty_BGM = DT->FindRow<FResourceProperty_BGM>(SoundAssetTag->RowName, TEXT(""));
            if (ResourceProperty_BGM)
            {
                ResourceProperty_BGM->BGM.GenerateKeyArray(SoundResourceName);
            }
            DTRowName = DT->GetRowNames();
        }
        break;
    }
    default:
        break;
    }

    for (FName& name : DTRowName)
    {
        AllRowName.Add(MakeShareable(new FString(name.ToString())));
    }
    for (FString& string : SoundResourceName)
    {
        AllResourceNameOrIndex.Add(MakeShareable(new FString(string)));
    }
}

void ISoundAssetTagCustomization::OnSelectionChanged_SoundType(TSharedPtr<FString> InItem, ESelectInfo::Type InSelectionInfo)
{
    //SoundTypeHandle->SetValue(&Name_Type[*InItem.Get()]);
    ComboBox_SoundType_Text->SetText(FText::FromString(*InItem));
    SoundAssetTag->SoundAssetType = Name_Type[*InItem.Get()];

    Refresh(RowNames, ResourceNameOrIndexs);
    SearchableComboBox_RowName->RefreshOptions();
    SearchableComboBox_RNOI->RefreshOptions();
}

TSharedRef<SWidget> ISoundAssetTagCustomization::OnGenerateWidget_SoundType(TSharedPtr<FString> InItem)
{
    return SNew(STextBlock)
        .Text(FText::FromString(*InItem));
}

void ISoundAssetTagCustomization::OnSelectionChanged_RowName(TSharedPtr<FString> InItem, ESelectInfo::Type InSelectionInfo)
{
    RowNameHandle->SetValue(FName(*InItem.Get()));
    ComboBox_Name_Text->SetText(FText::FromString(*InItem));
    SoundAssetTag->RowName = FName(*InItem.Get());

    Refresh(RowNames, ResourceNameOrIndexs);
    SearchableComboBox_RNOI->RefreshOptions();
}

TSharedRef<SWidget> ISoundAssetTagCustomization::OnGenerateWidget_RowName(TSharedPtr<FString> InItem)
{
    return SNew(STextBlock)
        .Text(FText::FromString(*InItem));
}

void ISoundAssetTagCustomization::OnSelectionChanged_RNOI(TSharedPtr<FString> InItem, ESelectInfo::Type InSelectionInfo)
{
    ResourceNameOrIndexHandle->SetValue(FName(*InItem.Get()));
    ComboBox_RNOI_Text->SetText(FText::FromString(*InItem));
    SoundAssetTag->ResourceNameOrIndex = *InItem.Get();
}

TSharedRef<SWidget> ISoundAssetTagCustomization::OnGenerateWidget_RNOI(TSharedPtr<FString> InItem)
{
    return SNew(STextBlock)
        .Text(FText::FromString(*InItem));
}

#undef LOCTEXT_NAMESPACE