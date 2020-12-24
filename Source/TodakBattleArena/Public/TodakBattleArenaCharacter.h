// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "..\Public\GestureInputsFunctions.h"
#include "Components/TimelineComponent.h"
#include "Components/BoxComponent.h"
#include "TargetLockInterface.h"
#include "Components/PrimitiveComponent.h"
#include "TodakBattleArenaCharacter.generated.h"

class UBaseCharacterWidget;
struct FKAggregateGeom;
class UPhysicsAsset;

FORCEINLINE uint32 GetTypeHash(const FFingerIndex& Key)
{
	return HashCombine(GetTypeHash(Key.FingerIndex), GetTypeHash(Key.StartLocation));
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGestureExecuted);

UCLASS(config=Game)
class TODAKBATTLEARENA_API ATodakBattleArenaCharacter : public ACharacter, public ITargetLockInterface
{
	GENERATED_BODY()
	//class UTimelineComponent* RagdollTimeline;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/**TargetLockCollision**/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* LockOnCollision;

	/**TargetLockCollision**/
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* ToBeIgnoredCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* LeftKickCol;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* RightKickCol;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* LKickArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* RKickArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* LeftPunchCol;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* RightPunchCol;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* LPunchArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* RPunchArrow;

	UPROPERTY(VisibleAnywhere, Category = "Timeline")
	class UTimelineComponent* MyTimeline;

	void StartAttack1();
	void StartAttack2();
	void StartAttack3();
	void StartAttack4();

public:
	ATodakBattleArenaCharacter();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	//Replicated Network setup
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//Interface
	virtual void LockOn_Implementation() override; //actual implementation of LockOn method

	//lock target player enemy
	void TriggerToggleLockOn();
	AController* SetNewControlRotation(FRotator& RotatorParam);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Output")
	void UpdateFitnessAfterAction();

	////////////////////////Called event for bp///////////////////////

	UFUNCTION(BlueprintImplementableEvent, Category = "Damage")
	void UpdateFatiguefterAction();

	UFUNCTION(BlueprintImplementableEvent, Category = "Damage")
	void UpdateEnergyfterAction(APlayerController* thisController);

	UFUNCTION(BlueprintImplementableEvent, Category = "Skill")
	void UpdateTutorialAction(const FString& Action);

	UFUNCTION(BlueprintImplementableEvent, Category = "Skill")
	void CompleteTutorialAction();

	UFUNCTION(BlueprintImplementableEvent, Category = "GameOver")
	void GameOverFunc();

	//UFUNCTION(BlueprintCallable, Category = "Collision")
	//void CheckLineTrace(AActor*& HitActor, FName& BoneNames, FVector& Location, bool& bBlockingHits);

	//Check Trace Upon Collision
	UFUNCTION(BlueprintCallable, Category = "Collision")
	void CheckHitTrace(AActor*& HitActor, FName& BoneNames, FVector& Location, bool& bBlockingHit);

	///////////////////////////////////////////////////////////////
	/////////////////////////Ragdoll on hit reaction///////////////////////////

	UPROPERTY(EditAnywhere, Category = "Timeline")
	class UCurveFloat* fCurve;

	UPROPERTY(VisibleAnywhere, Category = "Timeline")
	class UTimelineComponent* bwTimeline;

