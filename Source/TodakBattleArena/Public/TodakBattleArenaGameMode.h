// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TodakBattleArenaGameMode.generated.h"

class UBaseCharacterWidget;

UCLASS(minimalapi)
class ATodakBattleArenaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATodakBattleArenaGameMode();

	virtual void BeginPlay() override;

	class UBaseCharacterWidget* pWidget;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
};


