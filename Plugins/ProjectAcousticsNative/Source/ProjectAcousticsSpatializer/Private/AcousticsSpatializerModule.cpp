// Copyright (c) 2022 Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "ProjectAcousticsSpatializer.h"
#include "EngineUtils.h"
#include <Features/IModularFeatures.h>
#include <Interfaces/IPluginManager.h>

IMPLEMENT_MODULE(FAcousticsSpatializerModule, ProjectAcousticsSpatializer)

FAcousticsSpatializerModule::FAcousticsSpatializerModule()
{
}

void FAcousticsSpatializerModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin
    // file per-module
    IModularFeatures::Get().RegisterModularFeature(
        FSpatializationPluginFactory::GetModularFeatureName(), &m_SpatializationPluginFactory);
    IModularFeatures::Get().RegisterModularFeature(
        FReverbPluginFactory::GetModularFeatureName(), &m_ReverbPluginFactory);
}

void FAcousticsSpatializerModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.
}

IAudioPluginFactory* FAcousticsSpatializerModule::GetPluginFactory(EAudioPlugin PluginType)
{
    switch (PluginType)
    {
    case EAudioPlugin::SPATIALIZATION:
        return &m_SpatializationPluginFactory;
        break;
    case EAudioPlugin::REVERB:
        return &m_ReverbPluginFactory;
    default:
        return nullptr;
    }
}

void FAcousticsSpatializerModule::RegisterAudioDevice(FAudioDevice* AudioDeviceHandle)
{
    if (!m_RegisteredAudioDevices.Contains(AudioDeviceHandle))
    {
        // Spawn a listener for each audio device
        TAudioPluginListenerPtr NewAcousticsSpatializerPluginListener = TAudioPluginListenerPtr(new FAcousticsSpatializerPluginListener());
        AudioDeviceHandle->RegisterPluginListener(NewAcousticsSpatializerPluginListener);

        m_RegisteredAudioDevices.Add(AudioDeviceHandle);
    }
}

void FAcousticsSpatializerModule::UnregisterAudioDevice(FAudioDevice* AudioDeviceHandle)
{
    m_RegisteredAudioDevices.Remove(AudioDeviceHandle);
    UE_LOG(LogProjectAcousticsSpatializer, Log, TEXT("Audio Device unregistered from Project Acoustics"));
}