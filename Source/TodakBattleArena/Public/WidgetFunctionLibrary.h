// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WidgetFunctionLibrary.generated.h"

class UCheckBox;
class UButton;
class UTodakBattleArenaSaveGame;
class UGameInstance;
class USoundMix;
class USoundClass;
/**
 * 
 */
UCLASS()
class TODAKBATTLEARENA_API UWidgetFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	//Change current state of checkbox
	UFUNCTION(BlueprintCallable, Category = "Graphics|Widget Utils")
	static void ChangeCheckBoxBehavior(bool IsChecked, UCheckBox* OldCheckBox, UCheckBox* NewCheckedBox, UCheckBox*& NewCheckBox);

	//Change current button Image
	UFUNCTION(BlueprintCallable, Category = "Graphics|Widget Utils")
	static void ChangeButtonImage(UButton* PreviousClickedButton, UButton* NewClickedButton, FButtonStyle OriginalWidgetStyle, UButton*& ReturnNewClickedButton, FButtonStyle& ReturnNewClickedWidgetStyle);

	//Switch button image on flip flop mode
	UFUNCTION(BlueprintCallable, Category = "Graphics|Widget Utils")
	static void FlipFlopButtonImageChange(UButton* ClickedButton, bool IsTrue);

	UFUNCTION(BlueprintPure, Category = "Graphics|Widget Utils")
	static bool ChangeAudioState(const UObject* WorldContextObject, bool State, USoundMix* InSoundMixModifier, USoundClass* InSoundClass);

	//Load Game Settings
	UFUNCTION(BlueprintPure, Category = "Graphics|SavedGame Utils")
	static UTodakBattleArenaSaveGame* LoadGameSettings(int32 PlayerID);
	
	UFUNCTION(BlueprintPure, Category = "Graphics|Widget Utils")
	static UCheckBox* ChangeCurrentGraphicSettings(bool IsChecked, UCheckBox* OldCheckBox, UCheckBox* NewCheckedBox, int32 GraphicalIndex);

	UFUNCTION(BlueprintCallable, Category = "Graphics|Widget Utils")
	static void SaveCurrentGraphicSettings(UGameInstance* currGI, UTodakBattleArenaSaveGame* CurrentSaveGame, int32 Index, int32 UserIndex, bool MusicOn, bool SFXOn);
};
