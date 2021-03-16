// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\GestureInputsFunctions.h"
#include "GestureMathLibrary.h"
#include "Engine.h"
#include "TodakBattleArenaCharacter.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "BaseCharacterWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UGestureInputsFunctions::RightSwipeArea(ATodakBattleArenaCharacter* PlayerCharacter, FFingerIndex* FingerIndex, FVector2D Line1End)
{
	//Get player character
	ATodakBattleArenaCharacter* PlayerChar = Cast<ATodakBattleArenaCharacter>(PlayerCharacter);
	if (!PlayerChar)
		return;

	//Get screen sizes, as well as bot point of the screen
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	UE_LOG(LogTemp, Warning, TEXT("ViewportSize : %s"), *ViewportSize.ToString());

	//Get Top right point of the screen
	const FVector2D TopRightPoint = FVector2D(ViewportSize.X, 0.0f);
	UE_LOG(LogTemp, Warning, TEXT("TopRightPoint : %s"), *TopRightPoint.ToString());

	//Get centre right point of the screen
	const FVector2D CentRightPoint = FVector2D(ViewportSize.X, ViewportSize.Y/2);
	UE_LOG(LogTemp, Warning, TEXT("CentRightPoint : %s"), *CentRightPoint.ToString());

	//Get 3rd Quarter of the size x screen
	const FVector2D QuartX = FVector2D(ViewportSize.X - (ViewportSize.X / 4), ViewportSize.Y);
	UE_LOG(LogTemp, Warning, TEXT("QuartX : %s"), *QuartX.ToString());

	//Get centre of swipe area between left and right
	float CentSwipeX = QuartX.X + ((ViewportSize.X / 4) / 2);
	UE_LOG(LogTemp, Warning, TEXT("CentSwipeX : %f"), CentSwipeX);

	EInputType Branches;

	//Bot-left and top-right
	bool IsInsideTop = UGestureMathLibrary::IsInsideRect(QuartX.X, 0.0f, ViewportSize.X, CentRightPoint.Y, Line1End.X, Line1End.Y);

	bool IsInsideBot = UGestureMathLibrary::IsInsideRect(QuartX.X, CentRightPoint.Y, ViewportSize.X, ViewportSize.Y, Line1End.X, Line1End.Y);

	if (IsInsideTop)
	{
		UE_LOG(LogTemp, Warning, TEXT("smallcircle"));
		FingerIndex->FromSmallCircle = true;
		if (FingerIndex->FromSmallCircle == true)
		{
			//Check if line is within right circle
			if (FingerIndex->StartLocation.X >= CentSwipeX && Line1End.X >= CentSwipeX)
			{
				UE_LOG(LogTemp, Warning, TEXT("righthand"));
				if (FingerIndex->SwipeActions == EInputType::Pressed)
				{
					FingerIndex->BodyParts = EBodyPart::RightFoot;
				}
				FVector2D& End = Line1End;
				if (UGestureInputsFunctions::DetectLinearSwipe(FingerIndex->StartLocation, End, Branches, FingerIndex->bDo, FingerIndex->RightPoints) == true)
				{
					if ((FingerIndex->StartLocation - Line1End).Size() > 50.0f)
					{
						PlayerChar->RightFoot = false;
						//FingerIndex->bDo = true;
						FingerIndex->StartLocation = FVector2D(0, 0);
						PlayerChar->SwipeActions.Add(Branches);
						PlayerChar->BodyParts.Add(EBodyPart::RightFoot);
						FingerIndex->SwipeActions = Branches;
						PlayerChar->RemoveFromArray();
						FingerIndex->RightPoints.Empty();
						return;
					}
					else
						PlayerChar->RightFoot = true;
				}
			}
			else if (FingerIndex->StartLocation.X < CentSwipeX && Line1End.X < CentSwipeX)
			{
				UE_LOG(LogTemp, Warning, TEXT("lefthand"));
				if (FingerIndex->SwipeActions == EInputType::Pressed)
				{
					FingerIndex->BodyParts = EBodyPart::LeftFoot;
				}
				FVector2D& End = Line1End;
				if (UGestureInputsFunctions::DetectLinearSwipe(FingerIndex->StartLocation, End, Branches, FingerIndex->bDo, FingerIndex->LeftPoints) == true)
				{
					if ((FingerIndex->StartLocation - Line1End).Size() > 50.0f)
					{
						PlayerChar->LeftFoot = false;
						//FingerIndex->bDo = true;
						FingerIndex->StartLocation = FVector2D(0, 0);
						PlayerChar->SwipeActions.Add(Branches);
						PlayerChar->BodyParts.Add(EBodyPart::LeftFoot);
						FingerIndex->SwipeActions = Branches;
						PlayerChar->RemoveFromArray();
						FingerIndex->LeftPoints.Empty();
						return;
					}
					else
						PlayerChar->LeftFoot = true;
				}
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Is Inside Top"));
	}
	//If point is within bot swipe area
	else if (IsInsideBot)
	{
		UE_LOG(LogTemp, Warning, TEXT("Is Inside Bot"));
		if (FingerIndex->FromSmallCircle == false)
		{
			//Check if line is within right area
			if (FingerIndex->StartLocation.X > CentSwipeX && Line1End.X > CentSwipeX)
			{
				UE_LOG(LogTemp, Warning, TEXT("rightfoot"));
				if (FingerIndex->SwipeActions == EInputType::Pressed)
				{
					FingerIndex->BodyParts = EBodyPart::RightHand;
				}
				FVector2D& End = Line1End;
				if (UGestureInputsFunctions::DetectLinearSwipe(FingerIndex->StartLocation, End, Branches, FingerIndex->bDo, FingerIndex->RightPoints) == true)
				{
					if ((FingerIndex->StartLocation - Line1End).Size() > 50.0f)
					{
						//FingerIndex->bDo = true;
						FingerIndex->StartLocation = FVector2D(0, 0);
						PlayerChar->SwipeActions.Add(Branches);
						PlayerChar->BodyParts.Add(EBodyPart::RightHand);
						FingerIndex->SwipeActions = Branches;
						PlayerChar->RemoveFromArray();
						FingerIndex->RightPoints.Empty();
						return;
					}
				}
			}
			//Check if line is within left area
			else if (FingerIndex->StartLocation.X < CentSwipeX && Line1End.X < CentSwipeX)
			{
				UE_LOG(LogTemp, Warning, TEXT("leftfoot"));
				if (FingerIndex->SwipeActions == EInputType::Pressed)
				{
					FingerIndex->BodyParts = EBodyPart::LeftHand;
				}
				FVector2D& End = Line1End;
				if (UGestureInputsFunctions::DetectLinearSwipe(FingerIndex->StartLocation, End, Branches, FingerIndex->bDo, FingerIndex->LeftPoints) == true)
				{
					if ((FingerIndex->StartLocation - Line1End).Size() > 50.0f)
					{
						//FingerIndex->bDo = true;
						FingerIndex->StartLocation = FVector2D(0, 0);
						PlayerChar->SwipeActions.Add(Branches);
						PlayerChar->BodyParts.Add(EBodyPart::LeftHand);
						FingerIndex->SwipeActions = Branches;
						PlayerChar->RemoveFromArray();
						FingerIndex->RightPoints.Empty();
						return;
					}
				}
			}
		}
	}
	else
	{
		PlayerChar->RemoveFromArray();
		FingerIndex->LeftPoints.Empty();
		FingerIndex->RightPoints.Empty();
		UE_LOG(LogTemp, Warning, TEXT("Is Outside"));
	}
}

void UGestureInputsFunctions::CircleSwipeArea(ATodakBattleArenaCharacter* PlayerCharacter, FFingerIndex* FingerIndex, FVector2D Line1End)
{
	//float Scale = GEngine->GameViewport->GetDPIScale();

	//Get screen sizes
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	const FVector2D ViewportCenter = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);

	const FVector2D ViewportBottomHalfPoint = FVector2D(ViewportSize.X / 2, ViewportSize.Y);
	const FVector2D ViewportFromTopCenterPoint = FVector2D(ViewportSize.X / 2, 0.0f);

	//Get player character
	ATodakBattleArenaCharacter* PlayerChar = Cast<ATodakBattleArenaCharacter>(PlayerCharacter);
	if (!PlayerChar)
		return;

	EInputType Branches;

	FVector2D Peak;

	//FEET
	//small circles
	//Get distance from bottom circle to the starting vector
	//float DistanceFromStartingPoint = UGestureMathLibrary::RadiusCircle(Line1Start.X, ViewportBottomHalfPoint.X, Line1Start.Y, ViewportBottomHalfPoint.Y);

	//float DistanceFromStartingPoint = sqrt((pow((FingerIndex->StartLocation.X - (ViewportBottomHalfPoint.X)), 2)) + (pow((FingerIndex->StartLocation.Y - (ViewportBottomHalfPoint.Y)), 2)));

	//------------------------------------------------radius of small circle-----------------------------------------------------------------------//
	float RadiusOfCircle = UGestureMathLibrary::RadiusCircle(ViewportCenter.X, (ViewportSize.X) / 1.7f, ViewportSize.Y, ViewportSize.Y / 1.7f);


	//float RadiusOfCircle = sqrt((pow((ViewportCenter.X - (ViewportCenter.X) / 2), 2)) + (pow(((ViewportCenter.Y / 0.7f) - (ViewportSize.Y)), 2)));
	//ViewportCenter.X - (ViewportCenter.X) / 2;

	//Distance from current vector to the bottom small circle point
	//float DistanceFromLastPointToTheCircleCenter = UGestureMathLibrary::RadiusCircle(Line1End.X, ViewportBottomHalfPoint.X, Line1End.Y, ViewportBottomHalfPoint.Y);
	//float DistanceFromLastPointToTheCircleCenter = sqrt((pow((Line1End.X - ViewportBottomHalfPoint.X), 2)) + (pow((Line1End.Y - ViewportBottomHalfPoint.Y), 2)));

	//float Distances = sqrt((pow((Line1End.X - FingerIndex->StartLocation.X), 2)) + (pow((Line1End.Y - FingerIndex->StartLocation.Y), 2)));

	//UE_LOG(LogTemp, Warning, TEXT("Line1End - Line1Start : %f"), Distances);
	UE_LOG(LogTemp, Warning, TEXT("radius : %f"), RadiusOfCircle);

	//.......................................................................................................................................................//

	//HANDS

	//Big circles
	//Get distance from upper circle to the starting vector
	//float DistanceFromStartingPoint1 = UGestureMathLibrary::RadiusCircle(Line1Start.X, ViewportCenter.X, Line1Start.Y, 0.0f);
	//float DistanceFromStartingPoint1 = sqrt((pow((FingerIndex->StartLocation.X - (ViewportCenter.X)), 2)) + (pow((FingerIndex->StartLocation.Y), 2)));

	//Distance from current vector to the top small circle point
	//float DistanceFromLastPointToTheCircleCenter1 = UGestureMathLibrary::RadiusCircle(Line1End.X, ViewportCenter.X, Line1End.Y, 0.0f);

	//float DistanceFromLastPointToTheCircleCenter1 = sqrt((pow((Line1End.X - ViewportCenter.X), 2)) + (pow((Line1End.Y - (0.0f)), 2)));

	//.......................................................................................................................................................//

	//FVector2D FirstIntersect;
	//FVector2D SecondIntersect;

	//Get intersection points of both circles
	//UGestureMathLibrary::CircleIntersectCheckers(ViewportBottomHalfPoint, ViewportFromTopCenterPoint, RadiusOfCircle, ViewportSize.Y, FirstIntersect, SecondIntersect);

	//float Dist = ViewportSize.Y - RadiusOfCircle;

	//bool T = DistanceFromStartingPoint1 < RadiusOfCircle;

	//Checkers
	bool IsInsideSmallCircle = UGestureMathLibrary::IsInsideCircle(ViewportSize.X / 2, ViewportSize.Y, RadiusOfCircle, Line1End.X, Line1End.Y);
	bool IsInsideBigCircle = UGestureMathLibrary::IsInsideCircle(ViewportSize.X / 2, 0.0f, ViewportSize.Y, Line1End.X, Line1End.Y);

	//bool SmallCircleArea = (DistanceFromStartingPoint < RadiusOfCircle) && (DistanceFromLastPointToTheCircleCenter < RadiusOfCircle) && (FingerIndex->StartLocation.Y < ViewportSize.Y);
	//bool BigCircleArea = (DistanceFromStartingPoint1 < ViewportSize.Y) && (DistanceFromStartingPoint1 < ViewportSize.Y) && (DistanceFromLastPointToTheCircleCenter > RadiusOfCircle);

	//If point is within small circle area
	if (IsInsideSmallCircle)
	{
		UE_LOG(LogTemp, Warning, TEXT("smallcircle"));
		FingerIndex->FromSmallCircle = true;
		if (FingerIndex->FromSmallCircle == true)
		{
			//Check if line is within right circle
			if (FingerIndex->StartLocation.X >= ViewportCenter.X && Line1End.X >= ViewportCenter.X)
			{
				if (FingerIndex->SwipeActions == EInputType::Pressed)
				{
					FingerIndex->BodyParts = EBodyPart::RightFoot;
				}
				//PlayerChar->BodyParts.AddUnique(EBodyPart::RightFoot);
				//UE_LOG(LogTemp, Warning, TEXT("Current Location : %f"), (FingerIndex->StartLocation - Line1End).Size());
				//Get the highest curve vector if possible
				/*if (Line1End.X >= FingerIndex->StartLocation.X)
				{
					//UE_LOG(LogTemp, Warning, TEXT("Length : %f"), FGenericPlatformMath::Abs(Line1End.Y - FingerIndex->StartLocation.Y));
					//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Length : %f"), FGenericPlatformMath::Abs(Line1End.Y - FingerIndex->StartLocation.Y)));
					if (FingerIndex->RightPoints.Num() <= 0 && Line1End != FingerIndex->StartLocation && FGenericPlatformMath::Abs(Line1End.Y - FingerIndex->StartLocation.Y) >= 5.0f)
					{
						FingerIndex->RightPoints.AddUnique(Line1End);
					}
					else if (FingerIndex->RightPoints.Num() >= 1 && Line1End != FingerIndex->StartLocation && FGenericPlatformMath::Abs(Line1End.Y - FingerIndex->StartLocation.Y) >= 5.0f)
					{
						FVector2D MaxV = FMath::Max<FVector2D>(FingerIndex->RightPoints);
						if (Line1End.X > MaxV.X)
						{
							FingerIndex->RightPoints.AddUnique(Line1End);
						}
						else if (Line1End.X < MaxV.X)
						{
							//UE_LOG(LogTemp, Warning, TEXT("check angle"));
							Peak = MaxV;

							//Get the curve angle

							UE_LOG(LogTemp, Warning, TEXT("check angle"));
							if (FingerIndex->RightPoints.Num() > 5)
							{
								if (UGestureInputsFunctions::DetectCurveSwipe(FingerIndex->RightPoints, FingerIndex->StartLocation, Line1End, Peak, FingerIndex->bDo, Branches) == true)
								{
									FingerIndex->bDo = true;
									FingerIndex->StartLocation = FVector2D(0, 0);
									PlayerChar->SwipeActions.Add(Branches);
									PlayerChar->BodyParts.Add(EBodyPart::RightFoot);
									PlayerChar->RemoveFromArray();
									return;
								}
							}
						}
					}
					else
						goto RightFoot;
				}*/
				//else
				//{
					//RightFoot:
				FVector2D& End = Line1End;
				if (UGestureInputsFunctions::DetectLinearSwipe(FingerIndex->StartLocation, End, Branches, FingerIndex->bDo, FingerIndex->RightPoints) == true)
				{
					if ((FingerIndex->StartLocation - Line1End).Size() > 50.0f)
					{
						PlayerChar->RightFoot = false;
						//FingerIndex->bDo = true;
						FingerIndex->StartLocation = FVector2D(0, 0);
						PlayerChar->SwipeActions.Add(Branches);
						PlayerChar->BodyParts.Add(EBodyPart::RightFoot);
						FingerIndex->SwipeActions = Branches;
						PlayerChar->RemoveFromArray();
						FingerIndex->RightPoints.Empty();
						return;
					}
					else
						PlayerChar->RightFoot = true;
				}

				//}

				//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("%s"), *GETENUMSTRING("EBodyPart", EBodyPart::RightFoot)));
				//UE_LOG(LogTemp, Warning, TEXT("Right foot"));
			}

			//Check if line is within left circle
			else if (FingerIndex->StartLocation.X < ViewportCenter.X && Line1End.X < ViewportCenter.X)
			{
				if (FingerIndex->SwipeActions == EInputType::Pressed)
				{
					FingerIndex->BodyParts = EBodyPart::LeftFoot;
				}
				//PlayerChar->BodyParts.AddUnique(EBodyPart::LeftFoot);
				//UE_LOG(LogTemp, Warning, TEXT("Current Location : %f"), (FingerIndex->StartLocation - Line1End).Size());
				//Get the highest curve vector if possible
				/*if (Line1End.X <= FingerIndex->StartLocation.X)
				{
					//UE_LOG(LogTemp, Warning, TEXT("Length : %f"), FGenericPlatformMath::Abs(Line1End.Y - FingerIndex->StartLocation.Y));
					//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Length : %f"), FGenericPlatformMath::Abs(Line1End.Y - FingerIndex->StartLocation.Y)));

					if (FingerIndex->LeftPoints.Num() <= 0 && Line1End != FingerIndex->StartLocation && FGenericPlatformMath::Abs(Line1End.Y - FingerIndex->StartLocation.Y) >= 5.0f)
					{
						FingerIndex->LeftPoints.AddUnique(Line1End);
					}
					else if (FingerIndex->LeftPoints.Num() >= 1 && Line1End != FingerIndex->StartLocation && FGenericPlatformMath::Abs(Line1End.Y - FingerIndex->StartLocation.Y) >= 5.0f)
					{
						FVector2D MinV = FMath::Min<FVector2D>(FingerIndex->LeftPoints);
						if (Line1End.X < MinV.X)
						{
							FingerIndex->LeftPoints.AddUnique(Line1End);
						}
						else if (Line1End.X > MinV.X)
						{
							//UE_LOG(LogTemp, Warning, TEXT("check angle"));
							Peak = MinV;

							//Get the curve angle
							UE_LOG(LogTemp, Warning, TEXT("check angle"));
							if (FingerIndex->LeftPoints.Num() > 5)
							{
								if (UGestureInputsFunctions::DetectCurveSwipe(FingerIndex->LeftPoints, FingerIndex->StartLocation, Line1End, Peak, FingerIndex->bDo, Branches) == true)
								{
									FingerIndex->bDo = true;
									FingerIndex->StartLocation = FVector2D(0, 0);
									PlayerChar->SwipeActions.Add(Branches);
									PlayerChar->BodyParts.Add(EBodyPart::LeftFoot);
									PlayerChar->RemoveFromArray();
									return;
								}
							}
						}
					}
					else
						goto LeftFoot;
				}*/
				//else
				//{
					//LeftFoot:
				FVector2D& End = Line1End;
				if (UGestureInputsFunctions::DetectLinearSwipe(FingerIndex->StartLocation, End, Branches, FingerIndex->bDo, FingerIndex->LeftPoints) == true)
				{
					if ((FingerIndex->StartLocation - Line1End).Size() > 50.0f)
					{
						PlayerChar->LeftFoot = false;
						//FingerIndex->bDo = true;
						FingerIndex->StartLocation = FVector2D(0, 0);
						PlayerChar->SwipeActions.Add(Branches);
						PlayerChar->BodyParts.Add(EBodyPart::LeftFoot);
						FingerIndex->SwipeActions = Branches;
						PlayerChar->RemoveFromArray();
						FingerIndex->LeftPoints.Empty();
						return;
					}
					else
						PlayerChar->LeftFoot = true;
				}
				//}
				//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("%s"), *GETENUMSTRING("EBodyPart", EBodyPart::LeftFoot)));
				//UE_LOG(LogTemp, Warning, TEXT("Left foot"));
			}
		}
	}

	//If point is within big circle area
	else if (IsInsideBigCircle)
	{
		UE_LOG(LogTemp, Warning, TEXT("bigcircle"));
		if (FingerIndex->FromSmallCircle == false)
		{
			//Check if line is within right circle
			if (FingerIndex->StartLocation.X > ViewportCenter.X && Line1End.X > ViewportCenter.X)
			{
				if (FingerIndex->SwipeActions == EInputType::Pressed)
				{
					FingerIndex->BodyParts = EBodyPart::RightHand;
				}
				//PlayerChar->BodyParts.AddUnique(EBodyPart::RightHand);
				//UE_LOG(LogTemp, Warning, TEXT("Current Location : %f"), (FingerIndex->StartLocation - Line1End).Size());
				//Get the highest curve vector if possible
				/*if (Line1End.X >= FingerIndex->StartLocation.X)
				{
					//UE_LOG(LogTemp, Warning, TEXT("Length : %f"), FGenericPlatformMath::Abs(Line1End.Y - FingerIndex->StartLocation.Y));
					//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Length : %f"), FGenericPlatformMath::Abs(Line1End.Y - FingerIndex->StartLocation.Y)));
					if (FingerIndex->RightPoints.Num() <= 0 && Line1End != FingerIndex->StartLocation && FGenericPlatformMath::Abs(Line1End.Y - FingerIndex->StartLocation.Y) >= 5.0f)
					{
						FingerIndex->RightPoints.AddUnique(Line1End);
					}
					else if (FingerIndex->RightPoints.Num() >= 1 && Line1End != FingerIndex->StartLocation && FGenericPlatformMath::Abs(Line1End.Y - FingerIndex->StartLocation.Y) >= 5.0f)
					{
						FVector2D MaxV = FMath::Max<FVector2D>(FingerIndex->RightPoints);
						if (Line1End.X > MaxV.X)
						{
							FingerIndex->RightPoints.AddUnique(Line1End);
						}
						else if (Line1End.X < MaxV.X)
						{
							//UE_LOG(LogTemp, Warning, TEXT("check angle"));
							Peak = MaxV;

							//Get the curve angle
							UE_LOG(LogTemp, Warning, TEXT("check angle"));
							if (FingerIndex->RightPoints.Num() > 5)
							{
								if (UGestureInputsFunctions::DetectCurveSwipe(FingerIndex->RightPoints, FingerIndex->StartLocation, Line1End, Peak, FingerIndex->bDo, Branches) == true)
								{
									FingerIndex->bDo = true;
									FingerIndex->StartLocation = FVector2D(0, 0);
									PlayerChar->SwipeActions.Add(Branches);
									PlayerChar->BodyParts.Add(EBodyPart::RightHand);
									PlayerChar->RemoveFromArray();
									return;
								}
							}
						}
					}
					else
						goto RightHand;
				}*/
				//else
				//{
					//RightHand:
				FVector2D& End = Line1End;
				if (UGestureInputsFunctions::DetectLinearSwipe(FingerIndex->StartLocation, End, Branches, FingerIndex->bDo, FingerIndex->RightPoints) == true)
				{
					if ((FingerIndex->StartLocation - Line1End).Size() > 50.0f)
					{
						//FingerIndex->bDo = true;
						FingerIndex->StartLocation = FVector2D(0, 0);
						PlayerChar->SwipeActions.Add(Branches);
						PlayerChar->BodyParts.Add(EBodyPart::RightHand);
						FingerIndex->SwipeActions = Branches;
						PlayerChar->RemoveFromArray();
						FingerIndex->RightPoints.Empty();
						return;
					}
				}
				//}

				//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Right hand")));
				//UE_LOG(LogTemp, Warning, TEXT("Right hand"));
			}

			//Check if line is within left circle
			else if (FingerIndex->StartLocation.X < ViewportCenter.X && Line1End.X < ViewportCenter.X)
			{
				if (FingerIndex->SwipeActions == EInputType::Pressed)
				{
					FingerIndex->BodyParts = EBodyPart::LeftHand;
				}
				//PlayerChar->BodyParts.AddUnique(EBodyPart::LeftHand);
				//UE_LOG(LogTemp, Warning, TEXT("Current Location : %f"), (FingerIndex->StartLocation - Line1End).Size());
				//Get the highest curve vector if possible
				/*if (Line1End.X <= FingerIndex->StartLocation.X)
				{
					//UE_LOG(LogTemp, Warning, TEXT("Length : %f"), FGenericPlatformMath::Abs(Line1End.Y - FingerIndex->StartLocation.Y));
					//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Length : %f"), FGenericPlatformMath::Abs(Line1End.Y - FingerIndex->StartLocation.Y)));
					if (FingerIndex->LeftPoints.Num() <= 0 && Line1End != FingerIndex->StartLocation && FGenericPlatformMath::Abs(Line1End.Y - FingerIndex->StartLocation.Y) >= 5.0f)
					{
						FingerIndex->LeftPoints.AddUnique(Line1End);
					}
					else if (FingerIndex->LeftPoints.Num() >= 1 && Line1End != FingerIndex->StartLocation && FGenericPlatformMath::Abs(Line1End.Y - FingerIndex->StartLocation.Y) >= 5.0f)
					{
						FVector2D MinV = FMath::Min<FVector2D>(FingerIndex->LeftPoints);
						if (Line1End.X < MinV.X)
						{
							FingerIndex->LeftPoints.AddUnique(Line1End);
						}
						else if (Line1End.X > MinV.X)
						{
							Peak = MinV;
							//(FingerIndex->StartLocation - Line1End).Size() > 100.0f &&
							//Get the curve angle
							UE_LOG(LogTemp, Warning, TEXT("check angle"));
							if (FingerIndex->LeftPoints.Num() > 5)
							{
								if (UGestureInputsFunctions::DetectCurveSwipe(FingerIndex->LeftPoints, FingerIndex->StartLocation, Line1End, Peak, FingerIndex->bDo, Branches) == true)
								{
									FingerIndex->bDo = true;
									FingerIndex->StartLocation = FVector2D(0, 0);
									PlayerChar->SwipeActions.Add(Branches);
									PlayerChar->BodyParts.Add(EBodyPart::LeftHand);
									PlayerChar->RemoveFromArray();
									return;
								}
							}

						}
					}
					else
						goto LeftHand;
				}*/
				//else
				//{
					//LeftHand:
				FVector2D& End = Line1End;
				if (UGestureInputsFunctions::DetectLinearSwipe(FingerIndex->StartLocation, End, Branches, FingerIndex->bDo, FingerIndex->LeftPoints) == true)
				{
					if ((FingerIndex->StartLocation - Line1End).Size() > 50.0f)
					{
						//FingerIndex->bDo = true;
						FingerIndex->StartLocation = FVector2D(0, 0);
						PlayerChar->SwipeActions.Add(Branches);
						PlayerChar->BodyParts.Add(EBodyPart::LeftHand);
						FingerIndex->SwipeActions = Branches;
						PlayerChar->RemoveFromArray();
						FingerIndex->RightPoints.Empty();
						return;
					}
				}
				//}

				//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Left hand")));
				//UE_LOG(LogTemp, Warning, TEXT("Left hand"));
			}
		}
	}
	else
	{
		PlayerChar->RemoveFromArray();
		FingerIndex->LeftPoints.Empty();
		FingerIndex->RightPoints.Empty();
	}
}

bool UGestureInputsFunctions::DetectCurveSwipe(TArray<FVector2D> Points, FVector2D Line1Start, FVector2D Line1End, FVector2D Peak, bool Dos, EInputType& Branches)
{
	float Degree;

	FVector2D Pivot = FVector2D(Line1Start.X, Peak.Y);

	if (Line1End.Y < Peak.Y && Line1End.Y < Line1Start.Y && Peak.X < Line1Start.X && Peak.X < Line1End.X && Line1End.X < Line1Start.X)
	{
		Degree = UGestureMathLibrary::AngleBetweenVectors(Pivot, Line1Start, Line1End, Peak);
		float ArcLength = (2 * PI)*UGestureMathLibrary::RadiusCircle(Line1Start.X, Pivot.X, Line1Start.Y, Pivot.Y)*(Degree / 360);
		float VectorLength = UGestureMathLibrary::RadiusCircle(Line1End.X, Line1Start.X, Line1End.Y, Line1Start.Y);
		UE_LOG(LogTemp, Warning, TEXT("ArcLength %f;"), (ArcLength));
		UE_LOG(LogTemp, Warning, TEXT("VectorLength %f;"), VectorLength);
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, FString::Printf(TEXT("ArcLength : %f"), ArcLength));
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, FString::Printf(TEXT("VectorLength : %f"), VectorLength));

		if (Degree >= 90.0f && Degree < 180.0f)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, FString::Printf(TEXT("Angle : %f"), Degree));
			if (!Dos)
			{
				Branches = EInputType::UpwardLeftCurve;
				UE_LOG(LogTemp, Warning, TEXT("upward left curve"));
				//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("%s"), *GETENUMSTRING("EInputType", Branches)));
				//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, FString::Printf(TEXT("Angle : %f"), Degree));
				UE_LOG(LogTemp, Warning, TEXT("Angle : %f"), Degree);

				return true;
			}
		}
	}
	else if (Line1End.Y > Peak.Y && Line1End.Y > Line1Start.Y && Peak.X < Line1Start.X && Peak.X < Line1End.X && Line1End.X < Line1Start.X)
	{
		Degree = UGestureMathLibrary::AngleBetweenVectors(Pivot, Line1Start, Line1End, Peak);
		float ArcLength = (2 * PI)*UGestureMathLibrary::RadiusCircle(Line1Start.X, Pivot.X, Line1Start.Y, Pivot.Y)*(Degree / 360);
		float VectorLength = UGestureMathLibrary::RadiusCircle(Line1End.X, Line1Start.X, Line1End.Y, Line1Start.Y);
		UE_LOG(LogTemp, Warning, TEXT("ArcLength %f;"), (ArcLength));
		UE_LOG(LogTemp, Warning, TEXT("VectorLength %f;"), VectorLength);
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, FString::Printf(TEXT("ArcLength : %f"), ArcLength));
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, FString::Printf(TEXT("VectorLength : %f"), VectorLength));

		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, FString::Printf(TEXT("Angle : %f"), Degree));
		if (Degree >= 90.0f && Degree < 180.0f)
		{
			if (!Dos)
			{

				Branches = EInputType::DownwardLeftCurve;
				UE_LOG(LogTemp, Warning, TEXT("downward left curve"));
				//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("%s"), *GETENUMSTRING("EInputType", Branches)));
				//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, FString::Printf(TEXT("Angle : %f"), Degree));
				UE_LOG(LogTemp, Warning, TEXT("Angle : %f"), Degree);

				return true;
			}
		}

	}
	else if (Line1End.Y < Peak.Y && Line1End.Y < Line1Start.Y && Peak.X > Line1Start.X && Peak.X > Line1End.X && Line1End.X > Line1Start.X)
	{
		Degree = UGestureMathLibrary::AngleBetweenVectors(Pivot, Line1Start, Line1End, Peak);
		float ArcLength = (2 * PI)*UGestureMathLibrary::RadiusCircle(Line1Start.X, Pivot.X, Line1Start.Y, Pivot.Y)*(Degree / 360);
		float VectorLength = UGestureMathLibrary::RadiusCircle(Line1End.X, Line1Start.X, Line1End.Y, Line1Start.Y);
		UE_LOG(LogTemp, Warning, TEXT("ArcLength %f;"), (ArcLength));
		UE_LOG(LogTemp, Warning, TEXT("VectorLength %f;"), VectorLength);
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, FString::Printf(TEXT("ArcLength : %f"), ArcLength));
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, FString::Printf(TEXT("VectorLength : %f"), VectorLength));
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, FString::Printf(TEXT("Angle : %f"), Degree));

		if (Degree >= 90.0f && Degree < 180.0f)
		{
			if (!Dos)
			{

				Branches = EInputType::UpwardRightCurve;
				UE_LOG(LogTemp, Warning, TEXT("upward right curve"));
				//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("%s"), *GETENUMSTRING("EInputType", Branches)));
				//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, FString::Printf(TEXT("Angle : %f"), Degree));
				UE_LOG(LogTemp, Warning, TEXT("Angle : %f"), Degree);

				return true;
			}
		}

	}
	else if (Line1End.Y > Peak.Y && Line1End.Y > Line1Start.Y && Peak.X > Line1Start.X && Peak.X > Line1End.X && Line1End.X > Line1Start.X)
	{
		Degree = UGestureMathLibrary::AngleBetweenVectors(Pivot, Line1Start, Line1End, Peak);
		float ArcLength = (2 * PI)*UGestureMathLibrary::RadiusCircle(Line1Start.X, Pivot.X, Line1Start.Y, Pivot.Y)*(Degree / 360);
		float VectorLength = UGestureMathLibrary::RadiusCircle(Line1End.X, Line1Start.X, Line1End.Y, Line1Start.Y);
		UE_LOG(LogTemp, Warning, TEXT("ArcLength %f;"), (ArcLength));
		UE_LOG(LogTemp, Warning, TEXT("VectorLength %f;"), VectorLength);
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, FString::Printf(TEXT("ArcLength : %f"), ArcLength));
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, FString::Printf(TEXT("VectorLength : %f"), VectorLength));
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, FString::Printf(TEXT("Angle : %f"), Degree));
		if (Degree >= 90.0f && Degree < 180.0f)
		{
			if (!Dos)
			{
				Branches = EInputType::DownwardRightCurve;
				UE_LOG(LogTemp, Warning, TEXT("downward right curve"));
				//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("%s"), *GETENUMSTRING("EInputType", Branches)));
				//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, FString::Printf(TEXT("Angle : %f"), Degree));
				UE_LOG(LogTemp, Warning, TEXT("Angle : %f"), Degree);

				return true;
			}
		}

	}
	else
		return UGestureInputsFunctions::DetectLinearSwipe(Line1Start, Peak, Branches, Dos, Points);

	return false;
}

