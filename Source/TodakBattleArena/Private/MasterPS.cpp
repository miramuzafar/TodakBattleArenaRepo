// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterPS.h"
#include "Math/UnrealMathUtility.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "MFCGameInstance.h"


AMasterPS::AMasterPS()
{
	//Rank = 1.0f;
	WinPoint = 10.0f;
	LosePoint = 5.0f;
	//Level = 1.0f;

	static ConstructorHelpers::FObjectFinder<UDataTable> PointsDataObject(TEXT("DataTable'/Game/Blueprints/PointsDataTable.PointsDataTable'"));
	if (PointsDataObject.Succeeded())
	{
		PointDataTable = PointsDataObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> RanksDataObject(TEXT("DataTable'/Game/Blueprints/RanksDataTable.RanksDataTable'"));
	if (RanksDataObject.Succeeded())
	{
		RankDataTable = RanksDataObject.Object;
	}

}

void AMasterPS::BeginPlay()
{
	Super::BeginPlay();

	FetchPointTable();
	FetchWinTable();
}

void AMasterPS::SumWinPoint()
{
	MFCGI = Cast<UMFCGameInstance>(GetGameInstance());
	CurrentPoints = MFCGI->CurrentPoints;
	CurrentPoints = CurrentPoints + WinPoint;
	MFCGI->CurrentPoints = CurrentPoints;

	TotalAllPoints = MFCGI->TotalPoints;
	TotalAllPoints = TotalAllPoints + WinPoint;
	MFCGI->TotalPoints = TotalAllPoints;

	CurrentWin = MFCGI->CurrentWin;
	CurrentWin = CurrentWin + 5.0f;
	MFCGI->CurrentWin = CurrentWin;

	TotalWin = MFCGI->TotalWin;
	TotalWin = TotalWin + 1.0f;
	MFCGI->TotalWin = TotalWin;

	ConsecutiveWin = MFCGI->ConsecutiveWin;
	ConsecutiveWin = ConsecutiveWin + 1.0f;
	MFCGI->ConsecutiveWin = ConsecutiveWin;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Win")));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%f"), MFCGI->TotalPoints));
}

void AMasterPS::DeductLosePoint()
{
	MFCGI = Cast<UMFCGameInstance>(GetGameInstance());
	CurrentPoints = MFCGI->CurrentPoints;
	CurrentPoints = CurrentPoints - LosePoint;
	CurrentPoints = FMath::Clamp(CurrentPoints, 0.0f, 102500.0f);
	MFCGI->CurrentPoints = CurrentPoints;

	TotalAllPoints = MFCGI->TotalPoints;
	TotalAllPoints = TotalAllPoints - LosePoint;
	TotalAllPoints = FMath::Clamp(TotalAllPoints, 0.0f, 102500.0f);
	MFCGI->TotalPoints = TotalAllPoints;

	TotalLose = MFCGI->TotalLose;
	TotalLose = TotalLose + 1.0f;
	MFCGI->TotalLose = TotalLose;

	CurrentWin = MFCGI->CurrentWin;
	CurrentWin = CurrentWin - 1.0f;
	CurrentWin = FMath::Clamp(CurrentWin, 0.0f, 9999999.0f);
	MFCGI->CurrentWin = CurrentWin;

	ConsecutiveWin = MFCGI->ConsecutiveWin;
	ConsecutiveWin = 0.0f;
	MFCGI->ConsecutiveWin = ConsecutiveWin;

	TotalWin = MFCGI->TotalWin;
	TotalWin = TotalWin - 1.0f;
	TotalWin = FMath::Clamp(TotalWin, 0.0f, 9999999.0f);

	
}

void AMasterPS::CalculatePoint()
{
	if (CurrentPoints >= MaxPoints)
	{
		MFCGI = Cast<UMFCGameInstance>(GetGameInstance());
		CalculateAdditionalPoint();
		CurrentPoints = 0.0f;
		CurrentPoints = CurrentPoints + AdditionalPoints;
		MFCGI->CurrentPoints = CurrentPoints;

		Level = MFCGI->Level;
		Level = Level + 1;
		MFCGI->Level = Level;
		FetchPointTable();

	}
}

void AMasterPS::CalculateAdditionalPoint()
{
	AdditionalPoints = CurrentPoints - MaxPoints;
}

void AMasterPS::CalculateWin()
{
	if (CurrentWin >= MaxWin)
	{
		MFCGI = Cast<UMFCGameInstance>(GetGameInstance());
		CurrentWin = MFCGI->CurrentWin;
		CurrentWin = 0.0f;
		MFCGI->CurrentWin = CurrentWin;
		Rank = MFCGI->Rank;
		Rank = Rank + 1;
		Rank = FMath::Clamp(Rank, 1, 5);
		MFCGI->Rank = Rank;
		FetchWinTable();
	
	}
}

