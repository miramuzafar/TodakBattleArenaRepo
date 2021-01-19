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

	/*ACharacter* locChar = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (locChar)
	{
		ATodakBattleArenaCharacter* locUIChar = CastChecked<ATodakBattleArenaCharacter>(locChar);
		if (locUIChar && (locUIChar->LevelName != UGameplayStatics::GetCurrentLevelName(this, true)))
		{
			if (locUIChar->Controller->IsLocalPlayerController())
			{
				FStringClassReference locWidgetClassRef(TEXT("/Game/Blueprints/CharacterHUD.CharacterHUD_C"));
				if (locWidgetClassRef != nullptr)
				{
					CharacterHUDClass = CreateWidget<UBaseCharacterWidget>(this->GetGameInstance(), locWidgetClassRef);
					if (locUIChar->WidgetHUD)
					{
						CharacterHUDClass->AddToViewport();
						locUIChar->WidgetHUD = CharacterHUDClass;
						GetWorld()->GetFirstPlayerController()->ShouldShowMouseCursor();
						UE_LOG(LogTemp, Warning, TEXT("Show mouse cursor is %s "), (GetWorld()->GetFirstPlayerController()->ShouldShowMouseCursor() == GetWorld()->GetFirstPlayerController()->ShouldShowMouseCursor()) ? TEXT("True") : TEXT("False"));
						GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
					}
				}
			}
		}
	}*/
}

