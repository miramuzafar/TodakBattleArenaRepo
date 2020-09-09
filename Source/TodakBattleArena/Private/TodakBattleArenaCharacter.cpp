// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TodakBattleArenaCharacter.h"
#include "Engine.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/AggregateGeom.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Math/UnrealMathUtility.h"
#include "..\Public\GestureInputsFunctions.h"
#include "GestureMathLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"
#include "TodakBattleArenaGameMode.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "BaseCharacterWidget.h"
#include "Net/UnrealNetwork.h"
#include "TargetLockInterface.h"
#include "Misc/DateTime.h"
#include "DrawDebugHelpers.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TimelineComponent.h"

//////////////////////////////////////////////////////////////////////////
// ATodakBattleArenaCharacter

ATodakBattleArenaCharacter::ATodakBattleArenaCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bDoCollisionTest = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	damageAfterReduction = 0.0f;

	ToBeIgnoredCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("ToBeIgnoredCollision"));
	ToBeIgnoredCollision->SetupAttachment(GetMesh());

	//TargetLockCollision
	LockOnCollision = CreateDefaultSubobject<USphereComponent>(TEXT("LockOnCollision"));
	LockOnCollision->SetupAttachment(RootComponent);
	LockOnCollision->InitSphereRadius(400.0f);

	LockOnCollision->OnComponentBeginOverlap.AddDynamic(this, &ATodakBattleArenaCharacter::OnBeginOverlap);
	LockOnCollision->OnComponentEndOverlap.AddDynamic(this, &ATodakBattleArenaCharacter::OnEndOverlap);

	/*static ConstructorHelpers::FObjectFinder<UCurveFloat> Curvy(TEXT("CurveFloat'/Game/Blueprints/CurveFloatBP.CurveFloatBP"));

	if (Curvy.Object)
	{
		fCurve = Curvy.Object;
	}

	RagdollTimeline = ObjectInitializer.CreateDefaultSubobject<UTimelineComponent>(this, TEXT("RagdollTimeline"));
	InterpFunction.BindFunction(this, FName{ TEXT("TimelineFloatReturn") });

	RagdollTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));

	InterpFunction.BindUFunction(this, FName("TimelineFloatReturn"));
	TimelineFinished.BindUFunction(this, FName("OnTimelineFinished"));

	ZOffset = 50.0f;*/
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void ATodakBattleArenaCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATodakBattleArenaCharacter, damage);
	DOREPLIFETIME(ATodakBattleArenaCharacter, MajorDamage);
	DOREPLIFETIME(ATodakBattleArenaCharacter, MinorDamage);
	DOREPLIFETIME(ATodakBattleArenaCharacter, Stamina);
	DOREPLIFETIME(ATodakBattleArenaCharacter, Strength);
	DOREPLIFETIME(ATodakBattleArenaCharacter, Agility);
	DOREPLIFETIME(ATodakBattleArenaCharacter, Health);
	DOREPLIFETIME(ATodakBattleArenaCharacter, SecondaryHealth);
	DOREPLIFETIME(ATodakBattleArenaCharacter, MaxStrength);
	DOREPLIFETIME(ATodakBattleArenaCharacter, MaxAgility);
	DOREPLIFETIME(ATodakBattleArenaCharacter, MaxStamina);
	DOREPLIFETIME(ATodakBattleArenaCharacter, MaxHealth);
	DOREPLIFETIME(ATodakBattleArenaCharacter, playerHealth);
	DOREPLIFETIME(ATodakBattleArenaCharacter, playerHealth_1);
	DOREPLIFETIME(ATodakBattleArenaCharacter, playerEnergy);
	DOREPLIFETIME(ATodakBattleArenaCharacter, MaxEnergy);
	DOREPLIFETIME(ATodakBattleArenaCharacter, EnergyPercentage);

	//Timers
	DOREPLIFETIME(ATodakBattleArenaCharacter, MajorHealthRate);
	DOREPLIFETIME(ATodakBattleArenaCharacter, MinorHealthRate);
	DOREPLIFETIME(ATodakBattleArenaCharacter, EnergyRate);

	//TargetOnLock
	DOREPLIFETIME(ATodakBattleArenaCharacter, PlayerToEnemyDistance);
	DOREPLIFETIME(ATodakBattleArenaCharacter, CanBeTargeted);
	DOREPLIFETIME(ATodakBattleArenaCharacter, TargetLocked);
	DOREPLIFETIME(ATodakBattleArenaCharacter, NearestTarget);
	DOREPLIFETIME(ATodakBattleArenaCharacter, ClosestTargetDistance);
	DOREPLIFETIME(ATodakBattleArenaCharacter, MaximumTargetDistance);
	DOREPLIFETIME(ATodakBattleArenaCharacter, EnemyElement);
	DOREPLIFETIME(ATodakBattleArenaCharacter, isOverlap);

	//SwipeGesture
	DOREPLIFETIME(ATodakBattleArenaCharacter, Hit);
	DOREPLIFETIME(ATodakBattleArenaCharacter, IsHit);
	DOREPLIFETIME(ATodakBattleArenaCharacter, BlockedHit);
	DOREPLIFETIME(ATodakBattleArenaCharacter, AICanAttack);
	DOREPLIFETIME(ATodakBattleArenaCharacter, BlockHitTimer);

	//**AnimMontage**//
	DOREPLIFETIME(ATodakBattleArenaCharacter, BlockHit);
	DOREPLIFETIME(ATodakBattleArenaCharacter, SkillMoveset);
	DOREPLIFETIME(ATodakBattleArenaCharacter, SkillHold);

	DOREPLIFETIME(ATodakBattleArenaCharacter, RPCServerBlockHit);
	DOREPLIFETIME(ATodakBattleArenaCharacter, RPCServerSkill);
	DOREPLIFETIME(ATodakBattleArenaCharacter, RPCServerSkillHold);

	DOREPLIFETIME(ATodakBattleArenaCharacter, RPCMultiCastBlockHit);
	DOREPLIFETIME(ATodakBattleArenaCharacter, RPCMultiCastSkill);
	DOREPLIFETIME(ATodakBattleArenaCharacter, RPCMultiCastSkillHold);

	
	DOREPLIFETIME(ATodakBattleArenaCharacter, RPCMulticastGetUp);
	DOREPLIFETIME(ATodakBattleArenaCharacter, RPCServerGetUp);
	DOREPLIFETIME(ATodakBattleArenaCharacter, GetUpAnim);
	DOREPLIFETIME(ATodakBattleArenaCharacter, RagdollTimeline);
	DOREPLIFETIME(ATodakBattleArenaCharacter, ragdollTL);
	DOREPLIFETIME(ATodakBattleArenaCharacter, DoneRagdoll);

	//**EndAnimMontage**//

	DOREPLIFETIME(ATodakBattleArenaCharacter, InRagdoll);
	DOREPLIFETIME(ATodakBattleArenaCharacter, Recovering);
	DOREPLIFETIME(ATodakBattleArenaCharacter, StopRagdoll);
	DOREPLIFETIME(ATodakBattleArenaCharacter, IsAttacking);

	DOREPLIFETIME(ATodakBattleArenaCharacter, IsCollide);
	DOREPLIFETIME(ATodakBattleArenaCharacter, BlendWeight);
	DOREPLIFETIME(ATodakBattleArenaCharacter, BoneName);


	DOREPLIFETIME(ATodakBattleArenaCharacter, IsCollide);
	DOREPLIFETIME(ATodakBattleArenaCharacter, BlendWeight);
	DOREPLIFETIME(ATodakBattleArenaCharacter, BoneName);

	//SpawnWounds
	DOREPLIFETIME(ATodakBattleArenaCharacter, HitLocation);
	DOREPLIFETIME(ATodakBattleArenaCharacter, DecalMat);
}

void ATodakBattleArenaCharacter::LockOn_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("Interface Is Called")));
}

