// Copyright Epic Games, Inc. All Rights Reserved.

#include "Frogwater.h"

#include "ISettingsModule.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FFrogwaterModule"

void FFrogwaterModule::StartupModule()
{
	/*if(ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Game", "Spawnables",
			LOCTEXT("RuntimeSettingsName", "Spawnables"), LOCTEXT("RuntimeSettingsDescription", "Configure Spawnables"),
			GetMutableDefault<USpawnableSettings>());
	}*/
}

void FFrogwaterModule::ShutdownModule()
{
	/*if(ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		SettingsModule->UnregisterSettings("Project", "Game", "Spawnables");*/
}

IMPLEMENT_PRIMARY_GAME_MODULE(FFrogwaterModule, Frogwater, "Frogwater");

#undef LOCTEXT_NAMESPACE