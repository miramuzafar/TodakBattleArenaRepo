// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TodakBattleArenaGameMode.h"
#include "TodakBattleArenaCharacterHUD.h"
#include "TodakBattleArenaCharacter.h"
#include "EngineUtils.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "BaseCharacterWidget.h"

ATodakBattleArenaGameMode::ATodakBattleArenaGameMode()
	:Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// use custom HUD Class
	//HUDClass = ATodakBattleArenaCharacterHUD::StaticClass();
}

void ATodakBattleArenaGameMode::BeginPlay()
{
	//Call the base class
	Super::BeginPlay();

	/*FStringClassReference locWidgetClassRef(TEXT("/Game/Blueprints/CharacterHUD.CharacterHUD_C"));
	if (UClass* locWidgetClass = locWidgetClassRef.TryLoadClass<UBaseCharacterWidget>())
	{
		pWidget = CreateWidget<UBaseCharacterWidget>(this->GetGameInstance(), locWidgetClass);
		if (pWidget) //Check if the asset is assigned in the blueprint
		{
			pWidget->AddToViewport();

			ACharacter* locChar = GetWorld()->GetFirstPlayerController()->GetCharacter();
			if (locChar)
			{
				ATodakBattleArenaCharacter* locUIChar = CastChecked<ATodakBattleArenaCharacter>(locChar);
				if (locUIChar)
				{
					locUIChar->WidgetHUD = pWidget;
				}
			}

		}
	}*/
}

AActor* ATodakBattleArenaGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	UE_LOG(LogTemp, Warning, TEXT("Player"));
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart* currentPlayerStart = *It;
		if (currentPlayerStart->PlayerStartTag != "Taken")
		{
			currentPlayerStart->PlayerStartTag = "Taken";
			return currentPlayerStart;
		}
	}
	return nullptr;
}