void ATodakBattleArenaCharacter::TriggerToggleLockOn()
{
	//if the controller is valid, set its rotation to the rotator above
	FRotator NewRotator;
	AController* NewController = SetNewControlRotation(NewRotator);
	if (NewController != nullptr)
	{
		/*FVector OutLength;
		float Dist;
		(GetActorLocation() - NearestTarget->GetActorLocation()).ToDirectionAndLength(OutLength, Dist);
		if (Dist < 100.0f)
		{
			if (OutLength.Normalize())
			{
				FHitResult newHit;
				FVector newLength = FVector(OutLength);
				FVector Val = FVector(OutLength.X, OutLength.Y, OutLength.Z);
				if (SetActorLocation(Val, true, nullptr, ETeleportType::None))
				{
					UE_LOG(LogTemp, Warning, TEXT("Cannot pass through"));
				}
			}
		}*/
		if (GetCharacterMovement()->Velocity.Size() > 0.0f)
		{
			NewController->SetControlRotation(NewRotator);
		}
		else
			Controller->SetControlRotation(Controller->GetControlRotation());
	}
}

AController* ATodakBattleArenaCharacter::SetNewControlRotation(FRotator& RotatorParam)
{
	if (Controller)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Locally control"));
		if (Controller != nullptr && NearestTarget != nullptr && NearestTarget->Controller != this->Controller)
		{
			FVector Start = FVector(ToBeIgnoredCollision->GetComponentLocation());
			FVector End = NearestTarget->GetActorLocation();

			//find forward rotation of player towards target
			FRotator target = UKismetMathLibrary::FindLookAtRotation(Start, End);

			//Interpolate the rotator value for smooth rotation
			FRotator InterpVal = UKismetMathLibrary::RInterpTo(this->Controller->GetControlRotation(), target, 0.01f, 1.0f);

			//Return final rotator val
			RotatorParam = FRotator(InterpVal.Pitch, InterpVal.Yaw, this->Controller->GetControlRotation().Roll);

			//Return controller to owns it
			return this->Controller;
		}
	}
	return nullptr;
}

void ATodakBattleArenaCharacter::BeginPlay()
{
	Super::BeginPlay();

	//RagdollTimeline->AddInterpFloat(fCurve, InterpFunction(), FName{ TEXT("Floaty") });
	//RagdollTimeline->Play();

	for (TActorIterator<ATodakBattleArenaCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Looped"));
		ATodakBattleArenaCharacter* CurrActor = *ActorItr;
		if (CurrActor != this)
		{
			UE_LOG(LogTemp, Warning, TEXT("AI"));
			EnemyElement = CurrActor;
			NearestTarget = EnemyElement;
			break;
		}
	}

	FVector2D Result = FVector2D(1, 1);

	Result.X = GSystemResolution.ResX;
	Result.Y = GSystemResolution.ResY;

	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("Screen resolution : %s"), *Result.ToString()));

	Stamina = MaxStamina;
	Strength = MaxStrength;
	Agility = MaxAgility;

	StaminaPercentage = 1.0f;
	StrengthPercentage = 1.0f;
	AgilityPercentage = 1.0f;

	MaxHealth = 500;
	MaxEnergy = 700;

	//Energy
	playerEnergy = UGestureMathLibrary::CalculateValueFromPercentage(10.0f, MaxStamina, 100.0f) + UGestureMathLibrary::CalculateValueFromPercentage(10.0f, MaxStrength, 100.0f) + MaxEnergy;
	MaxEnergy = playerEnergy;
	EnergyPercentage = 1.0f;
	UE_LOG(LogTemp, Warning, TEXT("Energy : %f"), playerEnergy);

	//Health
	/*Health = UGestureMathLibrary::CalculateValueFromPercentage(10.0f, MaxStrength, 100.0f) + MaxHealth;
	MaxHealth = Health;
	playerHealth = 1.0f;
	UE_LOG(LogTemp, Warning, TEXT("MaxHealth : %d"), MaxHealth);*/

	//Get vitality status from current fitness level when game starts
	//TotalVitalityFromFitness(0.7f, 0.2f, 0.1f);
	if (!isAI)
	{
		if (LevelName == UGameplayStatics::GetCurrentLevelName(this, true))
		{
			SkillNames.AddUnique("Move");
			SkillNames.AddUnique("Rotate");

			//Used in error reporting
			FString Context;

			for (auto& name : ActionTable->GetRowNames())
			{
				FActionSkill* row = ActionTable->FindRow<FActionSkill>(name, Context);
				if (row)
				{
					SkillNames.AddUnique(name);
				}
			}
		}
		else
		{
			if (WidgetHUD)
			{
				//Assign values to the widget
					//Stamina
				const FName locTextControlStaminaName = FName(TEXT("Stamina"));
				UTextBlock* locTextControlStamina = (UTextBlock*)(WidgetHUD->WidgetTree->FindWidget(locTextControlStaminaName));

				if (locTextControlStamina != nullptr)
				{
					locTextControlStamina->SetText(UGestureMathLibrary::PrintStatusValue(Stamina, MaxStamina, "Stamina: "));
				}

				//Strength
				const FName locTextControlStrengthName = FName(TEXT("Strength"));
				UTextBlock* locTextControlStrength = (UTextBlock*)(WidgetHUD->WidgetTree->FindWidget(locTextControlStrengthName));

				if (locTextControlStrength != nullptr)
				{
					locTextControlStrength->SetText(UGestureMathLibrary::PrintStatusValue(Strength, MaxStrength, "Strength: "));
				}

				//Agility
				const FName locTextControlAgilityName = FName(TEXT("Agility"));
				UTextBlock* locTextControlAgility = (UTextBlock*)(WidgetHUD->WidgetTree->FindWidget(locTextControlAgilityName));

				if (locTextControlAgility != nullptr)
				{
					locTextControlAgility->SetText(UGestureMathLibrary::PrintStatusValue(Agility, MaxAgility, "Agility: "));
				}

				FTimerHandle handle;
				{
					//Energy stats
					const FName locTextControlEnergyName = FName(TEXT("Energy"));
					UTextBlock* locTextControlEnergy = (UTextBlock*)(WidgetHUD->WidgetTree->FindWidget(locTextControlEnergyName));

					const FName locTextControlEnergyPercent = FName(TEXT("EnergyText"));
					UTextBlock* locTextControlEnergyPercentBlock = (UTextBlock*)(WidgetHUD->WidgetTree->FindWidget(locTextControlEnergyPercent));

					const FName locTextControlEnergyBar = FName(TEXT("EnergyBar"));
					UProgressBar* energyBar = (UProgressBar*)(WidgetHUD->WidgetTree->FindWidget(locTextControlEnergyBar));

					if (energyBar != nullptr)
					{
						EnergyPercentage = UGestureMathLibrary::SetProgressBarValue("Energy", energyBar, locTextControlEnergyPercentBlock, locTextControlEnergy, playerEnergy, MaxEnergy);
					}
				}
				{
					//Health stats
					const FName locTextControlHealthName = FName(TEXT("Health"));
					UTextBlock* locTextControlHealth = (UTextBlock*)(WidgetHUD->WidgetTree->FindWidget(locTextControlHealthName));

					const FName locTextControlHPName = FName(TEXT("HP"));
					UTextBlock* locTextControlHP = (UTextBlock*)(WidgetHUD->WidgetTree->FindWidget(locTextControlHPName));

					const FName locTextControlHealthBar = FName(TEXT("HPBar"));
					UProgressBar* healthBar = (UProgressBar*)(WidgetHUD->WidgetTree->FindWidget(locTextControlHealthBar));

					if (healthBar != nullptr)
					{
						playerHealth = UGestureMathLibrary::SetProgressBarValue("Pain Meter", healthBar, locTextControlHealth, locTextControlHP, Health, MaxHealth);
					}
				}
			}
		}
	}

	/*if (fCurve)
	{
		// Add the float curve to the timeline and conenct to InterpFunction() delegate
		RagdollTimeline->AddInterpFloat(fCurve, InterpFunction, FName("Alpha"));

		//Add timeline to TimelineFinished()
		RagdollTimeline->SetTimelineFinishedFunc(TimelineFinished);

		//Setting location vectors
		StartLocation = GetActorLocation();
		EndLocation = FVector(StartLocation.X, StartLocation.Y, StartLocation.Z + ZOffset);

		// Setting our timeline's settings before we start it
		RagdollTimeline->SetLooping(false);
		RagdollTimeline->SetIgnoreTimeDilation(true);

		RagdollTimeline->Play();

	}*/
}

void ATodakBattleArenaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*if (EnableMovement == true)
	{
		if (GetMesh()->GetAnimInstance()->Montage_IsActive(RPCMultiCastSkillHold))
		{
			GetMesh()->GetAnimInstance()->Montage_Stop(.1f, RPCMultiCastSkillHold);
		}
		MoveOnHold();
	}*/
	//.267f
	//if hold montage is active, prepare for blocking hit
	if (isAI == false && GetMesh()->GetAnimInstance()->Montage_IsActive(RPCMultiCastSkillHold) && GetMesh()->GetAnimInstance()->Montage_GetPosition(RPCMultiCastSkillHold) >= SkillStopTime && EnableMovement == false)// && EnableMovement == false
	{
		GetMesh()->GetAnimInstance()->Montage_Pause(RPCMultiCastSkillHold);
	}
	/*if (NearestTarget != nullptr && TargetLocked == true)
	{
		TriggerToggleLockOn();
	}*/
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATodakBattleArenaCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATodakBattleArenaCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATodakBattleArenaCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATodakBattleArenaCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATodakBattleArenaCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ATodakBattleArenaCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ATodakBattleArenaCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ATodakBattleArenaCharacter::OnResetVR);
}

void ATodakBattleArenaCharacter::GetSkillAction(FFingerIndex* FingerIndex)
{
	//Used in error reporting
	FString Context;

	bool Found = false;
	bool SkillFound = false;

	//Search the skill available
	for (auto& name : ActionTable->GetRowNames())
	{
		FActionSkill* row = ActionTable->FindRow<FActionSkill>(name, Context);
		if (row)
		{
			//Check if the input is same as the input needed to execute the skill
			if (row->SwipeActions.Contains(FingerIndex->SwipeActions) && row->BodyParts.Contains(FingerIndex->BodyParts))
			{
				SkillFound = true;
			}
			if (SkillFound == true)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Touch index is %s"), (*GETENUMSTRING("ETouchIndex", FingerIndex->FingerIndex))));
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Touch swipeactions is %s"), (*GETENUMSTRING("EInputType", FingerIndex->SwipeActions))));
				//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Orange, FString::Printf(TEXT("Equal : %s"), areEqual(row->SwipeActions, InputType, row->SwipeActions.Num(), InputType.Num()) && areEqual(row->BodyParts, InputPart, row->BodyParts.Num(), InputPart.Num()) ? TEXT("True") : TEXT("False")));
				row->SkillTrigger = true;
				SkillTriggered = row->SkillTrigger;

				//Execute skill if cooldown is finished
				if (row->CDSkill == true)
				{
					row->SkillTrigger = false;
					SkillTriggered = row->SkillTrigger;
					row->CDSkill = ExecuteAction(row->SkillTrigger, row->HitTraceLength, row->SkillMoveSetRate, row->StartMontage, row->SkillMoveset, row->Damage, row->CDSkill);
					FingerIndex->bDo = true;
					Found = true;
					CheckForAction(name);
					if (SkillTriggered == false)
					{
						row->SkillTrigger = false;
					}
					SkillFound = false;
					break;
				}
				else if (row->CDSkill == false)
				{
					GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Touch index is %s"), (*GETENUMSTRING("ETouchIndex", FingerIndex->FingerIndex))));
					GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Touch swipeactions is %s"), (*GETENUMSTRING("EInputType", FingerIndex->SwipeActions))));
					//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Orange, FString::Printf(TEXT("Equal : %s"), areEqual(row->SwipeActions, InputType, row->SwipeActions.Num(), InputType.Num()) && areEqual(row->BodyParts, InputPart, row->BodyParts.Num(), InputPart.Num()) ? TEXT("True") : TEXT("False")));
					row->SkillTrigger = true;
					SkillTriggered = row->SkillTrigger;
					row->CDSkill = ExecuteAction(row->SkillTrigger, row->HitTraceLength, row->SkillMoveSetRate, row->StartMontage, row->SkillMoveset, row->Damage, row->CDSkill);
					FingerIndex->bDo = true;
					Found = true;
					CheckForAction(name);
					if (SkillTriggered == false)
					{
						row->SkillTrigger = false;
					}
					SkillFound = false;
					break;
				}
			}
		}
	}

	if (Found)
	{
		return;
	}
}

bool ATodakBattleArenaCharacter::UpdateHealth_Validate(int playerIndex, float HealthChange)
{
	if (Health >= MaxHealth)
	{
		//return false; // This will disconnect the caller
	}
	return true;
}

void ATodakBattleArenaCharacter::UpdateHealth_Implementation(int playerIndex, float HealthChange)
{
	//if can be accessed by the owning client
	if (IsLocallyControlled())
	{
		//Add pain meter value
		float currVal = Health + HealthChange + MajorDamage;
		if (currVal >= MaxHealth)
		{
			Health = MaxHealth;
		}
		else
			Health = currVal;

		float currSecHealth = Health + HealthChange + MinorDamage;
		if (currSecHealth >= MaxHealth)
		{
			SecondaryHealth = MaxHealth;
		}
		else
			SecondaryHealth = currSecHealth; 
		UE_LOG(LogTemp, Warning, TEXT("Health : %f"), Health);

		//Get the secondary progressbar for pain meter
		const FName locTextControlHealthBar_1 = FName(TEXT("HPBar_1"));
		UProgressBar* healthBar_1 = (UProgressBar*)(WidgetHUD->WidgetTree->FindWidget(locTextControlHealthBar_1));

		//Update both progress bar for pain meter
		playerHealth = UGestureInputsFunctions::UpdateProgressBarComponent(WidgetHUD, "HPBar", "Health", "HP", "Pain Meter", Health, MaxHealth);
		playerHealth_1 = UGestureMathLibrary::SetProgressBarValue("Pain Meter", healthBar_1, nullptr, nullptr, SecondaryHealth, MaxHealth);

		//Start Pain Meter degeneration
		if (GetWorld()->GetTimerManager().IsTimerActive(StartHealthTimer) == false && (Health > 0.0f))
		{
			//For first pain meter progress bar
			FTimerDelegate FunctionsName;
			//FunctionsName = FTimerDelegate::CreateUObject(this, &ATodakBattleArenaCharacter::UpdateHealthStatusBar, EBarType::PrimaryProgressBar);
			FunctionsName = FTimerDelegate::CreateUObject(this, &ATodakBattleArenaCharacter::UpdateCurrentPlayerMainStatusBar, EBarType::PrimaryProgressBar, EMainPlayerStats::PainMeter, StartHealthTimer, StartSecondaryHealthTimer);

			UE_LOG(LogTemp, Warning, TEXT("TimerHealth has started!"));
			GetWorld()->GetTimerManager().SetTimer(StartHealthTimer, FunctionsName, MajorHealthRate, true);
		}
		//For second pain meter progress bar
		if (GetWorld()->GetTimerManager().IsTimerActive(StartSecondaryHealthTimer) == false && ((SecondaryHealth > Health) && (SecondaryHealth > 0.0f)))
		{
			FTimerDelegate FunctionsNames;
			FunctionsNames = FTimerDelegate::CreateUObject(this, &ATodakBattleArenaCharacter::UpdateCurrentPlayerMainStatusBar, EBarType::SecondaryProgressBar, EMainPlayerStats::PainMeter, StartHealthTimer, StartSecondaryHealthTimer);

			UE_LOG(LogTemp, Warning, TEXT("SecondaryHealth has started!"));
			GetWorld()->GetTimerManager().SetTimer(StartSecondaryHealthTimer, FunctionsNames, MinorHealthRate, true);
		}
	}
}

void ATodakBattleArenaCharacter::UpdateDamage(float DamageValue)
{
	//damage = DamageValue;
	// Increase (or decrease) current damage
	if (GetLocalRole() == ROLE_Authority)
	{
		//Calculate total damage applied from current action with current instigator's maximum strength
		float DamageFromStrength = UGestureMathLibrary::CalculateValueFromPercentage(10.0f, MaxStrength, 100.0f);
		damage = DamageValue + DamageFromStrength;
		UE_LOG(LogTemp, Warning, TEXT("Damage : %f"), damage);
	}
}

bool ATodakBattleArenaCharacter::ServerSkillMoveset_Validate(UAnimMontage* ServerSkill, float DamageApplied, float PlayRate, float StartTime, bool SkillFound)
{
	return true;
}

void ATodakBattleArenaCharacter::ServerSkillMoveset_Implementation(UAnimMontage* ServerSkill, float DamageApplied, float PlayRate, float StartTime, bool SkillFound)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		RPCServerSkill = ServerSkill;
		SkillExecuted = SkillFound;
		//damage = DamageApplied;
		MulticastSkillMoveset(RPCServerSkill, DamageApplied, PlayRate, StartTime, SkillFound);
	}
}