void AMasterPS::CalculateLose()
{
	if (Rank > 1 && CurrentWin <= 0)
	{
		MFCGI = Cast<UMFCGameInstance>(GetGameInstance());
		CurrentWin = 0.0f;
		Rank = MFCGI->Rank;
		Rank = Rank - 1;
		Rank = FMath::Clamp(Rank, 1, 5);
		MFCGI->Rank = Rank;
		FetchWinTable();
		CurrentWin = MFCGI->CurrentWin;
		CurrentWin = MaxWin - 1.0f;
		CurrentWin = FMath::Clamp(CurrentWin, 0.0f, 9999999.0f);
		MFCGI->CurrentWin = CurrentWin;
	}
}

void AMasterPS::FetchPointTable()
{
	if (PointDataTable)
	{
		MFCGI = Cast<UMFCGameInstance>(GetGameInstance());
		Level = MFCGI->Level;
		FString SLevel = FString::FromInt(Level);
		FName NLevel = FName(*SLevel);
		static const FString ContextString(TEXT("Points Context"));
		FPointsTable* PointNeeded = PointDataTable->FindRow<FPointsTable>(NLevel, ContextString, true);
		if (PointNeeded)
		{
			MFCGI->MaxPoints = PointNeeded->PointsNeeded;
			MaxPoints = PointNeeded->PointsNeeded;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%f"), MFCGI->MaxPoints));
		}
	}
}

void AMasterPS::FetchWinTable()
{
	if (RankDataTable)
	{
		MFCGI = Cast<UMFCGameInstance>(GetGameInstance());
		Rank = MFCGI->Rank;
		FString SRank = FString::FromInt(Rank);
		FName NRank = FName(*SRank);
		static const FString ContextString(TEXT("Rank Context"));
		FRankTable* WinNeed = RankDataTable->FindRow<FRankTable>(NRank, ContextString, true);
		if (WinNeed)
		{
			MaxWin = WinNeed->WinNeeded;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("%f"), MaxWin));
			NameRank = WinNeed->RankName;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, NameRank);
		}
	}
}

void AMasterPS::RewardConsecutiveWin()
{
	MFCGI = Cast<UMFCGameInstance>(GetGameInstance());
	if (ConsecutiveWin == 5.0f)
	{
		CurrentWin = MFCGI->CurrentWin;
		CurrentWin = CurrentWin + 1.0f;
		MFCGI->CurrentWin = CurrentWin;
	}
	else if (ConsecutiveWin == 10.0f)
	{
		CurrentWin = MFCGI->CurrentWin;
		CurrentWin = CurrentWin + 1.0f;
		MFCGI->CurrentWin = CurrentWin;
	}
	else if (ConsecutiveWin == 15.0f)
	{
		CurrentWin = MFCGI->CurrentWin;
		CurrentWin = CurrentWin + 1;
		MFCGI->CurrentWin = CurrentWin;
	}
	else if (ConsecutiveWin == 20.0f)
	{
		CurrentWin = MFCGI->CurrentWin;
		CurrentWin = CurrentWin + 1.0f;
		MFCGI->CurrentWin = CurrentWin;
	}
	else if (ConsecutiveWin == 25.0f)
	{
		CurrentWin = MFCGI->CurrentWin;
		CurrentWin = CurrentWin + 1;
		MFCGI->CurrentWin = CurrentWin;
	}
	else if (ConsecutiveWin == 30.0f)
	{
		CurrentWin = MFCGI->CurrentWin;
		CurrentWin = CurrentWin + 1.0f;
		MFCGI->CurrentWin = CurrentWin;
	}
	else if (ConsecutiveWin == 35.0f)
	{
		CurrentWin = MFCGI->CurrentWin;
		CurrentWin = CurrentWin + 1;
		MFCGI->CurrentWin = CurrentWin;
	}
	else if (ConsecutiveWin == 40.0f)
	{
		CurrentWin = MFCGI->CurrentWin;
		CurrentWin = CurrentWin + 1;
		MFCGI->CurrentWin = CurrentWin;
	}
	else if (ConsecutiveWin == 45.0f)
	{
		CurrentWin = MFCGI->CurrentWin;
		CurrentWin = CurrentWin + 1;
		MFCGI->CurrentWin = CurrentWin;
	}
	else if (ConsecutiveWin == 50.0f)
	{
		CurrentWin = MFCGI->CurrentWin;
		CurrentWin = CurrentWin + 1.0f;
		MFCGI->CurrentWin = CurrentWin;
	}
}

