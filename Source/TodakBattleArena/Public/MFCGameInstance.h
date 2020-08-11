// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MFCGameInstance.generated.h"

class UTouchInterface;
/**
 * 
 */

UCLASS()
class TODAKBATTLEARENA_API UMFCGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Touch Interface")
	UTouchInterface* BaseTouchInterface;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login System")
	int ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login System")
	FString Username;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login System")
	float Longitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login System")
	float Latitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login System")
	FString Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friends System")
	FString UID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friends System")
	FString UIDTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friends System")
	FString UsernameTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friends System")
	float TotalFriends;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friends System")
	float Long;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friends System")
	float Lat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	int Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	float CurrentPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	float TotalPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	float MaxPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	int Rank;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	float CurrentWin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	float TotalWin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	float TotalLose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	float ConsecutiveWin;

	UFUNCTION(BlueprintCallable, Category = "InputManagement")
	void LoadJoystick(APlayerController* thisController, bool isTrueLevel);

protected:

	virtual void Init();
};