bool ATodakBattleArenaCharacter::MulticastSkillMoveset_Validate(UAnimMontage* MulticastSkill, float DamageApplied, float PlayRate, float StartTime, bool SkillFound)
{
	return true;
}

//Play swipe action anim
void ATodakBattleArenaCharacter::MulticastSkillMoveset_Implementation(UAnimMontage* MulticastSkill, float DamageApplied, float PlayRate, float StartTime, bool SkillFound)
{
	//if action is found, play new action anim, else stop the current action, else stop the current anim immediately
	if (SkillFound == true)
	{
		//If the anim is not currently playing
		FTimerHandle Delay;

		//Play new anim on client
		RPCMultiCastSkill = MulticastSkill;
		float Duration = GetMesh()->GetAnimInstance()->Montage_Play(RPCMultiCastSkill, PlayRate, EMontagePlayReturnType::MontageLength, StartTime, true);
		UpdateDamage(DamageApplied);
		
		//stop current played anim
		this->GetMesh()->GetAnimInstance()->Montage_Stop(3.0f, RPCMultiCastSkillHold);

		if (GetWorld()->GetTimerManager().IsTimerActive(Delay) == false)
		{
			//GetMesh()->SetSimulatePhysics(false);
			GetWorld()->GetTimerManager().SetTimer(Delay, this, &ATodakBattleArenaCharacter::ResetMovementMode, Duration, false);

			//if this client has access
			if (IsLocallyControlled())
			{
				if (this->BlockedHit == true)
				{
					this->BlockedHit = false;
				}
				//if still in blocked hit state
				//Reduce player energy after action
				EnergySpent(10.0f, 100.0f);
				UE_LOG(LogTemp, Warning, TEXT("Energy: %f"), playerEnergy);

				//Update stats after anim is played
				if (GetWorld()->GetTimerManager().IsTimerActive(StartEnergyTimer) == false && (playerEnergy <= MaxEnergy))
				{
					//Set timer for EnergyBar to regen after action
					FTimerDelegate FunctionsNames;
					FunctionsNames = FTimerDelegate::CreateUObject(this, &ATodakBattleArenaCharacter::UpdateCurrentPlayerMainStatusBar, EBarType::PrimaryProgressBar, EMainPlayerStats::Energy, StartEnergyTimer, StartEnergyTimer);

					UE_LOG(LogTemp, Warning, TEXT("EnergyTimer has started!"));
					GetWorld()->GetTimerManager().SetTimer(StartEnergyTimer, FunctionsNames, EnergyRate, true);

					//UE_LOG(LogTemp, Warning, TEXT("Timer has started!"));
					//GetWorld()->GetTimerManager().SetTimer(StartEnergyTimer, this, &ATodakBattleArenaCharacter::UpdateEnergyStatusBar, 1.5f, true, 2.0f);
				}
			}
		}
	}
	else
	{
		//If the anim is not currently playing
		this->StopAnimMontage(RPCMultiCastSkillHold);
		ResetMovementMode();
		if (IsLocallyControlled())
		{
			if (this->BlockedHit == true)
			{
				this->BlockedHit = false;
			}
		}
		
		//if still in blocked hit state
	}
}
//
bool ATodakBattleArenaCharacter::ServerSkillStartMontage_Validate(UAnimMontage* ServerSkill)
{
	return true;
}

void ATodakBattleArenaCharacter::ServerSkillStartMontage_Implementation(UAnimMontage* ServerSkill)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		RPCServerSkillHold = ServerSkill;
		MulticastSkillStartMontage(RPCServerSkillHold);
	}
}

bool ATodakBattleArenaCharacter::MulticastSkillStartMontage_Validate(UAnimMontage* MulticastSkill)
{
	return true;
}

void ATodakBattleArenaCharacter::MulticastSkillStartMontage_Implementation(UAnimMontage* MulticastSkill)
{
	//Play anim on touch press/hold
	RPCMultiCastSkillHold = MulticastSkill;
	this->GetMesh()->GetAnimInstance()->Montage_Play(RPCMultiCastSkillHold, 1.0f);
}

bool ATodakBattleArenaCharacter::ServerSkillBlockHitMontage_Validate(UAnimMontage* ServerSkill)
{
	return true;
}

void ATodakBattleArenaCharacter::ServerSkillBlockHitMontage_Implementation(UAnimMontage* ServerSkill)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		RPCServerBlockHit = ServerSkill;
		MulticastSkillBlockHitMontage(RPCServerBlockHit);
	}
}

bool ATodakBattleArenaCharacter::MulticastSkillBlockHitMontage_Validate(UAnimMontage* MulticastSkill)
{
	return true;
}

void ATodakBattleArenaCharacter::MulticastSkillBlockHitMontage_Implementation(UAnimMontage* MulticastSkill)
{
	//Play anim on block incoming hit
	if (GetWorld()->GetTimerManager().IsTimerActive(BlockHitTimer) == false && GetWorld()->GetTimerManager().IsTimerPaused(BlockHitTimer) == false)
	{
		RPCMultiCastBlockHit = MulticastSkill;

		//play skillblock anim montage
		//float Duration = GetMesh()->GetAnimInstance()->Montage_Play(RPCMultiCastBlockHit, 2.0f, EMontagePlayReturnType::MontageLength, 0.7f);
		this->GetMesh()->GetAnimInstance()->Montage_Play(RPCMultiCastBlockHit, 2.0f, EMontagePlayReturnType::MontageLength, false);
		
		//float Duration = GetMesh()->GetAnimInstance()->Montage_Play(RPCMultiCastBlockHit, 2.0f, EMontagePlayReturnType::MontageLength, 0.7f);
		this->GetMesh()->GetAnimInstance()->Montage_Stop(3.0f, RPCMultiCastSkillHold);

		//run the timer
		if (IsLocallyControlled())
		{
			//Create uobject for timer delegate
			FTimerDelegate FunctionsNames;
			FunctionsNames = FTimerDelegate::CreateUObject(this, &ATodakBattleArenaCharacter::UpdateCurrentMontage, RPCMultiCastBlockHit, &BlockHitTimer);
			GetWorld()->GetTimerManager().SetTimer(BlockHitTimer, FunctionsNames, 0.757f, false);
		}
	}
}

bool ATodakBattleArenaCharacter::ServerStopBlockHitMontage_Validate(UAnimMontage* ServerSkill)
{
	return true;
}
void ATodakBattleArenaCharacter::ServerStopBlockHitMontage_Implementation(UAnimMontage* ServerSkill)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		MulticastStopBlockHitMontage(ServerSkill);
	}
}

bool ATodakBattleArenaCharacter::MulticastStopBlockHitMontage_Validate(UAnimMontage* MulticastSkill)
{
	return true;
}

void ATodakBattleArenaCharacter::MulticastStopBlockHitMontage_Implementation(UAnimMontage* MulticastSkill)
{
	if (IsLocallyControlled())
	{
		//stops local timer
		GetWorld()->GetTimerManager().ClearTimer(BlockHitTimer);
	}
		//reset back to montage during blocked hit
	//this
	this->GetMesh()->GetAnimInstance()->Montage_Stop(3.0f, MulticastSkill);
	this->GetMesh()->GetAnimInstance()->Montage_Play(RPCMultiCastSkillHold, 2.0f, EMontagePlayReturnType::MontageLength, SkillStopTime, false);
	UE_LOG(LogTemp, Warning, TEXT("Stop block timer!"));	
}

void ATodakBattleArenaCharacter::UpdateCurrentMontage(UAnimMontage* MulticastSkill, FTimerHandle* TimerUsed)
{
	//if the current anim pos is more than .6 seconds, stop the montage and play the hold skill again
	if (GetWorld()->GetTimerManager().IsTimerActive(*TimerUsed) == true)
	{
		ServerStopBlockHitMontage(MulticastSkill);
	}
}

void ATodakBattleArenaCharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedActor, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("BeginOverlap!"));
	if (isCollisionInScript == true)//if the collision is executed in script
	{
		if (OtherActor != this)
		{
			ATodakBattleArenaCharacter* EnemyChar = Cast<ATodakBattleArenaCharacter>(OtherActor);
			if (EnemyChar && isAI == false)
			{
				//Set new target for locking system
				EnemyElement = EnemyChar;
				EnemyElement->CanBeTargeted = true;
				if (EnemyElement->CanBeTargeted == true)
				{
					if (EnemyElement != nullptr)
					{
						//Set target as nearest target available
						NearestTarget = EnemyElement;
						if (NearestTarget != nullptr)
						{
							//Start target lock timer
							GetWorld()->GetTimerManager().SetTimer(ToggleTimer, this, &ATodakBattleArenaCharacter::TriggerToggleLockOn, 0.001f, true);
							//GetWorld()->GetFirstPlayerController()->PlayerCameraManager->
							TargetLocked = true;
						}
					}
				}
			}
			else if (EnemyChar && isAI == true)
			{
				AICanAttack = true;
			}
		}
	}
}

void ATodakBattleArenaCharacter::OnEndOverlap(UPrimitiveComponent* OverlappedActor, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("EndOverlap!"));
	if (isCollisionInScript == true)//if the collision is executed in script
	{
		if (this != OtherActor)
		{
			ATodakBattleArenaCharacter* EnemyChar = Cast<ATodakBattleArenaCharacter>(OtherActor);
			if (EnemyChar && isAI == false)
			{
				//If current overlapped actor leaving the sphere collision, reset everything
				CanBeTargeted = false;
				if (TargetLocked == true)
				{
					if (EnemyChar == NearestTarget)
					{
						if (TargetLocked == true)
						{
							ClosestTargetDistance = 0.0f;
							TargetLocked = false;
						}
						//Stop target lock timer
						GetWorld()->GetTimerManager().ClearTimer(ToggleTimer);
					}
				}
			}
			else if (EnemyChar && isAI == true)
			{
				AICanAttack = false;
			}
		}
	}
}

bool ATodakBattleArenaCharacter::ExecuteAction(bool SkillTrigger, float HitTraceLengths, float AnimRate, float AnimStartTime, UAnimMontage* SkillMovesets, float DealDamage, bool& CDSkill)
{
	if (SkillTrigger == true)
	{
		//Emptying arrays
		SwipeActions.Empty();
		BodyParts.Empty();
		
		//Set all the attribute to the current vars of player
		HitTraceLength = HitTraceLengths;
		this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		//this->GetCharacterMovement()->StopMovementImmediately();
		CDSkill = true;

		//Get the Montage to be play
		SkillMoveset = SkillMovesets;

		//Server
		ServerSkillMoveset(SkillMoveset, DealDamage, AnimRate, AnimStartTime, SkillTriggered);

		if (this->BlockedHit == true)
		{
			this->BlockedHit = false;
		}

		if (BodyParts.IsValidIndex(0) == true)
		{
			BodyParts.Empty();
		}
		if (SwipeActions.IsValidIndex(0) == true)
		{
			SwipeActions.Empty();
		}
	}
	return false;
}



void ATodakBattleArenaCharacter::GetDamageFromPhysicsAssetShapeName(FName ShapeName, float& MajorDamageDealt, float& MinorDamageDealt, bool& IsUpperBody, UAnimMontage* DamageMovesets)
{
	FKBoxElem boxElem;
	FKSphylElem capsuleElem;
	bool isFound = false;

	//if capsule physics asset is valid
	if (GetMesh()->GetBodyInstance(ShapeName)->BodySetup.Get()->AggGeom.SphylElems.IsValidIndex(0))
	{
		//iterate through aggregate physics body
		for (FKSphylElem caps : GetMesh()->GetBodyInstance(ShapeName)->BodySetup.Get()->AggGeom.SphylElems)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s Capsule name is : %s"), *ShapeName.ToString(), *caps.GetName().ToString());
			capsuleElem = caps;
			isFound = true;
			break;
		}

		if (isFound)
		{
			//Used in error reporting
			FString Context;

			FBodyDamage* row = BodyDamageTable->FindRow<FBodyDamage>(ShapeName, Context);

		

			//if capsule elem is physics asset inside body damage datatable
			if (row)
			{
				//return datatable values
				IsUpperBody = row->IsUpperBody;
				if (row->MajorDamageReceived > 0.0f)
				{
					MajorDamageDealt = row->MajorDamageReceived;
				}
				if (row->MinorDamageReceived > 0.0f)
				{
					MinorDamageDealt = row->MinorDamageReceived;
				}
				if (row->DamageMoveset != NULL)
				{
					DamageMovesets = row->DamageMoveset;
				}
				return;
			}
		}
	}

	//else if box physics asset is valid
	else if (GetMesh()->GetBodyInstance(ShapeName)->BodySetup.Get()->AggGeom.BoxElems.IsValidIndex(0))
	{
		//iterate through aggregate physics body
		for (FKBoxElem caps : GetMesh()->GetBodyInstance(ShapeName)->BodySetup.Get()->AggGeom.BoxElems)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s Box name is : %s"), *ShapeName.ToString(), *caps.GetName().ToString());
			boxElem = caps;
			isFound = true;
			break;
		}
		if (isFound)
		{
			//Used in error reporting
			FString Context;

			FBodyDamage* row = BodyDamageTable->FindRow<FBodyDamage>(ShapeName, Context);

			//if box elem is physics asset inside body damage datatable
			if (row)
			{
				//return datatable values
				IsUpperBody = row->IsUpperBody;
				if (row->MajorDamageReceived > 0.0f)
				{
					MajorDamageDealt = row->MajorDamageReceived;
				}
				if (row->MinorDamageReceived > 0.0f)
				{
					MinorDamageDealt = row->MinorDamageReceived;
				}
				if (row->DamageMoveset != NULL)
				{
					DamageMovesets = row->DamageMoveset;
				}

				
				return;

			}
		}
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("%s is not affected by Physic Assets"), *ShapeName.ToString());
}


bool ATodakBattleArenaCharacter::SvrSpawnWounds_Validate(UMaterialInterface * DecalMaterial, USceneComponent * AttachToComponent, FName AttachPointName, FVector Location)
{
	return true;
}

void ATodakBattleArenaCharacter::SvrSpawnWounds_Implementation(UMaterialInterface * DecalMaterial, USceneComponent * AttachToComponent, FName AttachPointName, FVector Location)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		//spawn wounds multicast
		MulticastSpawnWounds(DecalMaterial, AttachToComponent, AttachPointName, Location);
	}

}

bool ATodakBattleArenaCharacter::MulticastSpawnWounds_Validate(UMaterialInterface * DecalMaterial, USceneComponent * AttachToComponent, FName AttachPointName, FVector Location)
{
	return true;
}
void ATodakBattleArenaCharacter::MulticastSpawnWounds_Implementation(UMaterialInterface * DecalMaterial, USceneComponent * AttachToComponent, FName AttachPointName, FVector Location)
{
	//get decal mat apa
	//buat variable current hit loc kt h

	UGameplayStatics::SpawnDecalAttached(DecalMat, FVector(10.0f, 10.0f, 10.0f), this->GetMesh(), BoneName, HitLocation, FRotator(0.0f, 0.0f, 0.0f), EAttachLocation::KeepWorldPosition, 0.0f);
}

void ATodakBattleArenaCharacter::MoveOnHold()
{
	//character movements
	if (SwipeDir == EInputType::Up)
	{
		MoveForward(1.0f);
	}
	if (SwipeDir == EInputType::Down)
	{
		MoveForward(-1.0f);
	}
	if (SwipeDir == EInputType::Right)
	{
		MoveRight(1.0f);
	}
	if (SwipeDir == EInputType::Left)
	{
		MoveRight(-1.0f);
	}
}

void ATodakBattleArenaCharacter::ResetMovementMode()
{
	//reset character movement mode back to walking
	this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	SkillTriggered = false;
}

/***********************************************************************START_STATUS*******************************************************************************************************************/

