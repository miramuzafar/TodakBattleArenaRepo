// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Net/UnrealNetwork.h"
#include "TodakBattleArenaGameMode.generated.h"

class UBaseCharacterWidget;
class APlayerStart;

UCLASS(minimalapi)
class ATodakBattleArenaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATodakBattleArenaGameMode();

	virtual void BeginPlay() override;

	//Replicated Network setup
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	class ATodakGameStateBase* GameStateClass;

	class UBaseCharacterWidget* pWidget;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, Category = "Player")
	APlayerStart* PStart;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
};


