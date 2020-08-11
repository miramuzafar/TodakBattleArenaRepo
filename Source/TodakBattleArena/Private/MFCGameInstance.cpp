// Fill out your copyright notice in the Description page of Project Settings.


#include "MFCGameInstance.h"
#include "Engine.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/TouchInterface.h"

void UMFCGameInstance::Init()
{
	
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