void ATodakBattleArenaCharacter::IncreaseMaxFitness(float StrengthIncrement, float StaminaIncrement, float AgilityIncrement, float StaminaPercent, float StrengthPercent, float AgilityPercent)
{
	//Get new max fitness status
	MaxStamina = MaxStamina + StaminaIncrement;
	MaxStrength = MaxStrength + StrengthIncrement;
	MaxAgility = MaxAgility + AgilityIncrement;

	//Get fitness new percentages from new max fitness status
	StaminaPercentage = UGestureMathLibrary::CalculatePercentageFromValue(Stamina, MaxStamina, 100.0f) / 100.0f;
	StrengthPercentage = UGestureMathLibrary::CalculatePercentageFromValue(Strength, MaxStrength, 100.0f) / 100.0f;
	AgilityPercentage = UGestureMathLibrary::CalculatePercentageFromValue(Agility, MaxAgility, 100.0f) / 100.0f;

	//Get vitality percentages
	float EnergyPercentages = UGestureMathLibrary::CalculatePercentageFromValue(playerEnergy, MaxEnergy, 100.0f)/100.0f;
	float FatiguePercentages = UGestureMathLibrary::CalculatePercentageFromValue(PlayerFatigue, MaxFatigue, 100.0f)/100.0f;

	//Get new vitality from updated fitness
	TotalVitalityFromFitness(0.7f, 0.2f, 0.1f);

	//get new vitality value based on new max vitality
	playerEnergy = UGestureMathLibrary::CalculateValueFromPercentage(EnergyPercentages, MaxEnergy, 1.0f);
	PlayerFatigue = UGestureMathLibrary::CalculateValueFromPercentage(FatiguePercentages, MaxFatigue, 1.0f);

	EnergyPercentage = EnergyPercentages;
	FatiguePercentage = FatiguePercentages;
}

void ATodakBattleArenaCharacter::ChangeCurrentFitness(EOperation Operations, float StrengthVal, float StaminaVal, float AgilityVal, float PercentageLimit)
{
	if (Operations == EOperation::Addition)
	{
		//Increase value to current fitness level
		StaminaPercentage = UGestureMathLibrary::SetPercentageValue(Stamina, StaminaVal, MaxStamina, EOperation::Addition, PercentageLimit, false, Stamina);
		StrengthPercentage = UGestureMathLibrary::SetPercentageValue(Strength, StrengthVal, MaxStrength, EOperation::Addition, PercentageLimit, false, Strength);
		AgilityPercentage = UGestureMathLibrary::SetPercentageValue(Agility, AgilityVal, MaxAgility, EOperation::Addition, PercentageLimit, false, Agility);
	}
	else if (Operations == EOperation::Subtraction)
	{
		//Decrease value from current fitness level
		StaminaPercentage = UGestureMathLibrary::SetPercentageValue(Stamina, StaminaVal, MaxStamina, EOperation::Subtraction, PercentageLimit, false, Stamina);
		StrengthPercentage = UGestureMathLibrary::SetPercentageValue(Strength, StrengthVal, MaxStrength, EOperation::Subtraction, PercentageLimit, false, Strength);
		AgilityPercentage = UGestureMathLibrary::SetPercentageValue(Agility, AgilityVal, MaxAgility, EOperation::Subtraction, PercentageLimit, false, Agility);
	}
}

void ATodakBattleArenaCharacter::EnergySpent(float ValDecrement, float PercentageLimit)
{
	//Reduce energy from current energy
	float tempEnergy = playerEnergy- ValDecrement;

	if (tempEnergy <= 0.0f)
	{
		playerEnergy = 0.0f;
	}
	else
		playerEnergy = tempEnergy;

	//Update energy after action on progress bar
	EnergyPercentage = UGestureInputsFunctions::UpdateProgressBarComponent(WidgetHUD, "EnergyBar", "EnergyText", "Energy", "Energy", playerEnergy, MaxEnergy);

	/*if (WidgetHUD)
	{
		UE_LOG(LogTemp, Warning, TEXT("Current Client : %s"), Controller);

		const FName locTextControlEnergyName = FName(TEXT("Energy"));
		UTextBlock* locTextControlEnergy = (UTextBlock*)(WidgetHUD->WidgetTree->FindWidget(locTextControlEnergyName));

		const FName locTextControlEnergyPercent = FName(TEXT("EnergyText"));
		UTextBlock* locTextControlEnergyPercentBlock = (UTextBlock*)(WidgetHUD->WidgetTree->FindWidget(locTextControlEnergyPercent));

		const FName locTextControlEnergyBar = FName(TEXT("EnergyBar"));
		UProgressBar* energyBar = (UProgressBar*)(WidgetHUD->WidgetTree->FindWidget(locTextControlEnergyBar));

		if (energyBar != nullptr)
		{
			EnergyPercentage = UGestureMathLibrary::SetProgressBarValue("Energy", energyBar, locTextControlEnergyPercentBlock, locTextControlEnergy, playerEnergy, MaxEnergy);
		}
	}*/

	//UpdateCurrentPlayerMainStatusBar(EBarType::PrimaryProgressBar, EMainPlayerStats::Energy, StartEnergyTimer, StartEnergyTimer);
}

void ATodakBattleArenaCharacter::CheckForAction(FName CurrentAction)
{
	if (SkillNames.IsValidIndex(0) == true)
	{
		if (SkillNames.Find(CurrentAction) == 0 && LevelName == UGameplayStatics::GetCurrentLevelName(this, true))
		{
			this->UpdateTutorialAction(CurrentAction.ToString());
			if (SkillNames.Num() <= 1)
			{
				this->CompleteTutorialAction();
			}
			SkillNames.Remove(CurrentAction);
			return;
		}
	}
}

void ATodakBattleArenaCharacter::FatigueResistanceReduction(float ValDecrement, float PercentageLimit)
{
	//Reduce fatigue resistance reduction from current fatigue level
	UGestureMathLibrary::TotalPercentageReduction(PlayerFatigue, ValDecrement, MaxFatigue, PercentageLimit, PlayerFatigue, FatiguePercentage);
}

void ATodakBattleArenaCharacter::TotalVitalityFromFitness(float StaminaPercent, float StrengthPercent, float AgilityPercent)
{
	//Calculate max energy and fatigue resistance from max fitness level
	MaxEnergy = UGestureMathLibrary::CalculateValueFromPercentage(0.7f, MaxStamina, 1.0f) + UGestureMathLibrary::CalculateValueFromPercentage(0.2f, MaxStrength, 1.0f) + UGestureMathLibrary::CalculateValueFromPercentage(0.1f, MaxAgility, 1.0f);
	playerEnergy = MaxEnergy;

	//Calculate new energy percentage based on its max percentage
	EnergyPercentage = UGestureMathLibrary::CalculatePercentageFromValue(playerEnergy, MaxEnergy, 100.0f) / 100.0f;

	MaxFatigue = MaxEnergy;
	PlayerFatigue = MaxFatigue;

	//Calculate new fatigue resistance based on its max percentage
	FatiguePercentage = UGestureMathLibrary::CalculatePercentageFromValue(PlayerFatigue, MaxFatigue, 100.0f) / 100.0f;
}

void ATodakBattleArenaCharacter::DetectInputTouch(float CurrEnergyValue, ETouchIndex::Type FingerIndex, FVector2D Location, ETouchType::Type Type)
{
	if (Type == ETouchType::Began)
	{
		if (CurrEnergyValue > 0.0f)
		{
			if (FingerIndex == ETouchIndex::Touch1)
			{
				float TempTime;
				EBodyPart GetPart;
				StartDetectSwipe(FingerIndex, Location, TempTime, GetPart);
			}
			if (FingerIndex == ETouchIndex::Touch2)
			{
				float TempTime;
				EBodyPart GetPart;
				StartDetectSwipe(FingerIndex, Location, TempTime, GetPart);
			}
		}
	}
	if (Type == ETouchType::Moved)
	{
		if (FingerIndex == ETouchIndex::Touch1)
		{
			DetectTouchMovement(FingerIndex, Location);
		}
		if (FingerIndex == ETouchIndex::Touch2)
		{
			DetectTouchMovement(FingerIndex, Location);
		}
	}
	if (Type == ETouchType::Ended)
	{
		if (FingerIndex == ETouchIndex::Touch1)
		{
			DetectTouchMovement(FingerIndex, Location);
		}
		if (FingerIndex == ETouchIndex::Touch2)
		{
			DetectTouchMovement(FingerIndex, Location);
		}
	}
}

