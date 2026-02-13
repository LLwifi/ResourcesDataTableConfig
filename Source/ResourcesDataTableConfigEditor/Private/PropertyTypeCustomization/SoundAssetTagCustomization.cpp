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
#include <Engine/CompositeDataTable.h>
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
    DataTableHandle = PropertyHandle->GetChildHandle("DataTable");

    //对应的数据表
    TSharedPtr<IPropertyHandle> SoundBaseHandle, ReferenceObjectHandle;
    SoundBaseHandle = PropertyHandle->GetChildHandle("SoundBase");
    ReferenceObjectHandle = PropertyHandle->GetChildHandle("ReferenceObject");
    //FName CurRowName;
    //RowNameHandle->GetValue(CurRowName);
    //TSharedPtr<IPropertyHandle> ResourceNameOrIndexHandle = PropertyHandle->GetChildHandle("ResourceNameOrIndex");
    //FString CurResourceNameOrIndex;
    //ResourceNameOrIndexHandle->GetValue(CurResourceNameOrIndex);

    //check(SoundAssetTypeHandle.IsValid()/* && WidgetHandle.IsValid()*/);

    void* ValuePtr;
    PropertyHandle->GetValueData(ValuePtr);
    if (ValuePtr != nullptr)//在动画通知上同时选择两个时会为空
    {
        SoundAssetTag = (FResourceProperty_SoundAssetTag*)ValuePtr;

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
                    + SVerticalBox::Slot().AutoHeight()
                    [
                        SNew(SHorizontalBox)
                            +SHorizontalBox::Slot().AutoWidth()
                            [
                                DataTableHandle->CreatePropertyValueWidget()
                            ]
                            + SHorizontalBox::Slot().AutoWidth()
                            [
                                SoundBaseHandle->CreatePropertyValueWidget()
                            ]
                            + SHorizontalBox::Slot().AutoWidth()
                            [
                                ReferenceObjectHandle->CreatePropertyValueWidget()
                            ]
                    ]
            ];
    }
}

