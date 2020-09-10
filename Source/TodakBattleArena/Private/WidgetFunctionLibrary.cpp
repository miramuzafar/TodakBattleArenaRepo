// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetFunctionLibrary.h"
#include "Engine.h"
#include "Engine/GameInstance.h"
#include "MFCGameInstance.h"
#include "TodakBattleArenaSaveGame.h"
#include "Components/CheckBox.h"
#include "Components/Button.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "AdvancedIdentityLibrary.h"
#include "AdvancedSessionsLibrary.h"
#include "GameFramework/GameUserSettings.h"

void UWidgetFunctionLibrary::ChangeCheckBoxBehavior(bool IsChecked, UCheckBox* OldCheckBox, UCheckBox* NewCheckedBox, UCheckBox*& NewCheckBox)
{
	if (IsChecked == true)
	{
		if (OldCheckBox != NewCheckedBox)
		{
			OldCheckBox->SetCheckedState(ECheckBoxState::Unchecked);
		}
	}
	else if (IsChecked == false)
	{
		if (OldCheckBox == NewCheckedBox)
		{
			OldCheckBox->SetCheckedState(ECheckBoxState::Checked);
		}
	}
	NewCheckBox = NewCheckedBox;
	return;
}

void UWidgetFunctionLibrary::ChangeButtonImage(UButton* PreviousClickedButton, UButton* NewClickedButton, FButtonStyle OriginalWidgetStyle, UButton*& ReturnNewClickedButton, FButtonStyle& ReturnNewClickedWidgetStyle)
{
	if (PreviousClickedButton != nullptr)
	{
		PreviousClickedButton->SetStyle(OriginalWidgetStyle);
	}
	UButton* CurrentClickedButton = NewClickedButton;
	FButtonStyle OriginalWidgetStyleButton = NewClickedButton->WidgetStyle;

	NewClickedButton->WidgetStyle.Disabled = NewClickedButton->WidgetStyle.Normal;
	NewClickedButton->WidgetStyle.Normal = NewClickedButton->WidgetStyle.Pressed;
	NewClickedButton->WidgetStyle.Pressed = NewClickedButton->WidgetStyle.Pressed;
	NewClickedButton->WidgetStyle.Hovered = NewClickedButton->WidgetStyle.Pressed;

	//Return new clicked button with its original style
	ReturnNewClickedButton = CurrentClickedButton;
	ReturnNewClickedWidgetStyle = OriginalWidgetStyleButton;
}

void UWidgetFunctionLibrary::FlipFlopButtonImageChange(UButton* ClickedButton, bool IsTrue)
{
	//if bool is true, change normal state of button to pressed state image, else invert the process
	if (IsTrue == false)
	{
		if (ClickedButton != nullptr)
		{
			//Set temp slatebrush for normal style before being clicked
			FSlateBrush NormalBrush = ClickedButton->WidgetStyle.Normal;

			//Set new ui button brush style
			ClickedButton->WidgetStyle.Normal = ClickedButton->WidgetStyle.Pressed;
			ClickedButton->WidgetStyle.Hovered = ClickedButton->WidgetStyle.Pressed;
			ClickedButton->WidgetStyle.Pressed = ClickedButton->WidgetStyle.Pressed;
			ClickedButton->WidgetStyle.Disabled = NormalBrush;
			return;
		}
	}
	if (IsTrue == true)
	{
		if (ClickedButton != nullptr)
		{
			//if the button style has been changed before
			if (ClickedButton->WidgetStyle.Normal == ClickedButton->WidgetStyle.Pressed)
			{
				//Set temp slatebrush for normal style before being clicked
				FSlateBrush PressedBrush = ClickedButton->WidgetStyle.Pressed;

				//Set new ui button brush style
				ClickedButton->WidgetStyle.Normal = ClickedButton->WidgetStyle.Disabled;
				ClickedButton->WidgetStyle.Hovered = ClickedButton->WidgetStyle.Normal;
				ClickedButton->WidgetStyle.Pressed = PressedBrush;
				return;
			}
			else
			{
				ClickedButton->WidgetStyle.Normal = ClickedButton->WidgetStyle.Normal;
				ClickedButton->WidgetStyle.Hovered = ClickedButton->WidgetStyle.Normal;
				//ClickedButton->WidgetStyle.Pressed = PressedBrush;
				return;
			}
		}
	}
}

bool UWidgetFunctionLibrary::ChangeAudioState(const UObject* WorldContextObject, bool State, USoundMix* InSoundMixModifier, USoundClass* InSoundClass)
{
	//if current state is audio on, mute them
	if (State == true)
	{
		UGameplayStatics::SetSoundMixClassOverride(WorldContextObject, InSoundMixModifier, InSoundClass, 0.0f, 1.0f, 0.0f);
		UGameplayStatics::PushSoundMixModifier(WorldContextObject, InSoundMixModifier);
		return false;
	}
	//if current state is audio off, unmute them
	if (State == false)
	{
		UGameplayStatics::SetSoundMixClassOverride(WorldContextObject, InSoundMixModifier, InSoundClass, 1.0f, 1.0f, 0.0f);
		UGameplayStatics::PushSoundMixModifier(WorldContextObject, InSoundMixModifier);
		return true;
	}
	return State;
}