	UPROPERTY(VisibleAnywhere, Replicated)
	bool DoneRagdoll = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
	bool IsFacingUp = false;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, Category = "Ragdoll")
		FVector CapsuleLocation;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, Category = "Ragdoll")
		FVector MeshLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		FVector TestMeshLoc;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, Category = "Ragdoll")
		FRotator MeshRotation;

	//FTimeline RagdollTimeLine = FTimeline();
	//UCurveFloat* RagdollCurve;
	//void TimeLineFloat(float Value);

	//UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	//float Montage_Play(UAnimMontage * MontageToPlay, float InPlayRate, EMontagePlayReturnType ReturnValueType, float InTimeToStartMontageAt, bool bStopAllMontages);

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* RPCServerGetUp;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* GetUpAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* UpMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* DownMontage;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* RPCMulticastGetUp;

	/*UFUNCTION()
	virtual void AddImpulse(FVector Impulse, FName BoneName, bool bVelChange);*/

	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable)
	void SvrOnHitRagdoll();

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void MulticastOnHitRagdoll();

	UFUNCTION()
	void TimelineFloatReturn(float value);

	UFUNCTION()
	void OnTimelineFinished();

	FOnTimelineFloat InterpFunction{};

	FOnTimelineEvent TimelineFinished{};


	/////////////////////////End of hit reaction///////////////////////////

	/** Delegate function called for execute blueprints function */
	UPROPERTY(BlueprintAssignable, Category = Gestures)
	FGestureExecuted NewGestureDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SwipeGesture")
	bool FromSmallCircle = false;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, Category = "SwipeGesture")
	bool isLocked = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SwipeGesture")
	bool LeftFoot = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SwipeGesture")
	bool RightFoot = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SwipeGesture")
	bool EnableMovement = false;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, Category = "SwipeGesture")
	bool BlockedHit = false;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, Category ="SwipeGesture")
	bool RepTurnRight = false;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, Category = "SwipeGesture")
	bool RepTurnLeft = false;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, Category = "SwipeGesture")
	bool RepIsMoving = false;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "SwipeGesture")
	float RepLocoPlayrate = 1.0f;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "DoOnce")
	bool bDo = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetLockOn")
	bool isCollisionInScript = false;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "BlockedHit")
	bool AICanAttack = false;


	FTimerHandle IterateArray;

	/////////TargetLockTimer/////////////
	FTimerHandle DistanceEnemTimer;
	FTimerHandle ToggleTimer;

	UPROPERTY(VisibleAnywhere, Replicated, Category = "SwipeGesture")
	FTimerHandle BlockHitTimer;

	////////////////Swipe pattern enums////////////////////////////////
	TArray<FName> RowNames;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DoOnce")
	TArray<EInputType> SwipeActions;

	//Circle area enums
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DoOnce")
	TArray<EBodyPart> BodyParts;
	//////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable)
	APlayerController* GetPlayerControllers();

	// Reduce damage taken when in defensive mode
	UFUNCTION(BlueprintCallable)
	void ReduceDamageTaken(float damageValue, float CurrStrength, float CurrStamina, float CurrAgility);

	/*****************BoneParts****************************/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Bones")
	TArray<FName> HandBone;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Bones")
	TArray<FVector> BoneLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Bones")
	TArray<FName> NeckBone;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Bones")
	TArray<FName> ChestBone;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Bones")
	TArray<FName> LegBone;
	/**************EndBoneParts***************************/

	/*UPROPERTY(VisibleAnywhere, Category = "Trace")
	AActor* HitActor;

	UPROPERTY(VisibleAnywhere, Category = "Trace")
	FVector Location;

	UPROPERTY(VisibleAnywhere, Category = "Trace")
	FName BoneNames;

	UPROPERTY(VisibleAnywhere, Category = "Trace")
	bool bBlockingHits;*/

	//remove touch inputs from array
	void RemoveFromArray();



protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	void MyDoOnce();

	//Timer to remove touch inputs from array
	void RemoveElementFromArrayTimer();

	//*******************************************TargetLock************************************************************************************************//
	/** called when something enters the sphere component */
	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* OverlappedActor, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** called when something leaves the sphere component */
	UFUNCTION()
	void OnEndOverlap(class UPrimitiveComponent* OverlappedActor, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//Execute the skill
	bool ExecuteAction(bool SkillTrigger, float HitTraceLengths, float AnimRate, float AnimStartTime, UAnimMontage* SkillMovesets, float DealDamage, bool& CDSkill);

	

	//Skill replicate on server
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSkillMoveset(UAnimMontage* ServerSkill, float DamageApplied, float CurrStrength, float CurrStamina, float CurrAgility, float PlayRate, float StartTime, bool SkillFound);

	//Skill replicate on all client
	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void MulticastSkillMoveset(UAnimMontage* MulticastSkill, float DamageApplied, float CurrStrength, float CurrStamina, float CurrAgility, float PlayRate, float StartTime, bool SkillFound);

	//SkillPress replicate on server
	UFUNCTION(Reliable, Server, BlueprintCallable, WithValidation)
	void ServerSkillStartMontage(UAnimMontage* ServerSkill);

	//SkillPress replicate on all client
	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void MulticastSkillStartMontage(UAnimMontage* MulticastSkill);

	//SkillPress replicate on server
	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable, Category = "BlockHit")
	void ServerSkillBlockHitMontage(UAnimMontage* ServerSkill);

	//SkillPress replicate on all client
	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void MulticastSkillBlockHitMontage(UAnimMontage* MulticastSkill);

	//SkillPress replicate on server
	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable, Category = "BlockHit")
	void ServerStopBlockHitMontage(UAnimMontage* ServerSkill);

	//SkillPress replicate on all client
	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void MulticastStopBlockHitMontage(UAnimMontage* MulticastSkill);

	//Get montage to play from server
	UFUNCTION(Unreliable, Server, WithValidation, BlueprintCallable, Category = "Ragdoll")
	void ServerPlayMontage(UAnimInstance* CurrMesh, UAnimMontage* MontageToPlay);

	//Play and replicate montages on all client
	UFUNCTION(Unreliable, NetMulticast, WithValidation)
	void MulticastPlayMontage(UAnimInstance* CurrMesh, UAnimMontage* MontageToPlay);

	/**Function to update the client's damage*/
	void UpdateCurrentMontage(UAnimMontage* MulticastSkill, FTimerHandle* TimerUsed);

	//SkillPress replicate on all client
	UFUNCTION(BlueprintCallable, Category = "Collision")
	void GetDamageFromPhysicsAssetShapeName(FName ShapeName, float& MajorDamageDealt, float& MinorDamageDealt, bool& IsUpperBody, UAnimMontage* DamageMovesets);
	
	//HitLocation where wounds spawned
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	FVector HitLocation;
	
	//Material used to spawn wound
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	UMaterialInterface * DecalMat;

	//Server spawn wounds
	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable)
	void SvrSpawnWounds(class UMaterialInterface * DecalMaterial, class USceneComponent * AttachToComponent, FName AttachPointName, FVector Location);

	//Multicast spawn wounds
	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void MulticastSpawnWounds(class UMaterialInterface * DecalMaterial, class USceneComponent * AttachToComponent, FName AttachPointName, FVector Location);

	//When the touch is hold
	void MoveOnHold();

	//Reset movement after skill is executed
	void ResetMovementMode();

	//Preparing for ragdoll
	UFUNCTION(BlueprintCallable)
	void CallFallRagdoll();

	//Execute ragdoll
	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable)
	void ServerFallRagdoll(AActor* RagdolledActor);

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void MulticastFallRagdoll(AActor* RagdolledActor);

	//increase maximum fitness status
	UFUNCTION(BlueprintCallable)
	void IncreaseMaxFitness(float StrengthIncrement, float StaminaIncrement, float AgilityIncrement, float StaminaPercent, float StrengthPercent, float AgilityPercent);

	//increase fitness status
	UFUNCTION(BlueprintCallable)
	void ChangeCurrentFitness(EOperation Operations, float StrengthVal, float StaminaVal, float AgilityVal, float PercentageLimit);

	//Check if action is executed is tutorial
	UFUNCTION(BlueprintCallable)
	void CheckForAction(FName CurrentAction);

	//Calculate energy spent
	UFUNCTION(BlueprintCallable)
	void FatigueResistanceReduction(float ValDecrement, float PercentageLimit = 1.0f);

	//Calculate energy gained from fitness
	UFUNCTION(BlueprintCallable)
	void TotalVitalityFromFitness(float StaminaPercent, float StrengthPercent, float AgilityPercent);

	UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = "Type"))
	void DetectInputTouch(float CurrEnergyValue, ETouchIndex::Type FingerIndex, FVector2D Location, ETouchType::Type Type);

	//Detect swipe input on pressed
	UFUNCTION(BlueprintCallable)
	void StartDetectSwipe(ETouchIndex::Type FingerIndex, FVector2D Locations, float& StartPressTime, EBodyPart& SwipeParts);

	UFUNCTION(BlueprintCallable)
	void DetectTouchMovement(ETouchIndex::Type FingerIndex, FVector2D Locations);

	UFUNCTION(BlueprintCallable)
	void ResetMyDoOnce();

	//OnRelease
	UFUNCTION(BlueprintCallable)
	void StopDetectTouch(ETouchIndex::Type FingerIndex, float StartPressTime);

	UFUNCTION(BlueprintCallable)
		void CalculateMeshLocation(USceneComponent* Capsule, FVector& FinalLoc);

	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable, Category = "Ragdoll")
		void ServerGetMeshLocation(FVector TempMeshLoc);

	UFUNCTION(Reliable, Client, WithValidation, Category = "Ragdoll")
		void MulticastGetMeshLocation(FVector TempMeshLoc);

	UFUNCTION()
		void OnRep_SetMeshLocation();

	UFUNCTION()
		void OnRep_SetMeshRotation();

	UFUNCTION(BlueprintPure)
		bool CalculatingFacingLocation(class USkeletalMeshComponent* currMesh);

	UFUNCTION(BlueprintCallable)
		void SetUpGetUpOrientation(USkeletalMeshComponent* currMesh);

	UFUNCTION(BlueprintCallable)
		void SetUpGetUpMontage(USkeletalMeshComponent* currMesh);

	//Get skills from input touch combo
	void GetSkillAction(FFingerIndex* FingerIndex);

	//Fire hit trace on server
	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable, Category = "Damage")
	void FireTrace(FVector StartPoint, FVector EndPoint);

	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable, Category = "Damage")
	void DoDamage(AActor* HitActor);

	//Calculate energy spent
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "Energy")
	void EnergySpent(float ValDecrement, float PercentageLimit = 1.0f);

	/**Function to update the damaged client's health**/
	UFUNCTION(Reliable, Client, WithValidation, BlueprintCallable, Category = "Damage")
	void UpdateHealth(int playerIndex, float HealthChange);

	/**Function to update the client's damage*/
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "Damage")
	void UpdateDamage(float DamageValue, float CurrStrength, float CurrStamina, float CurrAgility);

	//Function to update progressbar over time
	void UpdateCurrentPlayerMainStatusBar(EBarType Type, EMainPlayerStats StatType, FTimerHandle FirstHandle, FTimerHandle SecondHandle);

	//Update value when the timer is active
	void UpdateStatusValueTimer(FTimerHandle newHandle, EOperation Operation, bool StopOnFull, float ChangeVal, float Value, int MaxVal, float MinVal, float& totalVal);

	


	//***********************************Variables********************************************//
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SwipeDirection")
	EInputType SwipeDir;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Skills")
	TArray<FName> SkillNames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	FString LevelName;


