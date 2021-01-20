// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MFCGameInstance.generated.h"

class UTouchInterface;
class USoundMix;
class USoundClass;
/**
 * 
 */

UCLASS()
class TODAKBATTLEARENA_API UMFCGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Touch Interface")
	UTouchInterface* BaseTouchInterface;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login System")
	int ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login System")
	FString Username;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login System")
	float Longitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login System")
	float Latitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login System")
	FString Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friends System")
	FString UID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friends System")
	FString UIDTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friends System")
	FString UsernameTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friends System")
	float TotalFriends;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friends System")
	float Long;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friends System")
	float Lat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	int Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	float CurrentPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	float TotalPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	float MaxPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	int Rank;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	float CurrentWin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	float TotalWin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	float TotalLose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	float ConsecutiveWin;

	UFUNCTION(BlueprintCallable, Category = "InputManagement")
	void LoadJoystick(APlayerController* thisController, bool isTrueLevel);

public:

	//Index settings from 0 to 3, where 3 is the highest quality possible//
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphic Settings")
		TMap<int32, FText> GraphicalCommands;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcessing")
		TMap<int32, FText> PPCommands;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anti-Aliasing")
		TMap<int32, FText> AACommands;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShadowsSetting")
		TMap<int32, FText> ShadowCommands;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FramePerSeconds")
		TMap<int32, FText> FPSCommands;*/

	//Index settings from 0 to 3, where 3 is the highest quality possible//
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Graphic Settings|Scalability Level")
		TArray<int32> ScalabilityLevels = { 0,1,2,3 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings|Music")
		class USoundMix* MainMusicSoundMix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings|Music")
		class USoundClass* MainMusicSoundClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings|SFX")
		class USoundMix* MainSFXSoundMix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings|SFX")
		class USoundClass* MainSFXSoundClass;


protected:

	virtual void Init();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Status")
	float MaxAgility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Status")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Status")
	float MaxStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Status")
	bool FirstTimeLogin = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Customization")
	bool IsItFem = true;

	//Saved mesh outfit from character inventory
	//Female
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Customization")
	class USkeletalMesh* SkinFem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Customization")
	class USkeletalMesh* FemTop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Customization")
	class USkeletalMesh* FemBot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Customization")
	class USkeletalMesh* FemHair;

	//Male
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Customization")
	class USkeletalMesh* SkinMal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Customization")
	class USkeletalMesh* MalTop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Customization")
	class USkeletalMesh* MalBot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Customization")
	class USkeletalMesh* MalHair;
};