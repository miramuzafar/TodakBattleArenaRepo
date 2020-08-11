// Fill out your copyright notice in the Description page of Project Settings.


#include "TodakBattleArenaPlayerController.h"
#include "TodakBattleArenaCharacter.h"
#include "MFCGameInstance.h"

ATodakBattleArenaPlayerController::ATodakBattleArenaPlayerController()
{

}

void ATodakBattleArenaPlayerController::BeginPlay()
{
	//Call the base class
	Super::BeginPlay();

	UMFCGameInstance* thisGI = Cast<UMFCGameInstance>(this->GetGameInstance());
	if (thisGI)
	{
		thisGI->LoadJoystick(this, UGameplayStatics::GetCurrentLevelName(this, true) == LevelName);
	}
}