void ISoundAssetTagCustomization::Refresh(TArray<TSharedPtr<FString>>& AllRowName, TArray<TSharedPtr<FString>>& AllResourceNameOrIndex)
{
    AllRowName.Empty();
    AllResourceNameOrIndex.Empty();

    //ParentTables
    UDataTable* DataTable = nullptr;
    TArray<TSoftObjectPtr<UDataTable>> AllDT;
    TArray<FName> DTRowName;
    TArray<FString> SoundResourceName;
    switch (SoundAssetTag->SoundAssetType)
    {
    case ESoundAssetType::Sound:
    {
        DataTable = UResourcesConfig::GetInstance()->TypeMaping[EResourceType::Sound].LoadSynchronous();
        if (DataTable)
        {
            FResourceProperty_Sound* ResourceProperty_Sound = DataTable->FindRow<FResourceProperty_Sound>(SoundAssetTag->RowName, TEXT(""));
            if (ResourceProperty_Sound)
            {
                ResourceProperty_Sound->Sound.GenerateKeyArray(SoundResourceName);
                if(ResourceProperty_Sound->Sound.Contains(SoundAssetTag->ResourceNameOrIndex))
                {
                    SoundAssetTag->SoundBase = ResourceProperty_Sound->Sound[SoundAssetTag->ResourceNameOrIndex].Sound;
                    SoundAssetTag->ReferenceObject = ResourceProperty_Sound->Sound[SoundAssetTag->ResourceNameOrIndex].ReferenceObject;
                }
                //如果没找到配置的引用资源，这里使用上层通用配置的首个资源
                if (!SoundAssetTag->ReferenceObject && ResourceProperty_Sound->ReferencesObjects.IsValidIndex(0))
                {
                    SoundAssetTag->ReferenceObject = ResourceProperty_Sound->ReferencesObjects[0];
                }
            }
            DTRowName = DataTable->GetRowNames();
        }

        //尝试寻找准确的数据表
        AllDT = UResourcesConfig::GetInstance()->AllSoundDataTable;
        for (TSoftObjectPtr<UDataTable> DT : AllDT)
        {
            if (DT.LoadSynchronous())
            {
                FResourceProperty_Sound* ResourceProperty_Sound = DT.LoadSynchronous()->FindRow<FResourceProperty_Sound>(SoundAssetTag->RowName, TEXT(""));
                if (ResourceProperty_Sound)
                {
                    SoundAssetTag->DataTable = DT;
                    break;
                }
            }
        }
        break;
    }
    case ESoundAssetType::BGM:
    {
        DataTable = UResourcesConfig::GetInstance()->TypeMaping[EResourceType::BGM].LoadSynchronous();
        if (DataTable)
        {
            FResourceProperty_BGM* ResourceProperty_BGM = DataTable->FindRow<FResourceProperty_BGM>(SoundAssetTag->RowName, TEXT(""));
            if (ResourceProperty_BGM)
            {
                ResourceProperty_BGM->BGM.GenerateKeyArray(SoundResourceName);

                if(ResourceProperty_BGM->BGM.Contains(SoundAssetTag->ResourceNameOrIndex))
                {
                    SoundAssetTag->SoundBase = ResourceProperty_BGM->BGM[SoundAssetTag->ResourceNameOrIndex].SoundBase;
                    SoundAssetTag->ReferenceObject = ResourceProperty_BGM->BGM[SoundAssetTag->ResourceNameOrIndex].ReferenceObject;
                }

                //如果没找到配置的引用资源，这里使用上层通用配置的首个资源
                if (!SoundAssetTag->ReferenceObject && ResourceProperty_BGM->ReferencesObjects.IsValidIndex(0))
                {
                    SoundAssetTag->ReferenceObject = ResourceProperty_BGM->ReferencesObjects[0];
                }
            }
            DTRowName = DataTable->GetRowNames();
        }

        //尝试寻找准确的数据表
        AllDT = UResourcesConfig::GetInstance()->AllSoundDataTable;
        for (TSoftObjectPtr<UDataTable> DT : AllDT)
        {
            if (DT.LoadSynchronous())
            {
                FResourceProperty_BGM* ResourceProperty_BGM = DT.LoadSynchronous()->FindRow<FResourceProperty_BGM>(SoundAssetTag->RowName, TEXT(""));
                if (ResourceProperty_BGM)
                {
                    SoundAssetTag->DataTable = DT;
                    break;
                }
            }
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
    
    //如果没找到精确的数据表使用组合数据表
    if (!SoundAssetTag->DataTable)
    {
        SoundAssetTag->DataTable = DataTable;
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

    Refresh(RowNames, ResourceNameOrIndexs);
}

TSharedRef<SWidget> ISoundAssetTagCustomization::OnGenerateWidget_RNOI(TSharedPtr<FString> InItem)
{
    return SNew(STextBlock)
        .Text(FText::FromString(*InItem));
}

//----------------------------------------------------------------------------------------------

void IRDTC_AudioVolumeInfoHandleCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
    HeaderRow
        .NameContent()
        [
            PropertyHandle->CreatePropertyNameWidget()
        ];
}

void IRDTC_AudioVolumeInfoHandleCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
    //通过名称拿结构体变量
    RowNameHandle = PropertyHandle->GetChildHandle("RowName");

    void* ValuePtr;
    PropertyHandle->GetValueData(ValuePtr);
    if (ValuePtr != nullptr)
    {
        InfoHandle = (FRDTC_AudioVolumeInfoHandle*)ValuePtr;
        InfoHandle->DataTable = UResourcesConfig::GetInstance()->AudioVolumeInfoDataTable;
        DataTableHandle = PropertyHandle->GetChildHandle("DataTable");
    }

    //Refresh(RowNames, ResourceNameOrIndexs);

    UDataTable* DT = UResourcesConfig::GetInstance()->AudioVolumeInfoDataTable.LoadSynchronous();
    TArray<FName> DTRowName;
    if (DT)
    {
        DTRowName = DT->GetRowNames();
        for (FName& name : DTRowName)
        {
            RowNames.Add(MakeShareable(new FString(name.ToString())));
        }
    }

    //slate
    ChildBuilder.AddCustomRow(FText())
        [
            SNew(SVerticalBox)
                + SVerticalBox::Slot()
                [
                    SAssignNew(SearchableComboBox_RowName, SSearchableComboBox)
                        .OptionsSource(&RowNames)//所有选项
                        .OnGenerateWidget(this, &IRDTC_AudioVolumeInfoHandleCustomization::OnGenerateWidget_RowName)//每个下拉选项的样式通过函数构造
                        .OnSelectionChanged(this, &IRDTC_AudioVolumeInfoHandleCustomization::OnSelectionChanged_RowName)//改变选择的回调
                        [
                            SAssignNew(ComboBox_Name_Text, STextBlock)
                                .Text(FText::FromString(InfoHandle->RowName.ToString()))
                        ]
                ]
            + SVerticalBox::Slot().AutoHeight()
                [
                    SNew(SHorizontalBox)
                        + SHorizontalBox::Slot().AutoWidth()
                        [
                            DataTableHandle->CreatePropertyValueWidget()
                        ]
                ]
        ];
}

void IRDTC_AudioVolumeInfoHandleCustomization::OnSelectionChanged_RowName(TSharedPtr<FString> InItem, ESelectInfo::Type InSelectionInfo)
{
    RowNameHandle->SetValue(FName(*InItem.Get()));
    ComboBox_Name_Text->SetText(FText::FromString(*InItem));
    InfoHandle->RowName = FName(*InItem.Get());

    //Refresh(RowNames, ResourceNameOrIndexs);
}

TSharedRef<SWidget> IRDTC_AudioVolumeInfoHandleCustomization::OnGenerateWidget_RowName(TSharedPtr<FString> InItem)
{
    return SNew(STextBlock)
        .Text(FText::FromString(*InItem));
}

#undef LOCTEXT_NAMESPACE