protected:
	/**************************************START STATS******************************************/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetLock")
	bool isAI = false;

	//Major regen time rate
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	float MajorHealthRate = 1.0f;

	//Minor regen time rate
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	float MinorHealthRate = 0.1f;

	//Energy regen time rate
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	float EnergyRate = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	FTimerHandle StartHealthTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	FTimerHandle StartSecondaryHealthTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	FTimerHandle StartEnergyTimer;

	//Current Stamina value
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	float Stamina = 0.0f;

	//Current Strength value
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	float Strength = 0.0f;

	//Current Agility value
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	float Agility = 0.0f;

	//Current fitness value
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	float Fitness = 0.0f;

	//Maximum Stamina
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	int MaxStamina = 700;

	//Maximum Strength
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	int MaxStrength = 700;

	//Maximum agility
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	int MaxAgility = 700;

	//current stamina in percentage
	UPROPERTY(BlueprintReadWrite, Category = "Status")
	float StaminaPercentage;

	//current strength in percentage
	UPROPERTY(BlueprintReadWrite, Category = "Status")
	float StrengthPercentage;

	//Current agility in percentage
	UPROPERTY(BlueprintReadWrite, Category = "Status")
	float AgilityPercentage;

	UPROPERTY(BlueprintReadWrite, Category = "Status")
	float Vitality = 0.0f;

	//Current energy
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	float playerEnergy;

	//Maximum energy
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	int MaxEnergy;

	//current energy in percentage
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	float EnergyPercentage;

	//The amount of fatigue resistance the character currently has
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	float PlayerFatigue;

	//The amount of fatigue resistance percentage the character currently has
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	float FatiguePercentage;

	//The amount of max fatigue resistance the character currently has
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	int MaxFatigue;

	//Current major pain value
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;

	//Current minor pain value
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Health")
	float SecondaryHealth;

	//Maximum pain value
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Health")
	int MaxHealth;

	//Current major pain value in percentage
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Health")
	float playerHealth;

	//Current minor pain value in percentage
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Health")
	float playerHealth_1;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Magic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy")
	float DelaySpeed = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool canMove;

	UPROPERTY(Replicated, EditAnywhere,  BlueprintReadWrite, Category = "Anim")
	float SkillPlayrate = 1.0f;

	/**************************************END STATS******************************************/

	//Assign data table from bp 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* ActionTable;

	//Assign data table from bp 
	UPROPERTY(EditAnywhere)
	UDataTable* BodyDamageTable;

	//The amount of damage to reduce
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
	bool defensiveMode;

	//Circle area enums
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DoOnce")
	TArray<FFingerIndex> InputTouch;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DoOnce")
	TArray<TEnumAsByte<ETouchIndex::Type>> InputTouchIndex;

	//The amount of damage to apply after reduction
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
	float damageAfterReduction;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Anim")
	FName BoneName = "pelvis";

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Anim")
	float BlendWeight;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Health")
	bool Recovering;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Anim")
	bool InRagdoll;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, Category = "Anim")
	bool IsCollide = false;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Anim")
	float PhysicsAlpha;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	float IKOffsetRightfoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	float IKOffsetLeftFoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	float Scale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	float IKTraceDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	FName RightFootSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	FName LeftFootSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interp")
	float IKInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interp")
	float CapsuleComponentInterpValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interp")
	float CapsuleRadiusMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interp")
	float CapsuleRadiusMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interp")
	bool CapsuleComponentSizeAdjust;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float EnergyReduce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	bool CanAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	bool Skill1Trigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	bool CDSkill1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	bool Skill2Trigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	bool CDSkill2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	bool Skill3Trigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	bool CDSkill3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	bool SkillUltiTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	bool CDSkillUlti;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float UltiDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	int ConeTraceSteps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	float ConeTraceArcWidthinDegrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	float ConeSweepRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	FVector2D PeakVector;

	bool SkillExecuted = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	FVector2D StartLocation;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	//FVector2D PriorLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool IsPressed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool Left;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool Right;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool Up;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool Down;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FVector LyingLocation;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* GetUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	FRotator CapsuleRotation;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Status")
	bool IsHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float DamageVal;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Anim")
	bool StopRagdoll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float HitTraceLength;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Anim")
	bool RightKickColActivate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Anim")
	bool LeftKickColActivate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Anim")
	bool LeftHandColActivate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	bool RightHandColActivate;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, Category = "Combo")
	bool IsAttacking;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combo")
	bool ChainAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combo")
	int Combo;

	//********************TargetOnLock**************************************//
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Targetlock")
	float PlayerToEnemyDistance;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Targetlock")
	bool isOverlap = false;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Targetlock")
	bool CanBeTargeted;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Targetlock")
	bool TargetLocked;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Targetlock")
	ATodakBattleArenaCharacter* NearestTarget;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Targetlock")
	float ClosestTargetDistance;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Targetlock")
	int MaximumTargetDistance = 400;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Targetlock")
	ATodakBattleArenaCharacter* EnemyElement;

	//*********************************************************************//

	

	///////////////For swipe gesture//////////////////////////////
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
	bool DoOnce = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SwipeAction")
	float SwipeStartTime;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, Category = "Status")
	bool SkillTriggered = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Anim")
	float SkillStopTime;

	//The amount of damage to apply
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
	float damage;

	//The amount of damage to apply
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
	float MajorDamage;

	//The amount of damage to apply
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
	float MinorDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
	float LocalDamage;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* RPCServerSkill;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* RPCMultiCastSkill;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* SkillMoveset;
	/////////////////////////////////////////////////////////////

	/////////////////For touch start/hold/////////////////////////
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* RPCServerSkillHold;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* RPCMultiCastSkillHold;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* SkillHold;
	/////////////////////////////////////////////////////////////

	////////////////////For BlockHit/////////////////////////////
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* RPCServerBlockHit;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* RPCMultiCastBlockHit;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* BlockHit;
	/////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Misc")
	TEnumAsByte<EPhysicalSurface>Surface = EPhysicalSurface::SurfaceType_Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Misc")
	float RockFallPercentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Misc")
	float StoneFallPercentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	bool CinematicTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	bool BurstActivate;

	

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	//FVector ;
	//*********************************End Variables*******************************************//

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Stats")
	class UBaseCharacterWidget* WidgetHUD;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	TSubclassOf<UBaseCharacterWidget> CharacterHUD;

};