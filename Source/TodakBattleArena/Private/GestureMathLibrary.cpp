// 2014 MartianCraft, LLC, See eader file for license & copyright notice
//2016 juaxix
#include "GestureMathLibrary.h"
#include "EngineGlobals.h"
#include "Engine.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Math/UnrealMathUtility.h"
#include <Engine/UserInterfaceSettings.h>
#include "TodakBattleArenaCharacter.h"
#include "ReferenceSkeleton.h"
#include "Containers/Array.h"

float UGestureMathLibrary::AngleBetweenLines(const FVector2D& Line1Start, const FVector2D& Line1End, const FVector2D& Line2Start, const FVector2D& Line2End)
{
	float A = Line1End.X - Line1Start.X;
	float B = Line1End.Y - Line1Start.Y;
	float C = Line2End.X - Line2Start.X;
	float D = Line2End.Y - Line2Start.Y;
	
	float Line1Slope = ((Line1End.X - Line1Start.X) == 0.f) ? 0.f : (Line1End.Y - Line1Start.Y) / (Line1End.X - Line1Start.X);
	float Line2Slope = ((Line2End.X - Line2Start.X) == 0.f) ? 0.f : (Line2End.Y - Line2Start.Y) / (Line2End.X - Line2Start.X);
	
	float Angle = acosf(((A * C) + (B * D)) / ((sqrtf(A * A + B * B)) * (sqrtf(C * C + D * D))));
	
	if (Angle == 0.f) return 0.f;
	
	if (isnan(Angle) || isnan(-Angle))
		return 0.f;
	
	//ganim
	return (Line2Slope > Line1Slope) ? Angle : -Angle;
	
}
bool UGestureMathLibrary::PointOnLeftHalfOfScreen(FVector2D Point)
{
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	const FVector2D ViewportCenter =  FVector2D(ViewportSize.X/2, ViewportSize.Y/2);

	return (Point.X <= ViewportCenter.X);
}

bool UGestureMathLibrary::PointOnRighHalfOfScreen(FVector2D Point)
{
	return !PointOnLeftHalfOfScreen(Point);
}

bool UGestureMathLibrary::PointOnTopHalfOfScreen(FVector2D Point)
{
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	const FVector2D ViewportCenter =  FVector2D(ViewportSize.X/2, ViewportSize.Y/2);
	
	return (Point.Y <= ViewportCenter.Y);
}

bool UGestureMathLibrary::PointOnBottomHalfOfScreen(FVector2D Point)
{
	return !(PointOnTopHalfOfScreen(Point));
}

float UGestureMathLibrary::GetScalingFactorForScreen()
{
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	const float ViewportScale = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(ViewportSize.X, ViewportSize.Y));
	
	return ViewportScale;
}

float UGestureMathLibrary::ScaleFloatForScreen(float Value)
{
	return Value * GetScalingFactorForScreen();
}

FVector2D UGestureMathLibrary::ScaleVector2DForScreen(FVector2D Value)
{
	return Value * GetScalingFactorForScreen();
}

float UGestureMathLibrary::RadiusCircle(float FirstVectorX, float SecondVectorX, float FirstVectorY, float SecondVectorY)
{
	return sqrt((pow((FirstVectorX - (SecondVectorX)), 2)) + (pow((FirstVectorY - (SecondVectorY)), 2)));
}

bool UGestureMathLibrary::IsInsideRect(float x1, float y1, float x2, float y2, float x, float y)
{
	//function to find if given point lies inside a given rectangle or not. 
	if (x > x1 && x < x2 && y > y1 && y < y2)
	{
		UE_LOG(LogTemp, Warning, TEXT("Is Inside"));
		return true;
	}
		
	return false;
}

bool UGestureMathLibrary::IsInsideCircle(float circle_x, float circle_y, float rad, float x, float y)
{
	// Compare radius of circle with distance  
	// of its center from given point 
	if ((x - circle_x) * (x - circle_x) +
		(y - circle_y) * (y - circle_y) <= rad * rad)
		return true;
	else
		return false;
}

