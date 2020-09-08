// Fill out your copyright notice in the Description page of Project Settings.


#include "MFCGameInstance.h"
#include "Engine.h"
#include "TodakBattleArenaSaveGame.h"
#include "WidgetFunctionLibrary.h"
#include "GameFramework/GameUserSettings.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/TouchInterface.h"

class USoundMix;
class USoundClass;

/*TMap<int32,FText> UMFCGameInstance::GraphicalCommands = {
	{0, "r.ScreenPercentage 15"},
	{1, "r.ScreenPercentage 25"},
	{2, "r.ScreenPercentage 75"},
	{3, "r.ScreenPercentage 100"}
};

TMap<int32, FText> UMFCGameInstance::PPCommands = {
	{0, "sg.PostProcessQuality 0"},
	{1, "sg.PostProcessQuality 1"},
	{2, "sg.PostProcessQuality 2"},
	{3, "sg.PostProcessQuality 3"}
};

TMap<int32, FText> UMFCGameInstance::AACommands = {
	{0, "PostProcessAAQuality 0"},
	{1, "PostProcessAAQuality 1"},
	{2, "PostProcessAAQuality 2"},
	{3, "PostProcessAAQuality 3"}
};

TMap<int32, FText> UMFCGameInstance::ShadowCommands = {
	{0, "sg.ShadowQuality 0"},
	{1, "sg.ShadowQuality 1"},
	{2, "sg.ShadowQuality 2"},
	{3, "sg.ShadowQuality 3"}
};

TMap<int32, FText> UMFCGameInstance::FPSCommands = {
	{0, "t.MaxFPS 15"},
	{1, "t.MaxFPS 30"},
	{2, "t.MaxFPS 75"},
	{3, "t.MaxFPS 100"}
};*/

void UMFCGameInstance::Init()
{
	//UGameplayStatics::SetBaseSoundMix(this, MainMusicSoundMix);
	//initialize graphic settings from user last saved settings
	UTodakBattleArenaSaveGame* OptionsSavedGame = UWidgetFunctionLibrary::LoadGameSettings(ID);
	if (OptionsSavedGame)
	{
		if (ScalabilityLevels.Contains(OptionsSavedGame->GraphicalIndex))
		{
			UGameUserSettings* userSettings = GEngine->GetGameUserSettings();
			if (userSettings)
			{
				userSettings->SetOverallScalabilityLevel(OptionsSavedGame->GraphicalIndex);
				userSettings->ApplySettings(false);
			}
			
			//Set audio based on current setting
			if (OptionsSavedGame->MusicAudioOn == false)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("OptionsSavedGame->MusicAudioOn is %s"), OptionsSavedGame->MusicAudioOn ? TEXT("True") : TEXT("False")));
				
				UGameplayStatics::SetSoundMixClassOverride(this, MainMusicSoundMix, MainMusicSoundClass, 0.0f, 1.0f, 0.0f);
				UGameplayStatics::PushSoundMixModifier(this, MainMusicSoundMix);
			}
			else if (OptionsSavedGame->MusicAudioOn == true)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("OptionsSavedGame->MusicAudioOn is %s"), OptionsSavedGame->MusicAudioOn ? TEXT("True") : TEXT("False")));
				UGameplayStatics::SetSoundMixClassOverride(this, MainMusicSoundMix, MainMusicSoundClass, 1.0f, 1.0f, 0.0f);
				UGameplayStatics::PushSoundMixModifier(this, MainMusicSoundMix);
			}
			if (OptionsSavedGame->SFXAudioOn == false)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("OptionsSavedGame->SFXAudioOn is %s"), OptionsSavedGame->SFXAudioOn ? TEXT("True") : TEXT("False")));
				UGameplayStatics::SetSoundMixClassOverride(this, MainSFXSoundMix, MainSFXSoundClass, 0.0f, 1.0f, 0.0f);
				UGameplayStatics::PushSoundMixModifier(this, MainSFXSoundMix);
			}
			else if (OptionsSavedGame->SFXAudioOn == true)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("OptionsSavedGame->SFXAudioOn is %s"), OptionsSavedGame->SFXAudioOn ? TEXT("True") : TEXT("False")));
				UGameplayStatics::SetSoundMixClassOverride(this, MainSFXSoundMix, MainSFXSoundClass, 1.0f, 1.0f, 0.0f);
				UGameplayStatics::PushSoundMixModifier(this, MainSFXSoundMix);
			}
		}
	}
	else
	{
		//Get graphic setting benchmark before applying optimal settings
		UGameUserSettings* userSettings = GEngine->GetGameUserSettings();
		if (userSettings)
		{
			userSettings->RunHardwareBenchmark(10, 1.f, 1.f);
			userSettings->ApplyHardwareBenchmarkResults();
			userSettings->ApplySettings(false);
			userSettings->SaveSettings();
		}
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("OptionsSavedGame->MusicAudioOn is null")));
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("OptionsSavedGame->SFXAudioOn is null")));

		//Set audio on by default
		UGameplayStatics::SetSoundMixClassOverride(this, MainMusicSoundMix, MainMusicSoundClass, 1.0f, 1.0f, 0.0f);
		UGameplayStatics::PushSoundMixModifier(this, MainMusicSoundMix);

		//Set sfx on by default
		UGameplayStatics::SetSoundMixClassOverride(this, MainSFXSoundMix, MainSFXSoundClass, 1.0f, 1.0f, 0.0f);
		UGameplayStatics::PushSoundMixModifier(this, MainSFXSoundMix);
	}
}

void UMFCGameInstance::LoadJoystick(APlayerController* thisController, bool isTrueLevel)
{
	if (isTrueLevel == true)
	{
		thisController->SetVirtualJoystickVisibility(true);
	}
	else
		thisController->SetVirtualJoystickVisibility(false);
}
