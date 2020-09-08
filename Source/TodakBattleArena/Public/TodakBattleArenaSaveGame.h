// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "TodakBattleArenaSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class TODAKBATTLEARENA_API UTodakBattleArenaSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GraphicSettings")
	int32 GraphicalIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioSettings|Music")
	bool MusicAudioOn = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioSettings|SFX")
	bool SFXAudioOn = true;
};