float UGestureMathLibrary::CheckEllipse(float CenterX, float CenterY, float xDist, float yDist, float xPoint, float yPoint, bool& isInside)
{
	// checking the equation of 
	// ellipse with the given point
	float p = (pow((xPoint - CenterX), 2) / pow(xDist, 2)) + (pow((yPoint - CenterY), 2) / pow(yDist, 2));

	if (p > 1)
	{
		isInside = false;
		UE_LOG(LogTemp, Warning, TEXT("Outside The Ellipse : %f"), p);
	}
	else if (p == 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Is On The Ellipse : %f"), p);
	}
	else
	{
		isInside = true;
		UE_LOG(LogTemp, Warning, TEXT("Inside The Ellipse : %f"), p);
	}
		
	return p;
}

void UGestureMathLibrary::AbsoluteValueOfTwoVectors(FVector2D StartValue, FVector2D EndValue, float& x, float& y, float& AbsX, float& AbsY)
{
	FVector2D Total = StartValue - EndValue;

	x = Total.X;
	y = Total.Y;

	AbsX = FGenericPlatformMath::Abs(Total.X);
	AbsY = FGenericPlatformMath::Abs(Total.Y);
}

//Check for intersections
void UGestureMathLibrary::CircleIntersectCheckers(FVector2D FirstCircle, FVector2D SecondCircle, float RadiusOfFirstCircle, float RadiusOfSecondCircle, FVector2D& FirstPoint, FVector2D& SecondPoint)
{
	float d = hypot(SecondCircle.X - FirstCircle.X, SecondCircle.Y - FirstCircle.Y);

	if (d <= RadiusOfFirstCircle + RadiusOfSecondCircle && d >= FGenericPlatformMath::Abs(RadiusOfSecondCircle - RadiusOfFirstCircle))
	{
		float ex = (SecondCircle.X - FirstCircle.X) / d;
		float ey = (SecondCircle.Y - FirstCircle.Y) / d;

		float x = (RadiusOfFirstCircle* RadiusOfFirstCircle - RadiusOfSecondCircle * RadiusOfSecondCircle + d * d) / (2 * d);
		float y = sqrt(RadiusOfFirstCircle* RadiusOfFirstCircle - x * x);

		FirstPoint = FVector2D(FirstCircle.X + x * ex - y * ey, FirstCircle.Y + x * ey + y * ex);
		SecondPoint = FVector2D(FirstCircle.X + x * ey + y * ey, FirstCircle.Y + x * ey - y * ex);
	}
}

float UGestureMathLibrary::AngleBetweenVectors(FVector2D Pivot, FVector2D Line1Start, FVector2D Line1End, FVector2D Peak)
{
	FVector2D ForwardDirection = Line1Start - Pivot;
	ForwardDirection.Normalize();

	FVector2D ForwardDirections = Line1End - Line1Start;
	ForwardDirections.Normalize();

	float Dotproduct = FVector2D::DotProduct(ForwardDirection, ForwardDirections);

	float Angle = FGenericPlatformMath::Acos(Dotproduct);

	//UE_LOG(LogTemp, Warning, TEXT("ArcLength %f;"), ((2 * PI)* UGestureMathLibrary::RadiusCircle(Line1Start.X, Pivot.X, Line1Start.Y, Pivot.Y)*(FMath::RadiansToDegrees(FMath::Clamp(Angle, -PI, PI)) / 360)));
	//UE_LOG(LogTemp, Warning, TEXT("VectorLength %f;"), UGestureMathLibrary::RadiusCircle(Line1End.X, Line1Start.X, Line1End.Y, Line1Start.Y));

	return FMath::RadiansToDegrees(FMath::Clamp(Angle, -2 * PI, 2 * PI));
}

