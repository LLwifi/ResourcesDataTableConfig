// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <ResourcesStructAndEnum.h>
#include "IPropertyTypeCustomization.h"


//class SWidget;

/**
 * 
 */
class RESOURCESDATATABLECONFIGEDITOR_API ISoundAssetTagCustomization : public IPropertyTypeCustomization
{
public:
    static TSharedRef<IPropertyTypeCustomization> MakeInstance()
    {
        return MakeShareable(new ISoundAssetTagCustomization());
    }

    virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

    virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

    //ˢ��
    void Refresh(TArray<TSharedPtr<FString>>& AllRowName, TArray<TSharedPtr<FString>>& AllResourceNameOrIndex);

    void OnSelectionChanged_SoundType(TSharedPtr<FString> InItem, ESelectInfo::Type InSelectionInfo);
    TSharedRef<SWidget> OnGenerateWidget_SoundType(TSharedPtr<FString> InItem);

    //�ı�ѡ��Ļص�
    void OnSelectionChanged_RowName(TSharedPtr<FString> InItem, ESelectInfo::Type InSelectionInfo);
    //ÿ������ѡ������ʽ
    TSharedRef<SWidget> OnGenerateWidget_RowName(TSharedPtr<FString> InItem);

    //�ı�ѡ��Ļص�
    void OnSelectionChanged_RNOI(TSharedPtr<FString> InItem, ESelectInfo::Type InSelectionInfo);
    //ÿ������ѡ������ʽ
    TSharedRef<SWidget> OnGenerateWidget_RNOI(TSharedPtr<FString> InItem);

public:
    //��Ч�ʲ�����
    TMap<FString, ESoundAssetType> Name_Type;
    TSharedPtr<IPropertyHandle> SoundTypeHandle;
    TArray<TSharedPtr<FString>> SoundTypes;
    TSharedPtr<class SSearchableComboBox> SearchableComboBox_SoundType;
    TSharedPtr<class STextBlock> ComboBox_SoundType_Text;

    //������
    TSharedPtr<IPropertyHandle> RowNameHandle;
    TArray<TSharedPtr<FString>> RowNames;
    TSharedPtr<class SSearchableComboBox> SearchableComboBox_RowName;
    TSharedPtr<class STextBlock> ComboBox_Name_Text;

    //��Դ����
    TSharedPtr<IPropertyHandle> ResourceNameOrIndexHandle;
    TArray<TSharedPtr<FString>> ResourceNameOrIndexs;
    TSharedPtr<class SSearchableComboBox> SearchableComboBox_RNOI;
    TSharedPtr<class STextBlock> ComboBox_RNOI_Text;



    //��������Ľṹ������
    FResourceProperty_SoundAssetTag* SoundAssetTag;
};
