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

	//Exit the application
	UFUNCTION(BlueprintCallable, Category = "Game")
	static void MyQuitGame(bool bForce)
	{
		FPlatformMisc::RequestExit(bForce);
	};

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputManagement")
	FString LevelName;
};