FText UGestureMathLibrary::PrintStatusPercentage(float Value, FString Label)
{
	FText Val = FText::AsPercent(Value);

	FString NewString = Label + Val.ToString();

	return FText::AsCultureInvariant(NewString);
}

FText UGestureMathLibrary::PrintStatusValue(float Value, int MaxVal, FString Label)
{
	int NewVal = FMath::RoundToInt(Value);
	FString NewString = Label + FString::FromInt(NewVal) + "/" + FString::FromInt(MaxVal);
	return FText::AsCultureInvariant(NewString);
}

float UGestureMathLibrary::SetPercentageValue(float InitialVal, float ChangeValueBy, float LimitValue, EOperation Operation, float PercentageLimit, bool CalculatePercentageOnly, float& FinalVal)
{
	if (CalculatePercentageOnly == false)
	{
		if (Operation == EOperation::Addition)
		{
			float NewVal = InitialVal + ChangeValueBy;
			float totalVal = NewVal / LimitValue;
			if (NewVal <= LimitValue)
			{
				FinalVal = NewVal;
				return totalVal;
			}
			else
			{
				FinalVal = LimitValue;
				return PercentageLimit;
			}
		}
		else if (Operation == EOperation::Subtraction)
		{
			float NewVal = InitialVal - ChangeValueBy;
			float totalVal = NewVal / LimitValue;
			if (NewVal >= 0)
			{
				FinalVal = NewVal;
				return totalVal;
			}
			else
			{
				FinalVal = 0.0f;
				return 0.0f;
			}
		}
	}

	else if (CalculatePercentageOnly == true)
	{
		if (Operation == EOperation::Addition)
		{
			float totalVal = ChangeValueBy / LimitValue;
			if (totalVal <= PercentageLimit)
			{
				float NewVal = InitialVal + totalVal;
				FinalVal = totalVal;
				return totalVal;
			}
			else
			{
				FinalVal = LimitValue;
				return PercentageLimit;
			}
		}
		else if (Operation == EOperation::Subtraction)
		{
			float totalVal = ChangeValueBy / LimitValue;
			if (totalVal >= 0)
			{
				float NewVal = InitialVal + totalVal;
				FinalVal = totalVal;
				return totalVal;
			}
			else
			{
				FinalVal = 0.0f;
				return 0.0f;
			}
		}
	}
	
	return 0.0f;
}

/*float UGestureMathLibrary::TotalPercents(float A, float B)
{
	float newPercentage = 300.0f / 100.0f;
	A  = A / newPercentage;
	B  = B / newPercentage;

	return A + B;
}*/

float UGestureMathLibrary::TotalPercentage(float A, float B, float C, float& inHundredpercent, float PercentageLimit)
{
	float TotalPercent = PercentageLimit / 100.0f;

	float totalA = A / TotalPercent;
	float totalB = B / TotalPercent;
	float totalC = C / TotalPercent;

	inHundredpercent = totalA + totalB + totalC;
	return inHundredpercent*TotalPercent;
}

void UGestureMathLibrary::TotalPercentageReduction(float A, float DecrementA, float MaxA, float MaxPercentage, float& FinalVal, float& PercentageRemains)
{
	//reduction by percentage
	float TotalPercentageReduction = (DecrementA / MaxPercentage) * MaxA;

	float FinalVals = A - TotalPercentageReduction;

	if (FinalVals >= 0)
	{
		//percentage from limit remains
		PercentageRemains = (FinalVals / MaxA) * MaxPercentage;
		FinalVal = FinalVals;
	}
	else
	{
		//percentage from limit remains
		PercentageRemains = 0.0f;
		FinalVal = 0.0f;
	}
}

float UGestureMathLibrary::CalculateValueFromPercentage(float Percentage, float MaxA, float MaxPercentage)
{
	return (Percentage / MaxPercentage) * MaxA;
}

float UGestureMathLibrary::CalculatePercentageFromValue(float Value, int Max, float MaxPercentage)
{
	return (Value / Max) * MaxPercentage;
}

