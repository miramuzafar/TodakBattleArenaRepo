// Fill out your copyright notice in the Description page of Project Settings.


#include "TodakBattleArenaCharacterHUD.h"
#include "TodakBattleArenaCharacter.h"
#include "Blueprint/UserWidget.h"
#include "BaseCharacterWidget.h"
#include "Components/ProgressBar.h"

ATodakBattleArenaCharacterHUD::ATodakBattleArenaCharacterHUD()
{

}

void ATodakBattleArenaCharacterHUD::DrawHUD()
{
	Super::DrawHUD();
}

void ATodakBattleArenaCharacterHUD::BeginPlay()
{
	//Call the base class
	Super::BeginPlay();

	FStringClassReference locWidgetClassRef(TEXT("/Game/Blueprints/CharacterHUD.CharacterHUD_C"));
	if (UClass* locWidgetClass = locWidgetClassRef.TryLoadClass<UBaseCharacterWidget>())
	{
		CharacterHUDClass = CreateWidget<UBaseCharacterWidget>(this->GetGameInstance(), locWidgetClass);
		if (CharacterHUDClass)
		{
			CharacterHUDClass->AddToViewport();
			ACharacter* locChar = GetWorld()->GetFirstPlayerController()->GetCharacter();
			if (locChar)
			{
				ATodakBattleArenaCharacter* locUIChar = CastChecked<ATodakBattleArenaCharacter>(locChar);
				if (locUIChar)
				{
					locUIChar->WidgetHUD = CharacterHUDClass;
					GetWorld()->GetFirstPlayerController()->ShouldShowMouseCursor();
					UE_LOG(LogTemp, Warning, TEXT("Show mouse cursor is %s "), (GetWorld()->GetFirstPlayerController()->ShouldShowMouseCursor() == GetWorld()->GetFirstPlayerController()->ShouldShowMouseCursor()) ? TEXT("True") : TEXT("False"));
					GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
				}
			}
		}
	}
}

