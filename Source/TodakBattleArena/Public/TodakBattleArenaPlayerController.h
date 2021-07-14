// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TodakBattleArenaPlayerController.generated.h"

class UUserWidget;

/**
 * 
 */
UCLASS()
class TODAKBATTLEARENA_API ATodakBattleArenaPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	ATodakBattleArenaPlayerController();

	virtual void BeginPlay() override;

	class UUserWidget* pWidget;

	class ViewController* UIViewController;

	class UTouchInterface* MFCTouchInterface;

	//Exit the application
	UFUNCTION(BlueprintCallable, Category = "Game")
	static void MyQuitGame(bool bForce)
	{
		FPlatformMisc::RequestExit(bForce);
	};

	UFUNCTION(BlueprintCallable, Category = "Input")
	void ToggleOnInput();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void ToggleOffInput();


protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputManagement")
	FString LevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputManagement")
	TArray<FString> LevelJoystick = { TEXT("ThirdPersonExampleMap"), TEXT("fight_level_1_v2") };
};
