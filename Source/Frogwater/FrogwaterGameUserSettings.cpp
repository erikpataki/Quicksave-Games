// Fill out your copyright notice in the Description page of Project Settings.


#include "FrogwaterGameUserSettings.h"

#include "AudioDevice.h"

enum EGameUserSettingsVersion
{
	/** Version for user game settings. All settings will be wiped if the serialized version differs. */
	UE_GAMEUSERSETTINGS_VERSION = 69
};

UFrogwaterGameUserSettings::UFrogwaterGameUserSettings()
{
	UFrogwaterGameUserSettings::SetToDefaults();
}

void UFrogwaterGameUserSettings::ApplySettings(bool bCheckForCommandLineOverrides)
{
	ApplyAudioSettings();
	ApplyFSRSettings();
	ApplyCASSettings();
	
	Super::ApplySettings(bCheckForCommandLineOverrides);
}

void UFrogwaterGameUserSettings::ApplyNonResolutionSettings()
{
	Super::ApplyNonResolutionSettings();
}

void UFrogwaterGameUserSettings::ApplyHardwareBenchmarkResults()
{
	Super::ApplyHardwareBenchmarkResults();
}

void UFrogwaterGameUserSettings::SaveSettings()
{
	Super::SaveSettings();
}

void UFrogwaterGameUserSettings::ValidateSettings()
{
	Super::ValidateSettings();
}

bool UFrogwaterGameUserSettings::IsDirty() const
{
	return Super::IsDirty();
}

void UFrogwaterGameUserSettings::SetToDefaults()
{
	Super::SetToDefaults();

	MasterAudioVolume = 1;
	DialogueAudioVolume = 1;
	MusicAudioVolume = 1;
	EffectsAudioVolume = 1;
	VOIPAudioVolume = 1;

	CASQuality = ESettingMode0_2::High;
	FSRMode = ESettingModeFSR::Off;
}

bool UFrogwaterGameUserSettings::IsVersionValid()
{
	return Version == UE_GAMEUSERSETTINGS_VERSION;
}

void UFrogwaterGameUserSettings::UpdateVersion()
{
	Version = UE_GAMEUSERSETTINGS_VERSION;
}

void UFrogwaterGameUserSettings::ApplyAudioSettings()
{
	const FAudioDeviceHandle AudioDeviceHandle = GEngine->GetMainAudioDevice();
	if (!AudioDeviceHandle.IsValid())
		return;
	
	FAudioDevice* AudioDevice = AudioDeviceHandle.GetAudioDevice();
	if(AudioDevice != nullptr)
	{
		for (auto SoundClassTuple : AudioDevice->GetSoundClassPropertyMap())
		{
			USoundClass* SoundClass = SoundClassTuple.Key;

			if (SoundClass == nullptr)
			{
				continue;
			}

			if (SoundClass->GetName().Find(TEXT("Master")) != INDEX_NONE)
			{
				SoundClass->Properties.Volume = MasterAudioVolume;
			}
			else if (SoundClass->GetName().Find(TEXT("Effects")) != INDEX_NONE)
			{
				SoundClass->Properties.Volume = EffectsAudioVolume;
			}
			else if (SoundClass->GetName().Find(TEXT("Music")) != INDEX_NONE)
			{
				SoundClass->Properties.Volume = MusicAudioVolume;
			}
			else if (SoundClass->GetName().Find(TEXT("Dialog")) != INDEX_NONE)
			{
				SoundClass->Properties.Volume = DialogueAudioVolume;
			}
			else if (SoundClass->GetName().Find(TEXT("VOIP")) != INDEX_NONE)
			{
				SoundClass->Properties.Volume = VOIPAudioVolume;
			}
		}
	}

}

void UFrogwaterGameUserSettings::SetMasterAudioVolume(float NewVolume)
{
	MasterAudioVolume = FMath::Clamp(NewVolume, 0.f, 1.f);
}

void UFrogwaterGameUserSettings::SetDialogueAudioVolume(float NewVolume)
{
	DialogueAudioVolume = FMath::Clamp(NewVolume, 0.f, 1.f);
}

void UFrogwaterGameUserSettings::SetEffectsAudioVolume(float NewVolume)
{
	EffectsAudioVolume = FMath::Clamp(NewVolume, 0.f, 1.f);
}

void UFrogwaterGameUserSettings::SetMusicAudioVolume(float NewVolume)
{
	MusicAudioVolume = FMath::Clamp(NewVolume, 0.f, 1.f);
}