/*FName UGestureMathLibrary::CheckForBoneName(ATodakBattleArenaCharacter* PlayerCharacter, FName BoneName)
{
	TArray<FString> BoneNames = { "Hand", "Arm", "Neck", "Head", "Spine", "Clavicle", "Thigh", "Foot", "Calf", "Ball" };

	bool Found = false;

	FString NewString;

	ATodakBattleArenaCharacter* PlayerChar = Cast<ATodakBattleArenaCharacter>(PlayerCharacter);
	if (PlayerChar)
	{
		for (FString Names : BoneNames)
		{
			if (BoneName.ToString().Contains((Names)))
			{
				Found = true;
				NewString = Names;
				break;
			}
		}
		if (Found)
		{
			int val = BoneNames.Find(NewString);
			if (val <= 1)
			{
				UE_LOG(LogTemp, Warning, TEXT("Hand"));
				PlayerChar->HandBone.AddUnique(FName(*NewString));
			}
			else if (val > 1 && val <= 3)
			{
				UE_LOG(LogTemp, Warning, TEXT("Head"));
				PlayerChar->NeckBone.AddUnique(FName(*NewString));
			}
			else if (val > 3 && val <= 5)
			{
				UE_LOG(LogTemp, Warning, TEXT("Chest"));
				PlayerChar->ChestBone.AddUnique(FName(*NewString));
			}
			else if (val > 5)
			{
				UE_LOG(LogTemp, Warning, TEXT("Feet"));
				PlayerChar->LegBone.AddUnique(FName(*NewString));
			}
		}
	}
	
	return FName(*NewString);
}*/

float UGestureMathLibrary::SetProgressBarValue(FString StatusName, UProgressBar* ProgressBar, UTextBlock* StatusTextPercentage, UTextBlock* TextValue, float value, int MaxValue)
{
	//float tempvalue = 0.0f;
	//UE_LOG(LogTemp, Warning, TEXT("Hand"));
	//float tempvalue = 0.0f;
	//UE_LOG(LogTemp, Warning, TEXT("Hand"));

	if (ProgressBar != NULL)
	{
		if (ProgressBar->IsValidLowLevel())
		{
			float inHundredPercent;

			//tempvalue = CalculatePercentageFromValue(value, MaxValue, 100.0f) / 100.0f;
			float ReturnVal = TotalPercentage((CalculatePercentageFromValue(value, MaxValue, 100.0f) / 100.0f), 0.0f, 0.0f, inHundredPercent);

			//Set progressbar percentage
			ProgressBar->SetPercent(ReturnVal);
			if (StatusTextPercentage != NULL)
			{
				if (StatusTextPercentage->IsValidLowLevel())
				{
					StatusTextPercentage->SetText(PrintStatusPercentage(inHundredPercent, StatusName + ": "));
				}
			}
		}
		if (TextValue != NULL)
		{
			if (TextValue->IsValidLowLevel())
			{
				TextValue->SetText(PrintStatusValue(value, MaxValue, StatusName + ": "));
			}
		}
	}
	return CalculatePercentageFromValue(value, MaxValue, 100.0f) / 100.0f;
}

//Recursive function to have results relative to world origin
FTransform UGestureMathLibrary::GetWorldSpaceTransform(FReferenceSkeleton RefSkel, int32 BoneIdx)
{
	FTransform BoneTransform;

	if (BoneIdx > 0)
	{
		BoneTransform = RefSkel.GetRefBonePose()[BoneIdx];

		FMeshBoneInfo BoneInfo = RefSkel.GetRefBoneInfo()[BoneIdx];
		if (BoneInfo.ParentIndex != 0)
		{
			BoneTransform *= GetWorldSpaceTransform(RefSkel, BoneInfo.ParentIndex);
			UE_LOG(LogTemp, Warning, TEXT("if parent index is not equal to zero"));
		}
	}

	return BoneTransform;
}

