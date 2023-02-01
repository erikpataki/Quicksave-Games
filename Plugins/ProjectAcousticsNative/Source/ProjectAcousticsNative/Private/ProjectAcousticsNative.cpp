// Copyright (c) 2022 Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
// The Project Acoustics module

#include "ProjectAcousticsNative.h"
#include "AcousticsSourceDataOverride.h"
#include "AcousticsSourceDataOverrideSettings.h"
#include "AcousticsParameterInterface.h"
#include "AcousticsAudioPluginListener.h"

#define LOCTEXT_NAMESPACE "FProjectAcousticsNativeModule"

TAudioSourceDataOverridePtr FSourceDataOverridePluginFactory::CreateNewSourceDataOverridePlugin(FAudioDevice* OwningDevice)
{
    FProjectAcousticsNativeModule* Module = &FModuleManager::GetModuleChecked<FProjectAcousticsNativeModule>("ProjectAcousticsNative");
    if (Module != nullptr)
    {
        Module->RegisterAudioDevice(OwningDevice);
    }

    return TAudioSourceDataOverridePtr(new FAcousticsSourceDataOverride());
}

IAudioPluginFactory* FProjectAcousticsNativeModule::GetPluginFactory(EAudioPlugin PluginType)
{
    switch (PluginType)
    {
    case EAudioPlugin::SOURCEDATAOVERRIDE:
        return &SourceDataOverridePluginFactory;
        break;
    default:
        return nullptr;
    }
}

void FProjectAcousticsNativeModule::RegisterAudioDevice(FAudioDevice* AudioDeviceHandle)
{
    if (!RegisteredAudioDevices.Contains(AudioDeviceHandle))
    {
        // Spawn a listener for each audio device
        TAudioPluginListenerPtr NewAcousticsAudioPluginListener = TAudioPluginListenerPtr(new FAcousticsAudioPluginListener());
        AudioDeviceHandle->RegisterPluginListener(NewAcousticsAudioPluginListener);

        RegisteredAudioDevices.Add(AudioDeviceHandle);
    }
}

void FProjectAcousticsNativeModule::UnregisterAudioDevice(FAudioDevice* AudioDeviceHandle)
{
    RegisteredAudioDevices.Remove(AudioDeviceHandle);
}

void FProjectAcousticsNativeModule::StartupModule()
{
    IModularFeatures::Get().RegisterModularFeature(FSourceDataOverridePluginFactory::GetModularFeatureName(), &SourceDataOverridePluginFactory);

    AcousticsParameterInterface::RegisterInterface();
}

void FProjectAcousticsNativeModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FProjectAcousticsNativeModule, ProjectAcousticsNative)
