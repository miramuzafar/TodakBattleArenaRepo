// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Engine/DataTable.h"
#include "MasterPS.generated.h"

USTRUCT(BlueprintType)
struct FPointsTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int PointsNeeded;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int TotalPoints;
};

USTRUCT(BlueprintType)
struct FRankTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int WinNeeded;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString RankName;
};
/**
 * 
 */
UCLASS()
class TODAKBATTLEARENA_API AMasterPS : public APlayerState
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UDataTable* PointDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UDataTable* RankDataTable;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMFCGameInstance* MFCGI;

public:

	AMasterPS();

	// Calculate win points to be added to player's point
	UFUNCTION(BlueprintCallable)
	void SumWinPoint();

	// Calculate lose points to be deducted to player's point
	UFUNCTION(BlueprintCallable)
	void DeductLosePoint();

	// Calculate win streak to give star
	UFUNCTION(BlueprintCallable)
	void RewardConsecutiveWin();

	// Calculate current points
	UFUNCTION(BlueprintCallable)
	void CalculatePoint();

	// Calculate current win
	UFUNCTION(BlueprintCallable)
	void CalculateWin();

	// Calculate current lose
	UFUNCTION(BlueprintCallable)
	void CalculateLose();

	// Calculate additional points
	UFUNCTION(BlueprintCallable)
	void CalculateAdditionalPoint();

	// Fetch data from points datatable 
	UFUNCTION(BlueprintCallable)
	void FetchPointTable();

	// Fetch data from win datatable 
	UFUNCTION(BlueprintCallable)
	void FetchWinTable();

protected:

	virtual void BeginPlay();

	//Player Section

	//Player's level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leveling System")
	int Level;

	//Player's current points
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leveling System")
	float CurrentPoints;

	//Player's total points
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leveling System")
	float TotalAllPoints;

	//Player's additional points after level up
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leveling System")
	float AdditionalPoints;

	//Player's points needed to the next level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leveling System")
	float MaxPoints;

	//Player's current win
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	float CurrentWin;

	//Player's current consecutive win
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	float ConsecutiveWin;

	//Player's win needed to rank up
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	float MaxWin;

	//Player's total win
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	float TotalWin;

	//Player's total lose
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	float TotalLose;

	//Player's rank
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	int Rank;

	//Player's rank name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking System")
	FString NameRank;

	//System Section
	//Point for winning
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leveling System")
	float WinPoint;

	//Point for losing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leveling System")
	float LosePoint;


	
};
