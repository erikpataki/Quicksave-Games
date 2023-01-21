// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "FrogwaterGameUserSettings.generated.h"

UENUM(BlueprintType)
enum class ESettingMode0_2 : uint8
{
	Off,
	Low,
	High
};

UENUM(BlueprintType)
enum class ESettingMode0_3 : uint8
{
	Off,
	Low,
	Medium,
	High
};

UENUM(BlueprintType)
enum class ESettingMode1_3 : uint8
{
	Low,
	Medium,
	High
};

UENUM(BlueprintType)
enum class ESettingMode1_4 : uint8
{
	Low,
	Medium,
	High,
	Ultra
};

UENUM(BlueprintType)
enum class ESettingModeFSR : uint8
{
	Off,
	Performance,
	Balanced,
	Quality,
	UltraQuality
};

/**
 * 
 */
UCLASS()
class FROGWATER_API UFrogwaterGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

	UFrogwaterGameUserSettings();

public:
	UFUNCTION(BlueprintPure, Category="Game User Settings")
	static UFrogwaterGameUserSettings* GetUserSettings() { return Cast<UFrogwaterGameUserSettings>(GetGameUserSettings()); }
	
	virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;
	virtual void ApplyNonResolutionSettings() override;
	virtual void ApplyHardwareBenchmarkResults() override;
	virtual void SaveSettings() override;
	virtual void ValidateSettings() override;
	virtual bool IsDirty() const override;
	virtual void SetToDefaults() override;
	virtual bool IsVersionValid() override;
	virtual void UpdateVersion() override;


// Audio
public:
	UFUNCTION(BlueprintCallable, Category=Settings)
	virtual void ApplyAudioSettings();

	UFUNCTION(BlueprintCallable, Category=Settings)
	void SetMasterAudioVolume(float NewVolume);

	UFUNCTION(BlueprintPure, Category=Settings)
	float GetMasterAudioVolume() const { return MasterAudioVolume; }

	UFUNCTION(BlueprintCallable, Category=Settings)
	void SetDialogueAudioVolume(float NewVolume);

	UFUNCTION(BlueprintPure, Category=Settings)
	float GetDialogueAudioVolume() const { return DialogueAudioVolume; }

	UFUNCTION(BlueprintCallable, Category=Settings)
	void SetEffectsAudioVolume(float NewVolume);

	UFUNCTION(BlueprintPure, Category=Settings)
	float GetEffectsAudioVolume() const { return EffectsAudioVolume; }

	UFUNCTION(BlueprintCallable, Category=Settings)
	void SetMusicAudioVolume(float NewVolume);

	UFUNCTION(BlueprintPure, Category=Settings)
	float GetMusicAudioVolume() const { return MusicAudioVolume; }

	UFUNCTION(BlueprintCallable, Category=Settings)
	void SetVOIPAudioVolume(float NewVolume);

	UFUNCTION(BlueprintPure, Category=Settings)
	float GetVOIPAudioVolume() const { return VOIPAudioVolume; }

protected:
	UPROPERTY(Config)
	float MasterAudioVolume;

	UPROPERTY(Config)
	float DialogueAudioVolume;

	UPROPERTY(Config)
	float EffectsAudioVolume;

	UPROPERTY(Config)
	float MusicAudioVolume;

	UPROPERTY(Config)
	float VOIPAudioVolume;


// AMD FidelityFX
public:
	UFUNCTION(BlueprintCallable, Category=Settings)
	void SetCASQuality(ESettingMode0_2 NewQuality);

	UFUNCTION(BlueprintPure, Category=Settings)
	ESettingMode0_2 GetCASQuality() const { return CASQuality; }

	UFUNCTION(BlueprintCallable, Category=Settings)
	void SetFSRMode(ESettingModeFSR NewMode);

	UFUNCTION(BlueprintPure, Category=Settings)
	ESettingModeFSR GetFSRMode() const { return FSRMode; }

	UFUNCTION(BlueprintCallable, Category=Settings)
	void ApplyCASSettings();

	UFUNCTION(BlueprintCallable, Category=Settings)
	void ApplyFSRSettings();
	
protected:
	UPROPERTY(Config)
	ESettingMode0_2 CASQuality;

	UPROPERTY(Config)
	ESettingModeFSR FSRMode;
};