void UFrogwaterGameUserSettings::SetVOIPAudioVolume(float NewVolume)
{
	VOIPAudioVolume = FMath::Clamp(NewVolume, 0.f, 1.f);
}

void UFrogwaterGameUserSettings::SetCASQuality(ESettingMode0_2 NewQuality)
{
	// Always enable CAS if FSR is enabled.
	if (NewQuality == ESettingMode0_2::Off && FSRMode != ESettingModeFSR::Off)
		NewQuality = ESettingMode0_2::High;
	
	CASQuality = NewQuality;
}

void UFrogwaterGameUserSettings::SetFSRMode(ESettingModeFSR NewMode)
{
	FSRMode = NewMode;
}

void UFrogwaterGameUserSettings::ApplyCASSettings()
{
	// r.FidelityFX.FSR.RCAS.Enabled: enable CAS (true)
	// r.FidelityFX.FSR.RCAS.Denoise: enable de-noising (disabled)
	// r.FidelityFX.FSR.RCAS.Sharpness: set sharpness (0.2)

	float CASSharpness = 0.f;
	
	switch (CASQuality)
	{
		case ESettingMode0_2::Off:
			CASSharpness = 3.f;
			break;
		case ESettingMode0_2::Low:
			CASSharpness = .5f;
			break;
		case ESettingMode0_2::High:
			CASSharpness = .2f;
			break;;
	}

	auto CASEnabledCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR.RCAS.Enabled"));
	if (CASEnabledCVar)
	{
		// Game crashes when actually turning it off CAS, so instead just make the sharpness very slow.
		//CASEnabledCVar->Set(CASQuality != ESettingMode0_2::Off, ECVF_SetByGameSetting);
		CASEnabledCVar->Set(true, ECVF_SetByGameSetting);
	}
	
	auto CASSharpnessCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR.RCAS.Sharpness"));
	if (CASSharpnessCVar)
	{
		// Setting them all with console for now since console has highest priority and
		// I don't know how to delete console changes.
		CASSharpnessCVar->Set(CASSharpness, ECVF_SetByGameSetting);
	}

	auto FSREnabledCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR.Enabled"));
	if (FSREnabledCVar)
	{
		// FSR needs to be enabled to allow CAS, but don't turn off if FSR is actually being used.
		FSREnabledCVar->Set(true, ECVF_SetByGameSetting);
	}
}

void UFrogwaterGameUserSettings::ApplyFSRSettings()
{
	// r.FidelityFX.FSR.Enabled
	// r.ScreenPercentage
	// r.MipMapLODBias

	float ScreenPercentage = 100.f;
	float MipMapLODBias = 0.f;
	
	switch (FSRMode)
	{
		case ESettingModeFSR::Off:
			// 100% screen % / 0 mipmap
			break;
		case ESettingModeFSR::Performance:
			// 50% screen % / -1.000 mipmap
			ScreenPercentage = 50.f;
			MipMapLODBias = -1.f;
			break;
		case ESettingModeFSR::Balanced:
			// 59% screen % / -0.7606 mipmap
			ScreenPercentage = 59.f;
			MipMapLODBias = -.7606f;
			break;
		case ESettingModeFSR::Quality:
			// 67% screen % / -0.3765 mipmap
			ScreenPercentage = 67.f;
			MipMapLODBias = -.3765f;
			break;
		case ESettingModeFSR::UltraQuality:
			// 77% screen % / -0.5771 mipmap
			ScreenPercentage = 77.f;
			MipMapLODBias = .5771f;
			break;;
	}

	auto FSREnabledCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR.Enabled"));
	if (FSREnabledCVar)
	{
		// If Screen Percentage is less than 100, then able FSR.
		// Since FSR needs to be enabled to allow CAS, keep FSR enabled if CAS is enabled.
		FSREnabledCVar->Set(true, ECVF_SetByGameSetting);
	}

	auto ScreenPercCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ScreenPercentage"));
	if (ScreenPercCVar)
	{
		ScreenPercCVar->Set(ScreenPercentage, ECVF_SetByGameSetting);
	}
	
	auto MipMapCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.MipMapLODBias"));
	if (MipMapCVar)
	{
		MipMapCVar->Set(MipMapLODBias, ECVF_SetByGameSetting);
	}

	// Always enable CAS if FSR is enabled.
	if (CASQuality == ESettingMode0_2::Off && FSRMode != ESettingModeFSR::Off)
	{
		SetCASQuality(ESettingMode0_2::High);
		ApplyCASSettings();
	}
}