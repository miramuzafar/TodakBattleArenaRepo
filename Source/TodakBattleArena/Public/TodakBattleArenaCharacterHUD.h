// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TodakBattleArenaCharacterHUD.generated.h"

class UBaseCharacterWidget;
/**
 * 
 */
UCLASS()
class TODAKBATTLEARENA_API ATodakBattleArenaCharacterHUD : public AHUD
{
	GENERATED_BODY()

public:

	ATodakBattleArenaCharacterHUD();
	
	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

	class UBaseCharacterWidget* CharacterHUDClass;

};
