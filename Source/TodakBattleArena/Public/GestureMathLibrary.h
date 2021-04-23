#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "ReferenceSkeleton.h"
#include "GestureMathLibrary.generated.h"

class ATodakBattleArenaCharacter;

//Bodypart
UENUM(BlueprintType)
enum class EOperation : uint8
{
	Addition,
	Subtraction
};

UCLASS()
class UGestureMathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/**
	 * Calculates the angle between two lines in 2D space
	 *
	 * @param	Line1Start	- The start point of the first line
	 * @param	Line1End - The end point of the first line
	 * @param	Line2Start - The start point of the second line
	 * @param	Line2End - The end point of the second line
	 * @result	The angle between the two lines in Radians
	 */
	UFUNCTION(BlueprintPure, Category="Math|Gesture Utils")
	static float AngleBetweenLines(const FVector2D& Line1Start, const FVector2D& Line1End, const FVector2D& Line2Start, const FVector2D& Line2End);
	
	UFUNCTION(BlueprintPure, Category="Math|Gesture Utils")
	static bool PointOnLeftHalfOfScreen(FVector2D Point);
	
	UFUNCTION(BlueprintPure, Category="Math|Gesture Utils")
	static bool PointOnRighHalfOfScreen(FVector2D Point);
	
	UFUNCTION(BlueprintPure, Category="Math|Gesture Utils")
	static bool PointOnTopHalfOfScreen(FVector2D Point);
	
	UFUNCTION(BlueprintPure, Category="Math|Gesture Utils")
	static bool PointOnBottomHalfOfScreen(FVector2D Point);

		/** Gesture values typically are based on points, not pixels. On a Retina or other HighDPI screen, you typically need your tolerances to be considerably higher because moving your finger the same distance will result in a lot more pixels. This function will return the scaling factor for the current device's screen to normalize to a standard scale */
	UFUNCTION(BlueprintPure, Category="Math|Gesture Utils")
	static float GetScalingFactorForScreen();
	
	/** Converts a float value to account for the resolution of the screen (1.0 = ~72DPI) */
	static float ScaleFloatForScreen(float Value);
	
	/** Converts an FVector value to account for the resolution of the screen (1.0 = ~72DPI) */
	static FVector2D ScaleVector2DForScreen(FVector2D Value);

	//Get radius of the circle
	UFUNCTION(BlueprintPure, Category = "Math|Gesture Utils")
	static float RadiusCircle(float FirstVectorX, float SecondVectorX, float FirstVectorY, float SecondVectorY);

	//bottom-left and top-right 
	//corners of rectangle 
	UFUNCTION(BlueprintPure, Category = "Math|Gesture Utils")
	static bool IsInsideRect(float x1, float y1, float x2, float y2, float x, float y);

	//Checks if the point is inside a circle
	UFUNCTION(BlueprintPure, Category = "Math|Gesture Utils")
	static bool IsInsideCircle(float circle_x, float circle_y, float rad, float x, float y);

	//Checks if points is inside an ellipse
	UFUNCTION(BlueprintPure, Category = "Math|Gesture Utils")
	static float CheckEllipse(float CenterX, float CenterY, float xDist, float yDist, float xPoint, float yPoint, bool& isInside);

	UFUNCTION(BlueprintPure, Category = "Math|Gesture Utils")
	static void AbsoluteValueOfTwoVectors(FVector2D StartValue, FVector2D EndValue, float& x, float& y, float& AbsX, float& AbsY);

	//Checks for circles intersection
	UFUNCTION(BlueprintPure, Category = "Math|Gesture Utils")
	static void CircleIntersectCheckers(FVector2D FirstCircle, FVector2D SecondCircle, float RadiusOfFirstCircle, float RadiusOfSecondCircle, FVector2D& FirstPoint, FVector2D& SecondPoint);

	/** Peak by meaning is a vector between start and end vector **/
	static float AngleBetweenVectors(FVector2D Pivot, FVector2D Line1Start, FVector2D Line1End, FVector2D Peak);

	/*Converts a float Value into a string with a label*/
	UFUNCTION(BlueprintPure, Category = "Math|UI Utils")
	static FText PrintStatusPercentage(float Value, FString Label);

	/*Converts a float Value into a string with a label*/
	UFUNCTION(BlueprintPure, Category = "Math|UI Utils")
	static FText PrintStatusValue(float Value, int MaxVal, FString Label);

	/*Get Value changed and converts it to a percentage*/
	UFUNCTION(BlueprintPure, Category = "Math|UI Utils")
	static float SetPercentageValue(float InitialVal, float ChangeValueBy, float LimitValue, EOperation Operation, float PercentageLimit, bool CalculatePercentageOnly, float& FinalVal);

	/*UFUNCTION(BlueprintPure, meta = (DisplayName = "float + float", CompactNodeTitle = "+", Keywords = "+ add plus", CommutativeAssociativeBinaryOperator = "true"), Category = "Math|Utils")
	static float TotalPercents(float A, float B);*/

	/*Get total percentage value from each hundreds*/
	UFUNCTION(BlueprintPure, Category = "Math|UI Utils")
	static float TotalPercentage(float A, float B, float C, float& inHundredpercent, float PercentageLimit = 100.0f);

	/*Get remaining percentage from total percentage*/
	UFUNCTION(BlueprintPure, Category = "Math|UI Utils")
	static void TotalPercentageReduction(float A, float DecrementA, float MaxA, float MaxPercentage, float& FinalVal, float& PercentageRemains);

	/*Get value based on its percentages*/
	UFUNCTION(BlueprintPure, Category = "Math|UI Utils")
	static float CalculateValueFromPercentage(float Percentage, float MaxA, float MaxPercentage);

	/*Get percentage based on its value from the max value*/
	UFUNCTION(BlueprintPure, Category = "Math|UI Utils")
	static float CalculatePercentageFromValue(float Value, int Max, float MaxPercentage);

	//UFUNCTION(BlueprintPure, Category = "Bones")
	//static FName CheckForBoneName(ATodakBattleArenaCharacter* PlayerCharacter, FName BoneName);

	/*Assign values to progressbar*/
	UFUNCTION(BlueprintPure, Category = "Math|UI Utils")
	static float SetProgressBarValue(FString StatusName, UProgressBar* ProgressBar, UTextBlock* StatusTextPercentage, UTextBlock* TextValue, float value, int MaxValue);

	/*Transform*/
	static FTransform GetWorldSpaceTransform(FReferenceSkeleton RefSkel, int32 BoneIdx);

	UFUNCTION(BlueprintCallable, Category = "Transform")
	static FTransform GetRefPoseBoneTransform(USkeletalMeshComponent* SkelMesh, FName BoneName);

	UFUNCTION(BlueprintCallable, Category = "Transform")
	static FTransform GetBoneTransform(USkeletalMeshComponent* SkelMesh, FName BoneName);
	/***************************************/


	UFUNCTION(BlueprintCallable, Category = "DateAndTime")
	static float GetCurrentTimeInMinute();

	UFUNCTION(BlueprintCallable, Category = "DateAndTime")
	static FTimespan GetCurrentTime();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|UI Utils")
	static void CalculateProgressBarRate(float PercentageFromMax, int MaxValue, float TimeInSeconds, int CappedValueIncrement, int MaxStatusValue, float& RateFromCappedVal, int& TotalIncrementValue, float& TotalPercentageValue);

	UFUNCTION(BlueprintCallable, Category = "DateAndTime")
	static FDateTime GetCurrentDateAndTime();

	UFUNCTION(BlueprintPure, Category = "Rate")
	static float GetCurrentPercentInSeconds(float CurrVal, float IncrementPerSeconds, float& NewVal, int MaxCappedVal = 1000);

	UFUNCTION(BlueprintPure, Category = "Rate")
	static float GetRatePerSeconds(float CurrVal, float ConstantValue, float BoostVal);

	UFUNCTION(BlueprintPure, Category = "Rate")
	static float CalculateTotalMaxVal(float Percentage, float MaxVal, float& ValFromPercentage);

	//Rotations
	UFUNCTION(BlueprintPure, Category = "Rotation")
	static bool IsLooking(FVector Start, FVector Target, float ZVal);

	UFUNCTION(BlueprintPure, Category = "Rotation")
	static float GetAngleOffsetFromForwardVector(AActor* A, AActor* B);

	UFUNCTION(BlueprintPure, Category = "Rotation")
	static bool IsRightAngle(FVector SourceLoc, FVector HitLoc);
};