bool UGestureInputsFunctions::DetectLinearSwipe(FVector2D Line1Start, FVector2D Line1End, EInputType& Branches, bool Dos, TArray<FVector2D> Points)
{
	/*const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	const FVector2D ViewportCenter = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);

	const FVector2D ViewportBottomHalfPoint = FVector2D(ViewportSize.X / 2, ViewportSize.Y);
	const FVector2D ViewportFromTopCenterPoint = FVector2D(ViewportSize.X / 2, 0.0f);*/

	if (!Dos)
	{
		float x;
		float y;

		float AbsX;
		float AbsY;

		UGestureMathLibrary::AbsoluteValueOfTwoVectors(Line1Start, Line1End, x, y, AbsX, AbsY);

		//UE_LOG(LogTemp, Warning, TEXT("NewVector : %s"), *FVector2D(x, y).ToString());
		//UE_LOG(LogTemp, Warning, TEXT("NewAbsX : %f"), AbsX);
		//UE_LOG(LogTemp, Warning, TEXT("NewAbsY : %f"), AbsY);

		/*if (AbsX > AbsY)
		{
			if (x > 50.0f)
			{
				Branches = EInputType::Left;
				UE_LOG(LogTemp, Warning, TEXT("Left"));
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("%s"), *GETENUMSTRING("EInputType", Branches)));
				return true;
			}
			else if (x < -50.0f)
			{
				Branches = EInputType::Right;
				UE_LOG(LogTemp, Warning, TEXT("Right"));
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("%s"), *GETENUMSTRING("EInputType", Branches)));
				return true;
			}
		}*/
		if (AbsY > AbsX)
		{
			if (y > 50.0f)
			{
				Branches = EInputType::Up;
				UE_LOG(LogTemp, Warning, TEXT("Up"));
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("%s"), *GETENUMSTRING("EInputType", Branches)));
				return true;
				/*if (x > 10.0f)
				{
					Branches = EInputType::UpLeft;
					UE_LOG(LogTemp, Warning, TEXT("Up Left"));
					GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("%s"), *GETENUMSTRING("EInputType", Branches)));
					return true;
				}
				else if (x < -10.0f)
				{
					Branches = EInputType::UpRight;
					UE_LOG(LogTemp, Warning, TEXT("Up Right"));
					GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("%s"), *GETENUMSTRING("EInputType", Branches)));
					return true;
				}
				else
				{
					Branches = EInputType::Up;
					UE_LOG(LogTemp, Warning, TEXT("Up"));
					GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("%s"), *GETENUMSTRING("EInputType", Branches)));
					return true;
				}*/
				/*Branches = EInputType::Up;
				UE_LOG(LogTemp, Warning, TEXT("Up"));
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("%s"), *GETENUMSTRING("EInputType", Branches)));
				return true;*/
			}
			else if (y < -50.0f)
			{
				//Branches = EInputType::Down;
				//UE_LOG(LogTemp, Warning, TEXT("Down"));
				//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("%s"), *GETENUMSTRING("EInputType", Branches)));
				return false;

				/*if (x > 10.0f)
				{
					Branches = EInputType::DownLeft;
					UE_LOG(LogTemp, Warning, TEXT("Down Left"));
					GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("%s"), *GETENUMSTRING("EInputType", Branches)));
					return true;
				}
				else if (x < -10.0f)
				{
					Branches = EInputType::DownRight;
					UE_LOG(LogTemp, Warning, TEXT("Down Right"));
					GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("%s"), *GETENUMSTRING("EInputType", Branches)));
					return true;
				}
				else
				{
					Branches = EInputType::Down;
					UE_LOG(LogTemp, Warning, TEXT("Down"));
					GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("%s"), *GETENUMSTRING("EInputType", Branches)));
					return true;
				}*/
				/*Branches = EInputType::Down;
				UE_LOG(LogTemp, Warning, TEXT("Down"));
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("%s"), *GETENUMSTRING("EInputType", Branches)));
				return true;*/
			}
		}
	}
	return false;
}

