// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine.h"
#include "GestureInputsFunctions.generated.h"

class ATodakBattleArenaCharacter;
class UWidgetTree;
class UTextBlock;
class UBaseCharacterWidget;

//For logging purposes
#define GETENUMSTRING(etype, evalue) ( (FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true) != nullptr) ? FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true)->GetNameStringByIndex((int32)evalue) : FString("Invalid - are you sure enum uses UENUM() macro?") )

/**
 * 
 */

 //Input Touch Type
UENUM(BlueprintType)
enum class EInputType : uint8
{
	Pressed,
	Released,
	Up,
	Down,
	Left, 
	Right,
	UpLeft,
	UpRight,
	DownLeft,
	DownRight,
	UpwardLeftCurve,
	UpwardRightCurve,
	DownwardLeftCurve,
	DownwardRightCurve, 
	Tap,
	Hold
};

//Bodypart
UENUM(BlueprintType)
enum class EBodyPart : uint8
{
	LeftHand,
	RightHand,
	LeftFoot,
	RightFoot
};

UENUM(BlueprintType)
enum class EInputStyle : uint8
{
	Default,//The center swipe
	LeftJoystick,//The Left analog stick with right area swipe
	Button //Button
};

//PlayerStats
UENUM(BlueprintType)
enum class EMainPlayerStats : uint8
{
	PainMeter,
	Energy
};

//UENUM(BlueprintType)
//enum class EMontageSection : uint8
//{
//	Attack1,
//	Attack2,
//	Attack3
//};

UENUM(BlueprintType)
enum class EBarType : uint8
{
	PrimaryProgressBar,
	SecondaryProgressBar
};

USTRUCT(BlueprintType)
struct FFingerIndex : public FTableRowBase
{
	GENERATED_BODY()

	//Touch index
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	TEnumAsByte<ETouchIndex::Type> FingerIndex;

	//What swipe actions occured
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	EInputType SwipeActions;

	////Which body parts the swipe occurs
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	//EBodyPart BodyParts;

	//Start touch location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	FVector2D StartLocation;

	//Does touch swipes original start location from small circle?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	bool FromSmallCircle = false;

	//If current touch index is active
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	bool IsPressed = false;

	////--vector points for curved swipes--//
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DoOnce")
	//TArray<FVector2D> LeftPoints;

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DoOnce")
	//TArray<FVector2D> RightPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PointArray")
	TArray<FVector2D> Points;
	//---------------------------------//

	//Is swipe action is complete
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoOnce")
	bool bDo = false;

	bool operator ==(const FFingerIndex &other) const
	{
		if ((FingerIndex == other.FingerIndex))
		{
			//UE_LOG(LogTemp, Warning, TEXT("Key %s is %s and %s type is %s and DetectInputOnPressed is %s"), *KeyInput.ToString(), (KeyInput == other.KeyInput)? TEXT("True"): TEXT("False"), *GETENUMSTRING("EInputType", InputType), (InputType == other.InputType) ? TEXT("True") : TEXT("False"), (DetectInputOnPressed == other.DetectInputOnPressed) ? TEXT("True") : TEXT("False"));
			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("Key input is %s and Input type is %s and DetectInputOnPressed is %s"), (KeyInput == other.KeyInput) ? TEXT("True") : TEXT("False"), (InputType == other.InputType) ? TEXT("True") : TEXT("False"), (DetectInputOnPressed == other.DetectInputOnPressed) ? TEXT("True") : TEXT("False")));

			return true;
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("Key %s is %s and %s type is %s"), *KeyInput.ToString(), (KeyInput == other.KeyInput) ? TEXT("True") : TEXT("False"), *GETENUMSTRING("EInputType", InputType), (InputType == other.InputType) ? TEXT("True") : TEXT("False"));
			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("Key input is %s and Input type is %s"), (KeyInput == other.KeyInput) ? TEXT("True") : TEXT("False"), (InputType == other.InputType) ? TEXT("True") : TEXT("False")));
			return false;
		}
	}

};

USTRUCT(BlueprintType)
struct FBodyDamage : public FTableRowBase
{
	GENERATED_BODY()

	//Major damage dealt
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float MajorDamageReceived = 0.0f;

	//if the bodypart is upperbody or lower body
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		bool IsUpperBody = false;

	//Anim to be played on hit damage by body part
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		UAnimMontage* DamageBlockMoveset;

	//What time does the swipe anim will start
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
		float StartMontage;

	//Playrate swipe anim
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
		float DamageMovesetRate;

	//Time to stop current hold anim
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
		float StopHoldAnimTime;
};

USTRUCT(BlueprintType)
struct FBlockActions : public FTableRowBase
{
	GENERATED_BODY()

	//check if face block is active
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block")
		bool IsFaceBlock = false;

	//Major damage dealt
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block")
		bool HoldBlock = false;

	//check if face block is active
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block")
		bool IsRightBlock = false;

	//What time does the block anim will start playing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
		float StartAnimTime = 0.0f;

	//What time does the block anim will pause of holdblock is true
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
		float PauseAnimTime = 0.0f;

	//Anim to be played on block damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
		UAnimMontage* BlockMoveset;

	//Duration of montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
		float BlockMovesetLength;
};

USTRUCT(BlueprintType)
struct FActionSkill : public FTableRowBase
{
	GENERATED_BODY()

	//Does skill is in cooldown?
	bool CDSkill = false;

	//If skill is triggered
	bool SkillTrigger = false;

	//Which swipe action will execute the anim
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	EInputType SwipeActions;
	/*TArray<EInputType> SwipeActions;*/