void ATodakBattleArenaCharacter::UpdateCurrentPlayerMainStatusBar(EBarType Type, EMainPlayerStats StatType, FTimerHandle FirstHandle, FTimerHandle SecondHandle)
{
	//if the widget is exist
	if (WidgetHUD)
	{
		//if the primary and only one progressbar on each stats is exist
		if (Type == EBarType::PrimaryProgressBar)
		{
			//if current progressbar is for pain meter
			if (StatType == EMainPlayerStats::PainMeter)
			{
				//Decrease the current pain meter value
				UpdateStatusValueTimer(FirstHandle, EOperation::Subtraction, false, 1.0f, Health, MaxHealth, 0.0f, Health);

				//Calculate percentage for current pain meter bar
				playerHealth = UGestureInputsFunctions::UpdateProgressBarComponent(WidgetHUD, "HPBar", "Health", "HP", "Pain Meter", Health, MaxHealth);

				//if secondary progressbar value is more than primary progressbar
				if (SecondaryHealth > Health && SecondaryHealth <= MaxHealth)
				{
					//if secondary progressbar timer is not active
					if (GetWorld()->GetTimerManager().IsTimerActive(StartSecondaryHealthTimer) == false)
					{
						FTimerDelegate FunctionsName_1;
						FunctionsName_1 = FTimerDelegate::CreateUObject(this, &ATodakBattleArenaCharacter::UpdateCurrentPlayerMainStatusBar, EBarType::SecondaryProgressBar, EMainPlayerStats::PainMeter, FirstHandle, StartSecondaryHealthTimer);

						//Start the secondary progressbar regen timer
						GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("second timer is started")));
						GetWorld()->GetTimerManager().SetTimer(StartSecondaryHealthTimer, FunctionsName_1, .1f, true);
					}
				}
			}
			else if (StatType == EMainPlayerStats::Energy)
			{
				//Decrease the current pain meter value
				UpdateStatusValueTimer(FirstHandle, EOperation::Addition, false, 10.0f, playerEnergy, MaxEnergy, 0.0f, playerEnergy);

				//Calculate percentage for current pain meter bar
				EnergyPercentage = UGestureInputsFunctions::UpdateProgressBarComponent(WidgetHUD, "EnergyBar", "EnergyText", "Energy", "Energy", playerEnergy, MaxEnergy);
			}
		}
		//If secondary progressbar is exist
		if (Type == EBarType::SecondaryProgressBar)
		{
			//if the current secondary timer is active
			if (GetWorld()->GetTimerManager().IsTimerActive(StartSecondaryHealthTimer) == true)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Timer %s"), (GetWorld()->GetTimerManager().IsTimerActive(StartSecondaryHealthTimer)) ? TEXT("is Active") : TEXT("is not Active")));
				if (StatType == EMainPlayerStats::PainMeter)
				{
					//Decrease the current pain meter value
					UpdateStatusValueTimer(StartSecondaryHealthTimer, EOperation::Subtraction, false, 1.0f, SecondaryHealth, MaxHealth, Health, SecondaryHealth);

					const FName locTextControlHealthBar_1 = FName(TEXT("HPBar_1"));
					UProgressBar* healthBar_1 = (UProgressBar*)(WidgetHUD->WidgetTree->FindWidget(locTextControlHealthBar_1));

					if (healthBar_1)
					{
						playerHealth_1 = UGestureMathLibrary::SetProgressBarValue("", healthBar_1, nullptr, nullptr, SecondaryHealth, MaxHealth);
					}
				}
			}
		}
	}
}

//Update current value based on the timer
void ATodakBattleArenaCharacter::UpdateStatusValueTimer(FTimerHandle newHandle, EOperation Operation, bool StopOnFull, float ChangeVal, float Value, int MaxVal, float MinVal, float& totalVal)
{
	//if the value is increasing
	if (Operation == EOperation::Addition)
	{
		float val = Value + ChangeVal;
		if (val < MaxVal)
		{
			totalVal = val;
		}
		else if (val >= MaxVal)
		{
			totalVal = MaxVal;
			if (StopOnFull == true)
			{
				GetWorld()->GetTimerManager().ClearTimer(newHandle);
			}
		}
		else if (val <= MinVal)
		{
			totalVal = MinVal;
			if (StopOnFull == true)
			{
				GetWorld()->GetTimerManager().ClearTimer(newHandle);
			}
		}
	}

	//if the value is decreasing
	else if (Operation == EOperation::Subtraction)
	{
		float val = Value - ChangeVal;
		if (val > MinVal)
		{
			totalVal = val;
		}
		else if (val <= MinVal)
		{
			totalVal = MinVal;
			if (StopOnFull == true)
			{
				GetWorld()->GetTimerManager().ClearTimer(newHandle);
			}
		}
		else if (val >= MaxVal)
		{
			totalVal = MaxVal;
			if (StopOnFull == true)
			{
				GetWorld()->GetTimerManager().ClearTimer(newHandle);
			}
		}
	}
}

/*
FOnTimelineFloat ATodakBattleArenaCharacter::InterpFunction()
{
	return FOnTimelineFloat();
}

FOnTimelineEvent ATodakBattleArenaCharacter::TimelineFinished()
{
	return FOnTimelineEvent();
}

void ATodakBattleArenaCharacter::TimelineFloatReturn(float value)
{
	SetActorLocation(FMath::Lerp(StartLocation, EndLocation, value));
}

void ATodakBattleArenaCharacter::OnTimelineFinished()
{
	if (RagdollTimeline->GetPlaybackPosition() == 0.0f)
	{
		RagdollTimeline->Play();
	}
	else if (RagdollTimeline->GetPlaybackPosition() == RagdollTimeline->GetTimelineLength())
	{
		RagdollTimeline->Reverse();
	}
}*/

/***********************************************************************END_STATUS*******************************************************************************************************************/

void ATodakBattleArenaCharacter::ReduceDamageTaken(float damageValue)
{
	float minDamage = 30.0f;
	float maxDamage = 50.0f;
	damageValue = damage * (FMath::FRandRange(minDamage, maxDamage))/100;
	damageAfterReduction = damage - damageValue;
}

APlayerController* ATodakBattleArenaCharacter::GetPlayerControllers()
{
	AController* Controllers = Controller;
	if (!Controllers) return NULL;

	APlayerController* PC = Cast<APlayerController>(Controllers);
	if (!PC) return NULL;

	return PC;
}

void ATodakBattleArenaCharacter::RemoveFromArray()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(IterateArray) == false)
	{
		GetWorld()->GetTimerManager().SetTimer(IterateArray, this, &ATodakBattleArenaCharacter::RemoveElementFromArrayTimer, DelaySpeed, true);
	}
}

void ATodakBattleArenaCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ATodakBattleArenaCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		//Jump();
	//StartDetectSwipe(FingerIndex);
}

void ATodakBattleArenaCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		//StopJumping();
	//InputTouchIndex.Remove(FingerIndex);
	//StopTouchTimer(FingerIndex);
}

void ATodakBattleArenaCharacter::MyDoOnce()
{
	if (bDo)
	{
		//Do something
		return;
	}
	else
		return;
}

void ATodakBattleArenaCharacter::RemoveElementFromArrayTimer()
{
	if (SwipeActions.IsValidIndex(0) || BodyParts.IsValidIndex(0) && GetWorld()->GetTimerManager().IsTimerActive(IterateArray))
	{
		//UE_LOG(LogTemp, Warning, TEXT("%s is removed"), *KeyName[0].KeyInput.ToString());
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("%s is removed"), *KeyName[0].KeyInput.ToString()));

		//Remove key at index 0;
		SwipeActions.RemoveAt(0);
		BodyParts.RemoveAt(0);

		//if array is empty and timer still active, clear the timer
		if (GetWorld()->GetTimerManager().IsTimerActive(IterateArray) == true && (!SwipeActions.IsValidIndex(0) || !BodyParts.IsValidIndex(0)))
		{
			UE_LOG(LogTemp, Warning, TEXT("Timer has stopped!"));
			GetWorld()->GetTimerManager().ClearTimer(IterateArray);
		}
	}
}

void ATodakBattleArenaCharacter::ResetMyDoOnce()
{
	bDo = true;
	return;
}

