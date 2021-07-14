// Fill out your copyright notice in the Description page of Project Settings.


#include "TodakBattleArenaPlayerController.h"
#include "TodakBattleArenaCharacter.h"
#include "MFCGameInstance.h"

ATodakBattleArenaPlayerController::ATodakBattleArenaPlayerController()
{
	static ConstructorHelpers::FObjectFinder<UTouchInterface> LocoJoystick(TEXT("TouchInterface'/Engine/MobileResources/HUD/LeftVirtualJoystickOnly.LeftVirtualJoystickOnly'"));

	if (LocoJoystick.Object)
	{
		MFCTouchInterface = LocoJoystick.Object;
	}
	
}

void ATodakBattleArenaPlayerController::BeginPlay()
{
	//Call the base class
	Super::BeginPlay();

	this->ActivateTouchInterface(MFCTouchInterface);
	
	UMFCGameInstance* thisGI = Cast<UMFCGameInstance>(this->GetGameInstance());
	if (thisGI)
	{
		//thisGI->LoadJoystick(this, LevelName == UGameplayStatics::GetCurrentLevelName(this, true));

		for (int i = 0; i < 2; i++)
		{
			if (LevelJoystick[i] == UGameplayStatics::GetCurrentLevelName(this, true))
			{
				thisGI->LoadJoystick(this, true);
				break;
			}
			else
			{
				thisGI->LoadJoystick(this, false);
			}
		}
		
	}
}

void ATodakBattleArenaPlayerController::ToggleOnInput()
{
	EnableInput(Cast<APlayerController>(this));
}

void ATodakBattleArenaPlayerController::ToggleOffInput()
{
	DisableInput(Cast<APlayerController>(this));
	UE_LOG(LogTemp, Warning, TEXT("Player Input OFF"));
}