	////Which body part will execute the anim
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	//TArray<EBodyPart> BodyParts;

	//Anim to be played on swipe when switch right
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillSwipe")
	UAnimMontage* SkillMoveset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitReaction")
	UAnimMontage* HitReactionMoveset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlockReaction")
	FBlockActions BlockReactionMoveset;

	//What time does the swipe anim will start
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SwipeAnimTime")
	float SkillMovesetTime;

	//Playrate swipe anim
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	float SkillMoveSetRate;

	//Playrate swipe anim when energy less than 0
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	float FatigueMovesetRate;

	//Damage to be dealt from the action
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float Damage = 0.0f;

	//Damage received when attacker is fatigue
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float FatigueDamage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	float MontageDelay = 0.0f;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float BlockedDamage = 0.0f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float FatigueBlockedDamage = 0.0f;*/

	//Damage to be dealt from the action
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaUsage = 0.0f;

	//Damage to be dealt from the action
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaDrain = 0.0f;

	

	//For array comparison
	bool operator ==(const FActionSkill &other) const
	{
		if ((SwipeActions == other.SwipeActions)/* && (BodyParts == other.BodyParts)*/)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Key %s is %s and %s type is %s and DetectInputOnPressed is %s"), *KeyInput.ToString(), (KeyInput == other.KeyInput)? TEXT("True"): TEXT("False"), *GETENUMSTRING("EInputType", InputType), (InputType == other.InputType) ? TEXT("True") : TEXT("False"), (DetectInputOnPressed == other.DetectInputOnPressed) ? TEXT("True") : TEXT("False"));
			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("Key input is %s and Input type is %s and DetectInputOnPressed is %s"), (KeyInput == other.KeyInput) ? TEXT("True") : TEXT("False"), (InputType == other.InputType) ? TEXT("True") : TEXT("False"), (DetectInputOnPressed == other.DetectInputOnPressed) ? TEXT("True") : TEXT("False")));

			return true;
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("Key %s is %s and %s type is %s"), *KeyInput.ToString(), (KeyInput == other.KeyInput) ? TEXT("True") : TEXT("False"), *GETENUMSTRING("EInputType", InputType), (InputType == other.InputType) ? TEXT("True") : TEXT("False"));
			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("Key input is %s and Input type is %s"), (KeyInput == other.KeyInput) ? TEXT("True") : TEXT("False"), (InputType == other.InputType) ? TEXT("True") : TEXT("False")));
			return false;
		}
	}
};

USTRUCT(BlueprintType)
struct FPlayerStatus : public FTableRowBase
{
	GENERATED_BODY()

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
		float Stamina = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
		float Strength = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
		float Agility = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
		float Fitness = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
		int MaxStamina = 700;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
		int MaxStrength = 700;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
		int MaxAgility = 700;

	UPROPERTY(BlueprintReadWrite, Category = "Status")
		float StaminaPercentage;

	UPROPERTY(BlueprintReadWrite, Category = "Status")
		float StrengthPercentage;

	UPROPERTY(BlueprintReadWrite, Category = "Status")
		float AgilityPercentage;

	UPROPERTY(BlueprintReadWrite, Category = "Status")
		float Vitality = 0.0f;

	//The amount of energy the character currently has
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
		float playerEnergy;

	//The amount of energy the character currently has
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
		float MaxEnergy;

	//The amount of energy percentage the character currently has
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
		float EnergyPercentage;

	//The amount of fatigue resistance the character currently has
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
		float PlayerFatigue;

	//The amount of fatigue resistance percentage the character currently has
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
		float FatiguePercentage;

	//The amount of max fatigue resistance the character currently has
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
		float MaxFatigue;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float Magic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy")
		float DelaySpeed = 1.5f;

	//The amount of health the character currently has
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
		float playerHealth;

	//The amount of health the character currently has
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
		float MaxHealth;
};


//Compare arrays while ignoring the order
template<typename T>
FORCEINLINE bool areEqual(TArray<T> arr1, TArray<T> arr2, int n, int m)
{
	// If lengths of array are not equal means 
	// array are not equal 
	if (n != m)
		return false;

	arr1.Sort();
	arr2.Sort();

	for (int i = 0; i < n; i++)
		if (arr1[i] != arr2[i])
			return false;

	return true;
};

UCLASS()
class TODAKBATTLEARENA_API UGestureInputsFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	//UFUNCTION(BlueprintCallable, Category = "GestureInput")
	static void RightSwipeArea(ATodakBattleArenaCharacter* PlayerCharacter, FFingerIndex* FingerIndex, FVector2D Line1End);

	//Detect swipe areas
	static void CircleSwipeArea(ATodakBattleArenaCharacter* PlayerCharacter, struct FFingerIndex* FingerIndex, FVector2D Line1End);

	//Detect curve swipe
	static bool DetectCurveSwipe(TArray<FVector2D> Points, FVector2D Line1Start, FVector2D Line1End, FVector2D Peak, bool Dos, EInputType& Branches);

	//Detect linear swipe
	static bool DetectLinearSwipe(FVector2D Line1Start, FVector2D Line1End, EInputType& Branches, bool Dos, TArray<FVector2D> Points);

	//DrawLine
	static void DrawJoyLine(const FVector2D& Start, const FVector2D& End, const FLinearColor& TheColor, const float& Thick);

	//Update progress bar value
	static float UpdateProgressBarComponent(UBaseCharacterWidget* WidgetToUse, const FString ProgressBarName, const FString TextBlockName, const FString AdditionalTextBlockName, FString StatusName, float Val, int MaxVal);
};