void UGestureInputsFunctions::DrawJoyLine(const FVector2D & Start, const FVector2D & End, const FLinearColor & TheColor, const float & Thick)
{
	/*auto Canvas = UWidgetLayoutLibrary::SlotAsCanvasSlot("Button2");

	if (!Canvas) return;
	//
	FCanvasLineItem NewLine(Start, End);
	NewLine.SetColor(TheColor);
	NewLine.LineThickness = Thick;
	Canvas->DrawItem(NewLine);*/
}

float UGestureInputsFunctions::UpdateProgressBarComponent(UBaseCharacterWidget* WidgetToUse, const FString ProgressBarName, const FString TextBlockName, const FString AdditionalTextBlockName, FString StatusName, float Val, int MaxVal)
{
	const FName locTextControlName = FName(*TextBlockName);
	UTextBlock* locTextControl = (UTextBlock*)(WidgetToUse->WidgetTree->FindWidget(locTextControlName));

	const FName locTextControlPercent = FName(*AdditionalTextBlockName);
	UTextBlock* locTextControlPercentBlock = (UTextBlock*)(WidgetToUse->WidgetTree->FindWidget(locTextControlPercent));

	const FName locProgressBar = FName(*ProgressBarName);
	UProgressBar* ProgressBar = (UProgressBar*)(WidgetToUse->WidgetTree->FindWidget(locProgressBar));

	if (ProgressBar != nullptr)
	{
		return UGestureMathLibrary::SetProgressBarValue(StatusName, ProgressBar, locTextControl, locTextControlPercentBlock, Val, MaxVal);
	}
	return 0.0f;
}