//Reference pose bone transform
FTransform UGestureMathLibrary::GetRefPoseBoneTransform(USkeletalMeshComponent* SkelMesh, FName BoneName)
{
	FTransform BoneTransform;

	if (SkelMesh && !BoneName.IsNone())
	{
		SkelMesh->ClearRefPoseOverride();
		FReferenceSkeleton RefSkel;
		RefSkel = SkelMesh->SkeletalMesh->RefSkeleton;
		BoneTransform = GetWorldSpaceTransform(RefSkel, RefSkel.FindBoneIndex(BoneName));
		//BoneTransform = RefSkel.GetRefBonePose()[RefSkel.FindBoneIndex(BoneName)];
	}

	return BoneTransform;
}

//Bone transform
FTransform UGestureMathLibrary::GetBoneTransform(USkeletalMeshComponent* SkelMesh, FName BoneName)
{
	FTransform BoneTransform;

	if (SkelMesh && !BoneName.IsNone())
	{
		FReferenceSkeleton RefSkel;
		RefSkel = SkelMesh->SkeletalMesh->RefSkeleton;
		BoneTransform = SkelMesh->GetBoneTransform(RefSkel.FindBoneIndex(BoneName));
	}

	return BoneTransform;
}

float UGestureMathLibrary::GetCurrentTimeInMinute()
{
	return GetCurrentTime().GetTotalMinutes();
}

FTimespan UGestureMathLibrary::GetCurrentTime()
{
	return GetCurrentDateAndTime().GetTimeOfDay();
}

void UGestureMathLibrary::CalculateProgressBarRate(float PercentageFromMax, int MaxValue, float TimeInSeconds, int CappedValueIncrement, int MaxStatusValue, float & RateFromCappedVal, int & TotalIncrementValue, float & TotalPercentageValue)
{
	float PercentVal = CalculateValueFromPercentage(PercentageFromMax, MaxValue, 100.0f);

	//Increment Rate over time
	RateFromCappedVal = (CalculatePercentageFromValue(PercentVal, CappedValueIncrement, 100.0f) / 100.0f) / TimeInSeconds;

	//total values
	float TotalVal = PercentVal + MaxStatusValue;

	TotalIncrementValue = TotalVal;

	//total percentage
	TotalPercentageValue = CalculatePercentageFromValue(TotalVal, CappedValueIncrement, 100.0f) / 100.0f;
}

FDateTime UGestureMathLibrary::GetCurrentDateAndTime()
{
	return FDateTime::Now();
}

float UGestureMathLibrary::GetCurrentPercentInSeconds(float CurrVal, float IncrementPerSeconds, float& NewVal, int MaxCappedVal)
{
	//new value
	NewVal = CurrVal + IncrementPerSeconds;
	
	//new percentage from new value
	return (CalculatePercentageFromValue(NewVal, MaxCappedVal, 100.0f) / 100.0f);
}

float UGestureMathLibrary::GetRatePerSeconds(float CurrVal, float ConstantValue, float BoostVal)
{
	//rate of increment per seconds
	return (((1 / CurrVal)*ConstantValue)) + (((1 / CurrVal)*ConstantValue)*(BoostVal/100.0f));
}

float UGestureMathLibrary::CalculateTotalMaxVal(float Percentage, float MaxVal, float& ValFromPercentage)
{
	//Get current value from percentage
	ValFromPercentage = CalculateValueFromPercentage(Percentage, MaxVal, 100.0f);
	return (MaxVal+ValFromPercentage);
}

bool UGestureMathLibrary::IsLooking(FVector Start, FVector Target, float ZVal)
{
	//Get look at rotation value
	FRotator RotatorVal = UKismetMathLibrary::FindLookAtRotation(Start, Target);

	bool InView = UKismetMathLibrary::InRange_FloatFloat(ZVal, RotatorVal.Yaw + (-90.0), RotatorVal.Yaw + 90.0);

	//If object is facing the target
	if (InView)
	{
		return true;
	}
	return false;
}