UTodakBattleArenaSaveGame* UWidgetFunctionLibrary::LoadGameSettings(int32 PlayerID)
{
	//Does save game exist, if doesnt, return null
	if (UGameplayStatics::DoesSaveGameExist("Options", PlayerID))
	{
		//If does exist, return the available saved games
		UTodakBattleArenaSaveGame* OptionsSavedGame = Cast<UTodakBattleArenaSaveGame>(UGameplayStatics::LoadGameFromSlot("Options", PlayerID));
		if (OptionsSavedGame)
		{
			return OptionsSavedGame;
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("No save game exist")));
		return nullptr;
	}
	return nullptr;
}

UCheckBox* UWidgetFunctionLibrary::ChangeCurrentGraphicSettings(bool IsChecked, UCheckBox* OldCheckBox, UCheckBox* NewCheckedBox, int32 GraphicalIndex)
{
	//Change checked checkbox state
	UCheckBox* CurrCheckBox;
	UWidgetFunctionLibrary::ChangeCheckBoxBehavior(IsChecked, OldCheckBox, NewCheckedBox, CurrCheckBox);

	if (IsChecked)
	{
		return CurrCheckBox;
	}
	return CurrCheckBox;
}

void UWidgetFunctionLibrary::SaveCurrentGraphicSettings(UGameInstance* currGI, UTodakBattleArenaSaveGame* CurrentSaveGame, int32 GraphicIndex, int32 UserIndex, bool MusicOn, bool SFXOn)
{
	//Check if saved game already exist, else create new one
	if (UGameplayStatics::DoesSaveGameExist("Options", UserIndex))
	{
		//apply changes to the current saved game
		CurrentSaveGame->GraphicalIndex = GraphicIndex;
		CurrentSaveGame->MusicAudioOn = MusicOn;
		CurrentSaveGame->SFXAudioOn = SFXOn;
	}
	else
	{
		//Creates new save game
		CurrentSaveGame = Cast<UTodakBattleArenaSaveGame>(UGameplayStatics::CreateSaveGameObject(UTodakBattleArenaSaveGame::StaticClass()));
		if (CurrentSaveGame)
		{
			//apply changes to the current saved game
			CurrentSaveGame->GraphicalIndex = GraphicIndex;
			CurrentSaveGame->MusicAudioOn = MusicOn;
			CurrentSaveGame->SFXAudioOn = SFXOn;

		}
	}
	//Save games to slot 
	if (UGameplayStatics::SaveGameToSlot(CurrentSaveGame, "Options", UserIndex))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("EXIST")));
		UMFCGameInstance* GI = Cast<UMFCGameInstance>(currGI);
		if (GI)
		{
			if (GI->ScalabilityLevels.Contains(GraphicIndex))
			{
				UGameUserSettings* userSettings = GEngine->GetGameUserSettings();
				if (userSettings)
				{
					userSettings->SetOverallScalabilityLevel(GraphicIndex);
					userSettings->ApplySettings(false);
					userSettings->SaveSettings();
				}
				//UGameUserSettings currUserSettings = U
				/*FText GSettings = GraphicalCommands[OptionsSavedGame->GraphicalIndex];
				UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), GSettings.ToString(), UGameplayStatics::GetPlayerController(GetWorld(), ID));*/
			}
			/*if (GI->GraphicalCommands.Contains(GraphicalIndex))
			{
				//Set new graphic settings
				FText GSettings = GI->GraphicalCommands[GraphicalIndex];
				UKismetSystemLibrary::ExecuteConsoleCommand(GEngine->GetWorld(), GSettings.ToString(), UGameplayStatics::GetPlayerController(GEngine->GetWorld(), 0));
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Changed")));

				//return current checkbox
				return CurrCheckBox;
			}*/
		}
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Saved")));
	}
}

void UWidgetFunctionLibrary::GetOnlineAccountID(APlayerController* PlayerController, FString& UniqueID, FString& GName)
{
	FBPUniqueNetId UniqueNetID;
	//Get unique net ID
	//UAdvancedSessionsLibrary::GetUniqueNetIDFromPlayerState(PlayerController->PlayerState, UniqueNetID);

	//UAdvancedSessionsLibrary::UniqueNetIdToString(UniqueNetID, UniqueID);
	//Check if ID is exist
	if (UniqueNetID.IsValid())
	{
		//UAdvancedSessionsLibrary::UniqueNetIdToString(UniqueNetID, UniqueID);
		//UAdvancedIdentityLibrary::GetPlayerNickname(UniqueNetID, GName);
	}
}