void ATodakBattleArenaCharacter::StartDetectSwipe(ETouchIndex::Type FingerIndex, FVector2D Locations, float& StartPressTime, EBodyPart& SwipeParts)
{
	if (!isAI)
	{
		//Temp var
		FFingerIndex NewIndex;

		NewIndex.StartLocation = Locations;
		NewIndex.IsPressed = true;
		NewIndex.FingerIndex = FingerIndex;
		NewIndex.SwipeActions = EInputType::Pressed;
		NewIndex.bDo = false;

		if (InputTouch.Contains(NewIndex) == false)
		{
			//if current touch index does not exist, add it to array
			InputTouch.Add(NewIndex);

			int Index = InputTouch.Find(NewIndex);
			if (InputTouch[Index].IsPressed == true)
			{
				if (BlockedHit == false)
				{
					BlockedHit = true;
				}

				//Checks for touch within the input area
				UGestureInputsFunctions::CircleSwipeArea(this, &InputTouch[Index], InputTouch[Index].StartLocation);

				//TArray<EBodyPart>& InputPart = BodyParts;

				//Used in error reporting
				FString Context;

				RowNames = ActionTable->GetRowNames();

				//iterate through datatable
				for (auto& name : RowNames)
				{
					FActionSkill* row = ActionTable->FindRow<FActionSkill>(name, Context);
					if (row)
					{
						//Check if the input is same as the input needed to execute the skill
						if (row->BodyParts.Contains(InputTouch[Index].BodyParts))
						{
							SwipeParts = InputTouch[Index].BodyParts;
							GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Touch index is %s"), (*GETENUMSTRING("ETouchIndex", InputTouch[Index].FingerIndex))));
							GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Touch swipeactions is %s"), (*GETENUMSTRING("EInputType", InputTouch[Index].SwipeActions))));
							GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Touch bdo is %s"), (InputTouch[Index].bDo) ? TEXT("True") : TEXT("False")));
							SkillHold = row->StartAnimMontage;
							SkillStopTime = row->StopHoldAnimTime;
							BlockHit = row->SkillBlockHit;

							//play animation on press
							this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
							ServerSkillStartMontage(row->StartAnimMontage);
							//ServerSkillBlockHitMontage(row->SkillBlockHit);
							break;
						}
					}
				}
			}
		}
	}
}

void ATodakBattleArenaCharacter::DetectTouchMovement(ETouchIndex::Type FingerIndex, FVector2D Locations)
{
	if (InputTouch.IsValidIndex(0) == true)
	{
		TArray<FFingerIndex>& Touches = InputTouch;

		for (FFingerIndex& TouchIndex : Touches)
		{
			if (TouchIndex.FingerIndex == FingerIndex)
			{
				if (TouchIndex.IsPressed == true)
				{
					GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, FString::Printf(TEXT("Touch bdo is %s"), (TouchIndex.bDo) ? TEXT("True") : TEXT("False")));

					//Update finger position every tick
					if (TouchIndex.bDo == false)
					{
						//if the current finger position is more than 0 units from starting point
						if ((TouchIndex.StartLocation - Locations).Size() > 50.0f)
						{
							/*if (TouchIndex.FingerIndex == ETouchIndex::Touch2 && (TouchIndex.BodyParts == EBodyPart::LeftFoot || TouchIndex.BodyParts == EBodyPart::RightFoot))
							{
								if ((TouchIndex.StartLocation - Locations).Size() > 50.0f)
								{
									if (UGestureInputsFunctions::DetectLinearSwipe(TouchIndex.StartLocation, Locations, SwipeDir, TouchIndex.bDo, TouchIndex.RightPoints))
									{
										if (EnableMovement == false)
										{
											EnableMovement = true;
											//SwipeDir = TouchIndex.SwipeActions;
											CheckForAction("Move");
											break;
										}
									}
								}
							}
							else*/
							{
								//Checks for touch within the input area
								UGestureInputsFunctions::CircleSwipeArea(this, &TouchIndex, Locations);
								//SwipeDir = TouchIndex.SwipeActions;
								if (EnableMovement == true)
								{
									EnableMovement = false;
								}
								//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, FString::Printf(TEXT("Touch bdo is true")));

								//Get skill combos
								GetSkillAction(&TouchIndex);
								//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Touch index is %s"), (*GETENUMSTRING("ETouchIndex", TouchIndex.FingerIndex))));
								//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Touch swipeactions is %s"), (*GETENUMSTRING("EInputType", TouchIndex.SwipeActions))));
								//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, FString::Printf(TEXT("Current location is %s"), (*Locations.ToString())));
							}
						}
					}
				}
			}
		}
	}
}

void ATodakBattleArenaCharacter::StopDetectTouch(ETouchIndex::Type FingerIndex, float StartPressTime)
{
	if (!isAI)
	{
		bool IsFound = false;

		SwipeDir = EInputType::Pressed;

		EnableMovement = false;
		RightFoot = false;
		LeftFoot = false;

		FFingerIndex NewIndex;
		NewIndex.FingerIndex = FingerIndex;

		if (InputTouch.IsValidIndex(0) == true)
		{
			if (InputTouch.Contains(NewIndex))
			{
				if (BlockedHit == true)
				{
					BlockedHit = false;
				}
				//if touch index is found, remove from array
				int32 Index = InputTouch.Find(NewIndex);
				if (GetMesh()->GetAnimInstance()->Montage_IsActive(SkillHold) == true)
				{
					//Stop current active anim
					ServerSkillMoveset(SkillHold, damage, 1.0f, 0.0f, false);
				}
				InputTouch.RemoveAt(Index);
			}
		}
		if (BodyParts.IsValidIndex(0) == true)
		{
			BodyParts.Empty();
		}
		if (SwipeActions.IsValidIndex(0) == true)
		{
			SwipeActions.Empty();
		}
	}
}

void ATodakBattleArenaCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATodakBattleArenaCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	//AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATodakBattleArenaCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATodakBattleArenaCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ATodakBattleArenaCharacter::StartAttack1()
{
	UE_LOG(LogTemp, Warning, TEXT("We are using our first attack."));
}

void ATodakBattleArenaCharacter::StartAttack2()
{
	UE_LOG(LogTemp, Warning, TEXT("We are using our ssecond attack."));
}

void ATodakBattleArenaCharacter::StartAttack3()
{
	UE_LOG(LogTemp, Warning, TEXT("We are using our third attack."));
}

void ATodakBattleArenaCharacter::StartAttack4()
{
	UE_LOG(LogTemp, Warning, TEXT("We are using our fourth attack."));
}


//void ATodakBattleArenaCharacter::SetAllBodiesBelowSimulatePhysics(const FName & InBoneName, bool bNewSimulate, bool bIncludeSelf)
//{
//}
//
//void ATodakBattleArenaCharacter::SetAllBodiesPhysicsBlendWeight(float PhysicsBlendWeight, bool bSkipCustomPhysicsType)
//{
//}
//
//void ATodakBattleArenaCharacter::SetAllBodiesSimulatePhysics(bool bNewSimulate)
//{
//}

void ATodakBattleArenaCharacter::OnRagdoll(UAnimMontage* GetUpSkill)
{
	DoneRagdoll = false;
	//FName InBoneName;
	//GetWorld()->MakeLiteralname(FName pelvis);
	//FName UKismetSystemLibrary::MakeLiteralName(FName Value);
	//UKismetSystemLibrary::MakeLiteralName('pelvis');

	if (!InRagdoll)
	{
		//set all bodies below simulate physics
		GetMesh()->SetAllBodiesBelowSimulatePhysics(UKismetSystemLibrary::MakeLiteralName("pelvis"), true, true);
		
		PhysicsAlpha = 0.0f;
		InRagdoll = true;
		
	}
	else
	{
		PhysicsAlpha = 1.0f;
		Recovering = true;
		//get anim instace
		RPCMulticastGetUp = GetUpSkill;
		this->GetMesh()->GetAnimInstance()->Montage_Play(RPCMulticastGetUp, 2.0f, EMontagePlayReturnType::MontageLength, 0.0f);
	}
	DoneRagdoll = true;
	//return;
}

bool ATodakBattleArenaCharacter::SvrOnHitRagdoll_Validate()
{
	return true;
}

void ATodakBattleArenaCharacter::SvrOnHitRagdoll_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		//spawn wounds multicast
		MulticastOnHitRagdoll();
	}
}

bool ATodakBattleArenaCharacter::MulticastOnHitRagdoll_Validate()
{
	return true;
}

void ATodakBattleArenaCharacter::MulticastOnHitRagdoll_Implementation()
{
	if (InRagdoll)
	{
		//execute ragdoll function once
		OnRagdoll(GetUpAnim);

		RagdollTimeline->Stop();
		do
		{ 
			BlendWeight = RagdollTimeline->GetPlaybackPosition();
			//execute on ragdoll function once

		} while (!DoneRagdoll);
		//BlendWeight
	}

	else
	{
		RagdollTimeline->PlayFromStart();
	}

	GetMesh()->SetAllBodiesPhysicsBlendWeight(0.0f, false);
	GetMesh()->SetAllBodiesSimulatePhysics(false);
	IsHit = false;
}




