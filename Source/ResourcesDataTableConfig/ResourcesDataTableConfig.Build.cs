// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ResourcesDataTableConfig : ModuleRules
{
	public ResourcesDataTableConfig(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UMG",
				"DeveloperSettings",
				"GameplayTags",
                "AudioExtensions",//ʹ����FAudioParameter�ṹ��
				"AudioModulation",//��ģ����Ҫ��Ŀ���ò��
				/*
					{
						"Name": "AudioModulation",
						"Enabled": true
					},
				 */
				//"MetasoundEngine" //����Metasound�ܷ��ȡSoundWaveʱʹ�ù�
				// ... add private dependencies that you statically link with here ...	
			}
            );
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
