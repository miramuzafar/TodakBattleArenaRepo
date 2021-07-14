// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TodakBattleArenaCharacter.h"
#include "Engine.h"
#include "TodakBattleArenaSaveGame.h"
#include "TodakBattleArenaPlayerController.h"
#include "WidgetFunctionLibrary.h"
//#include "HeadMountedDisplayFunctionLibrary.h"
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
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "TargetLockInterface.h"
#include "Misc/DateTime.h"
#include "DrawDebugHelpers.h"
#include "Engine/DecalActor.h"
#include "GameFramework/Actor.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TimelineComponent.h"
//#include <extensions/PxD6Joint.h>
//#include <PxRigidBody.h>
//#include <PxRigidDynamic.h>
//#include <PxTransform.h>
#include "GestureMathLibrary.h"
#include "Components/ArrowComponent.h"
#include "Math/Rotator.h"
#include "TBAAnimInstance.h"
#include "..\Public\TodakBattleArenaCharacter.h"
#include "Item.h"
#include "Animation/AnimMontage.h"


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

	//Hair
	Hair = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh(), "head"); // Attach the hair to head
	Hair->SetRelativeLocation(FVector(12.0f, -1.999999f, 0.000001f));

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = 50.0f;
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh(), "head");
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->TargetOffset = FVector(0.0f, 0.0f, 20.0f); // The camera gives over the shoulder view
	CameraBoom->bDoCollisionTest = true; // The camera won't collide with world objects
	CameraBoom->ProbeSize = 12.0f;
	CameraBoom->ProbeChannel = ECollisionChannel::ECC_Camera;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->SetRelativeLocationAndRotation(FVector(0.0f, 60.0f, 0.0f), FRotator(-10.0f, 0.0f, 0.0f));
	FollowCamera->ProjectionMode = ECameraProjectionMode::Perspective;
	FollowCamera->SetFieldOfView(75.0f); // Set FOV to 60 degree

	LeftKickCol = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LeftKickCol"));
	LeftKickCol->SetupAttachment(GetMesh(), "calf_l");
	LeftKickCol->SetRelativeLocation(FVector(-15.000000f, 0.000000f, 0.000000f));
	LeftKickCol->SetRelativeRotation(FRotator(180.000000f, 90.000000f, 0.000000f));
	LeftKickCol->SetCapsuleHalfHeight(28);
	LeftKickCol->SetCapsuleRadius(8);
	LeftKickCol->SetRelativeLocation(FVector(-20.000000f, -0.000016f, -0.000000f));
	LeftKickCol->SetRelativeRotation(FRotator(90.000000f, 0.0f, 179.999924f));
	LeftKickCol->SetCapsuleHalfHeight(33);
	LeftKickCol->SetCapsuleRadius(10);
	LeftKickCol->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	RightKickCol = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RightKickCol"));
	RightKickCol->SetupAttachment(GetMesh(), "calf_r");
	RightKickCol->SetRelativeLocation(FVector(15.000000f, 0.000000f, 0.000000f));
	RightKickCol->SetRelativeRotation(FRotator( 180.000000f, 90.000000f, 0.000000f));
	RightKickCol->SetCapsuleHalfHeight(28);
	RightKickCol->SetCapsuleRadius(8);
	RightKickCol->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	LKickArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LKickArrow"));
	LKickArrow->SetupAttachment(LeftKickCol);
	LKickArrow->SetRelativeRotation(FRotator( 0.000000f, -90.000000f, 0.000000f));

	RKickArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RKickArrow"));
	RKickArrow->SetupAttachment(RightKickCol);
	RKickArrow->SetRelativeRotation(FRotator( 0.000000f, 90.000000f, 0.000000f));

	LeftPunchCol = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LeftPunchCol"));
	LeftPunchCol->SetupAttachment(GetMesh(), "hand_l");
	LeftPunchCol->SetRelativeLocation(FVector(0.000000f, 0.000000f, 0.000000f));
	LeftPunchCol->SetRelativeRotation(FRotator(0.000000f, 80.000000f, 0.000000f));
	LeftPunchCol->SetCapsuleHalfHeight(22);
	LeftPunchCol->SetCapsuleRadius(5);
	LeftPunchCol->SetRelativeRotation(FRotator(90.000000f, 0.000000f, 0.000000f));
	LeftPunchCol->SetCapsuleHalfHeight(16);
	LeftPunchCol->SetCapsuleRadius(7);
	LeftPunchCol->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	RightPunchCol = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RightPunchCol"));
	RightPunchCol->SetupAttachment(GetMesh(), "hand_r");
	RightPunchCol->SetRelativeLocation(FVector(0.000000f, 0.000000f, 0.000000f));
	RightPunchCol->SetRelativeRotation(FRotator(0.000000f, 80.000000f, 0.000000f));
	RightPunchCol->SetCapsuleHalfHeight(22);
	RightPunchCol->SetCapsuleRadius(5);
	RightPunchCol->SetRelativeRotation(FRotator(90.000000f, 0.000000f, 0.000000f));
	RightPunchCol->SetCapsuleHalfHeight(16);
	RightPunchCol->SetCapsuleRadius(7);
	RightPunchCol->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	LPunchArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LPunchArrow"));
	LPunchArrow->SetupAttachment(GetMesh(), "lowerarm_l");

	RPunchArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RPunchArrow"));
	RPunchArrow->SetupAttachment(GetMesh(), "lowerarm_r");
	RPunchArrow->SetRelativeRotation(FRotator(0.000157f, -179.999084f, 0.000011f));
	
	
	damageAfterReduction = 0.0f;

	ToBeIgnoredCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("ToBeIgnoredCollision"));
	ToBeIgnoredCollision->SetupAttachment(GetMesh());
	ToBeIgnoredCollision->SetRelativeLocation(FVector(0.000000f, 0.000000f, 100.0f));

	//TargetLockCollision
	LockOnCollision = CreateDefaultSubobject<USphereComponent>(TEXT("LockOnCollision"));
	LockOnCollision->SetupAttachment(RootComponent);
	LockOnCollision->InitSphereRadius(400.0f);

	//WidgetComponent
	W_DamageOutput = CreateDefaultSubobject<UWidgetComponent>(TEXT("W_DamageOutput"));
	W_DamageOutput->SetupAttachment(RootComponent);
	W_DamageOutput->SetRelativeLocation(FVector(0.000000f, 20.000000f, 90.0f));
	W_DamageOutput->InitWidget();

	W_DamageOutput->SetWidgetSpace(EWidgetSpace::Screen);
	W_DamageOutput->SetDrawAtDesiredSize(true);
	W_DamageOutput->SetVisibility(false);
	W_DamageOutput->SetGenerateOverlapEvents(false);

	LockOnCollision->OnComponentBeginOverlap.AddDynamic(this, &ATodakBattleArenaCharacter::OnBeginOverlap);
	LockOnCollision->OnComponentEndOverlap.AddDynamic(this, &ATodakBattleArenaCharacter::OnEndOverlap);

	//Prevent out of sync ragdoll
	//GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = true;
	GetCharacterMovement()->bServerAcceptClientAuthoritativePosition = true;

	//Inventory = CreateDefaultSubobject<UInventoryComponent>("Inventory");
	//Inventory->Capacity = 20;
	
	//check if fCurve is valid
	/*if (fCurve)
	{
		MyTimeline->AddInterpFloat(fCurve, InterpFunction, FName("Alpha"));
		MyTimeline->SetTimelineFinishedFunc(TimelineFinished);

		StartLocation = GetActorLocation();

		EndLocation = FVector(StartLocation.X, StartLocation.Y, StartLocation.Z + ZOffset);

		MyTimeline->SetLooping(false);
		MyTimeline->SetIgnoreTimeDilation(true);

		MyTimeline->Play();

		UE_LOG(LogTemp, Warning, TEXT("Timeline is Created"));
	}*/

	static ConstructorHelpers::FObjectFinder<UCurveFloat> FPPToFar(TEXT("CurveFloat'/Game/ThirdPersonCPP/Timelines/TL_FPPToFar.TL_FPPToFar'"));
	static ConstructorHelpers::FObjectFinder<UCurveFloat> FarToTPP(TEXT("CurveFloat'/Game/ThirdPersonCPP/Timelines/TL_FarToTPP.TL_FarToTPP'"));
	
	

	if (FPPToFar.Object)
	{
		fCurve2 = FPPToFar.Object;
	}

	if (FarToTPP.Object)
	{
		fCurve = FarToTPP.Object;
	}

	FPPToFarTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("FPPToFarTimeline"));
	FarToTPPTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("FarToTPPTimeline"));

	Interp_FPPToFar.BindUFunction(this, FName{ TEXT("FPPToFarFloatReturn") });
	Interp_FarToTPP.BindUFunction(this, FName{ TEXT("FarToTPPFloatReturn") });

}

void ATodakBattleArenaCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATodakBattleArenaCharacter, damage);
	DOREPLIFETIME(ATodakBattleArenaCharacter, staminaDrained);
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
	DOREPLIFETIME(ATodakBattleArenaCharacter, canMove);
	DOREPLIFETIME(ATodakBattleArenaCharacter, ReactionStartTime);

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
	DOREPLIFETIME(ATodakBattleArenaCharacter, IsLocked);
	DOREPLIFETIME(ATodakBattleArenaCharacter, PhysicsAlpha);
	DOREPLIFETIME(ATodakBattleArenaCharacter, SkillTriggered);
	DOREPLIFETIME(ATodakBattleArenaCharacter, RightVal);
	DOREPLIFETIME(ATodakBattleArenaCharacter, IsRotating);
	DOREPLIFETIME(ATodakBattleArenaCharacter, Radius);

	//SwipeGesture
	DOREPLIFETIME(ATodakBattleArenaCharacter, SkillStopTime);
	DOREPLIFETIME(ATodakBattleArenaCharacter, IsHit);
	DOREPLIFETIME(ATodakBattleArenaCharacter, BlockedHit);
	DOREPLIFETIME(ATodakBattleArenaCharacter, AICanAttack);
	DOREPLIFETIME(ATodakBattleArenaCharacter, BlockHitTimer);
	DOREPLIFETIME(ATodakBattleArenaCharacter, RepTurnRight);
	DOREPLIFETIME(ATodakBattleArenaCharacter, RepTurnLeft);
	DOREPLIFETIME(ATodakBattleArenaCharacter, RepIsMoving);
	DOREPLIFETIME(ATodakBattleArenaCharacter, RepLocoPlayrate);
	DOREPLIFETIME(ATodakBattleArenaCharacter, RepIdleAnimToPlay);
	DOREPLIFETIME(ATodakBattleArenaCharacter, RepSwitchSide);
	DOREPLIFETIME(ATodakBattleArenaCharacter, CanSwipeAction);
	//DOREPLIFETIME(ATodakBattleArenaCharacter, JabCounts);


	//**AnimMontage**//
	DOREPLIFETIME(ATodakBattleArenaCharacter, BlockHit);
	DOREPLIFETIME(ATodakBattleArenaCharacter, DoFaceBlock);
	DOREPLIFETIME(ATodakBattleArenaCharacter, IsEffectiveBlock);
	DOREPLIFETIME(ATodakBattleArenaCharacter, SkillMoveset);
	DOREPLIFETIME(ATodakBattleArenaCharacter, HitReactionsMoveset)
	DOREPLIFETIME(ATodakBattleArenaCharacter, SkillHold);
	DOREPLIFETIME(ATodakBattleArenaCharacter, SkillPlayrate);
	//DOREPLIFETIME(ATodakBattleArenaCharacter, SectionName);
	DOREPLIFETIME(ATodakBattleArenaCharacter, RandSection)
	DOREPLIFETIME(ATodakBattleArenaCharacter, SectionLength);


	DOREPLIFETIME(ATodakBattleArenaCharacter, RPCServerBlockHit);
	DOREPLIFETIME(ATodakBattleArenaCharacter, RPCServerSkill);
	DOREPLIFETIME(ATodakBattleArenaCharacter, RPCServerSkillHold);
	DOREPLIFETIME(ATodakBattleArenaCharacter, RPCServerBlockReaction);

	DOREPLIFETIME(ATodakBattleArenaCharacter, RPCMultiCastBlockHit);
	DOREPLIFETIME(ATodakBattleArenaCharacter, RPCMultiCastSkill);
	DOREPLIFETIME(ATodakBattleArenaCharacter, RPCMultiCastSkillHold);
	DOREPLIFETIME(ATodakBattleArenaCharacter, EffectiveBlockAttacker);
	DOREPLIFETIME(ATodakBattleArenaCharacter, RPCMultiCastBlockReaction);

	DOREPLIFETIME(ATodakBattleArenaCharacter, RepWalkSpeed);

	//FallDown
	DOREPLIFETIME(ATodakBattleArenaCharacter, FallBackAnimChar);
	DOREPLIFETIME(ATodakBattleArenaCharacter, FallFrontAnimChar);

	//GetUpFromFall
	DOREPLIFETIME(ATodakBattleArenaCharacter, RPCMulticastGetUp);
	DOREPLIFETIME(ATodakBattleArenaCharacter, RPCServerGetUp);
	DOREPLIFETIME(ATodakBattleArenaCharacter, GetUpAnim);
	DOREPLIFETIME(ATodakBattleArenaCharacter, GetUp);
	//DOREPLIFETIME(ATodakBattleArenaCharacter, fCurve);
	DOREPLIFETIME(ATodakBattleArenaCharacter, DoneRagdoll);
	DOREPLIFETIME(ATodakBattleArenaCharacter, bDo);
	DOREPLIFETIME(ATodakBattleArenaCharacter, IsFacingUp);
	DOREPLIFETIME(ATodakBattleArenaCharacter, CapsuleLocation);
	DOREPLIFETIME(ATodakBattleArenaCharacter, MeshLocation);
	DOREPLIFETIME(ATodakBattleArenaCharacter, MeshRotation);

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

	DOREPLIFETIME(ATodakBattleArenaCharacter, WidgetHUD);

	
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
		if (EnemyElement != nullptr)
		{
			//Restrict movement within target lock area and avoid players from overlapping

		//Determine the target position
			const FVector targetPos = EnemyElement->GetMesh()->GetComponentLocation();
			float Dist = this->GetDistanceTo(EnemyElement);

			//if distance between players are smaller than the accepted radius
			if (Dist < Radius)
			{
				//UE_LOG(LogTemp, Warning, TEXT("You are within the effective range"));

				//Get Vector from player minus target
				FVector FromOriginToTarget = this->GetActorLocation() - this->EnemyElement->GetActorLocation();

				//Multiply by Radius and divided by distance
				FromOriginToTarget *= Radius / Dist;

				//Locate player position based of the radius size
				this->SetActorLocation(this->EnemyElement->GetActorLocation() + FromOriginToTarget);
				// Set camera to fight

				//EnemyElement->CameraBoom->SetRelativeLocation(FVector((198.000000f, 194.000000f, 50.000000f)));
				//EnemyElement->CameraBoom->RelativeRotation += (FRotator((0.000000f, -50.000000f, 0.000000f)));
				//this->FollowCamera->SetRelativeRotation(FRotator(((0.000000f, -50.000069f, 0.000000f))));
			}
			NewController->SetControlRotation(NewRotator);
		}
	}

	if (this->EnemyElement != nullptr)
	{
		if (this->RightVal == 0.0f || this->GetCharacterMovement()->Velocity.Size() == 0.0f)
		{
			UTBAAnimInstance* AnimInst = Cast<UTBAAnimInstance>(this->GetMesh()->GetAnimInstance());

			//enemy forward vector
			FVector FWEnem = UKismetMathLibrary::GetRightVector(this->EnemyElement->GetActorRotation());

			//velocity of Enemy
			FVector VEnem = this->EnemyElement->GetCharacterMovement()->Velocity;
			VEnem.Normalize();

			//get dot product
			UKismetMathLibrary::Dot_VectorVector(FWEnem, VEnem);

			if (UKismetMathLibrary::Dot_VectorVector(FWEnem, VEnem) > 0.0f)
			{
				if (AnimInst->TurnRight == true)
				{
					AnimInst->TurnRight = false;
				}

				if (AnimInst->TurnLeft == false)
				{
					AnimInst->TurnLeft = true;
				}

			}
			else if (UKismetMathLibrary::Dot_VectorVector(FWEnem, VEnem) < 0.0f)
			{
				if (AnimInst->TurnLeft == true)
				{
					AnimInst->TurnLeft = false;
				}

				if (AnimInst->TurnRight == false)
				{
					AnimInst->TurnRight = true;
				}

			}
			else
			{
				AnimInst->TurnRight = false;
				AnimInst->TurnLeft = false;

			}
		}
		if (this->EnemyElement->RightVal == 0.0f || this->EnemyElement->GetCharacterMovement()->Velocity.Size() == 0.0f)
		{
			UTBAAnimInstance* AnimInst = Cast<UTBAAnimInstance>(this->EnemyElement->GetMesh()->GetAnimInstance());
			//enemy forward vector
			FVector FWEnem = UKismetMathLibrary::GetRightVector(this->GetActorRotation());

			//velocity of Enemy
			FVector VEnem = this->GetCharacterMovement()->Velocity;
			VEnem.Normalize();
			//get dot product`
			UKismetMathLibrary::Dot_VectorVector(FWEnem, VEnem);

			if (UKismetMathLibrary::Dot_VectorVector(FWEnem, VEnem) > 0.0f)
			{
				if (AnimInst->TurnRight == true)
				{
					AnimInst->TurnRight = false;
				}

				if (AnimInst->TurnLeft == false)
				{
					AnimInst->TurnLeft = true;
				}

				/*if (this->IsLocallyControlled())
				{
					ServerTurnAnim(this, true, false);
					AnimInst->TurnRight = false;
					AnimInst->TurnLeft = true;
					UE_LOG(LogTemp, Warning, TEXT("left"));
				}*/
			}
			else if (UKismetMathLibrary::Dot_VectorVector(FWEnem, VEnem) < 0.0f)
			{
				if (AnimInst->TurnLeft == true)
				{
					AnimInst->TurnLeft = false;
				}

				if (AnimInst->TurnRight == false)
				{
					AnimInst->TurnRight = true;
				}

				//if (this->IsLocallyControlled())
				//{
				//	ServerTurnAnim(this, false, true);
				//	//AnimInst->TurnLeft = false;
				//	//AnimInst->TurnRight = true;
				//	UE_LOG(LogTemp, Warning, TEXT("right"));
				//}
			}
			else
			{
				AnimInst->TurnRight = false;
				AnimInst->TurnLeft = false;
				/*if (this->IsLocallyControlled())
				{
					ServerTurnAnim(this, false, false);
					AnimInst->TurnRight = false;
					AnimInst->TurnLeft = false;
					UE_LOG(LogTemp, Warning, TEXT("out"));
				}*/
			}
		}
		else
		{
			/*UTBAAnimInstance* AnimInst = Cast<UTBAAnimInstance>(this->GetMesh()->GetAnimInstance());
			AnimInst->TurnRight = false;
			AnimInst->TurnLeft = false;
			UTBAAnimInstance* AnimEnemInst = Cast<UTBAAnimInstance>(EnemyElement->GetMesh()->GetAnimInstance());
			AnimEnemInst->TurnRight = false;
			AnimEnemInst->TurnLeft = false;*/
		}
	}
	
}

bool ATodakBattleArenaCharacter::ServerTurnAnim_Validate(AActor* thisActor, float TurnLeft, float TurnRight)
{
	if (this == thisActor)
	{
		return true;
	}
	return false;
}

void ATodakBattleArenaCharacter::ServerTurnAnim_Implementation(AActor* thisActor, float TurnLeft, float TurnRight)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		MulticastTurnAnim(thisActor, TurnLeft, TurnRight);
	}
}

bool ATodakBattleArenaCharacter::MulticastTurnAnim_Validate(AActor* thisActor, float TurnLeft, float TurnRight)
{
	return true;
}

void ATodakBattleArenaCharacter::MulticastTurnAnim_Implementation(AActor* thisActor, float TurnLeft, float TurnRight)
{
	if (thisActor == this)
	{
		UTBAAnimInstance* AnimInst = Cast<UTBAAnimInstance>(this->GetMesh()->GetAnimInstance());
		if (AnimInst != nullptr)
		{
			AnimInst->TurnLeft = TurnLeft;
			AnimInst->TurnRight = TurnRight;
		}
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
			FRotator InterpVal = UKismetMathLibrary::RInterpTo(this->Controller->GetControlRotation(), target, 0.01f, 5.0f);

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

	playerController = Cast<APlayerController>(Controller);
	/*if (this->IsLocallyControlled() == true)
	{
		this->WidgetHUD = CreateWidget<UBaseCharacterWidget>(GetWorld(), CharacterHUD);
		this->WidgetHUD->AddToViewport();
		if (this->WidgetHUD)
		{
			InitializeCharAtt();
		}
	}*/

	FollowCamera->SetRelativeLocationAndRotation(FVector(0.0f, 60.0f, 0.0f), FRotator(-10.0f, 0.0f, 0.0f));
	FollowCamera->SetFieldOfView(75.0f); // Set FOV to 75 degree

	/*
	if (CameraPerspective == 0)
	{
		//Sets player camera nearer TPP
		this->FollowCamera->SetFieldOfView(90.0f);
		FLatentActionInfo LatentInfo = FLatentActionInfo();
		LatentInfo.CallbackTarget = this;
		UKismetSystemLibrary::MoveComponentTo(this->FollowCamera, FVector(225.0f, 226.0f, -60.0f), FRotator(0.0f, -55.0f, 0.0f), true, true, 3.0f, true, EMoveComponentAction::Type::Move, LatentInfo);

	}*/

	this->GetCharacterMovement()->MaxWalkSpeed = RepWalkSpeed;
	/*FStringClassReference locWidgetClassRef(TEXT("/Game/Blueprints/CharacterHUD.CharacterHUD_C"));
	if (UClass* locWidgetClass = locWidgetClassRef.TryLoadClass<UBaseCharacterWidget>())
	{
		if (UGameplayStatics::GetPlayerController(this, 0)->IsLocalPlayerController())
		{
			//WidgetHUD = CreateWidget<UBaseCharacterWidget>(this, locWidgetClass);
			WidgetHUD = CreateWidget<UBaseCharacterWidget>(this->GetGameInstance(), locWidgetClass);
			if (WidgetHUD)
			{
				WidgetHUD->AddToViewport();
				InitializeCharAtt();
				//locUIChar->WidgetHUD = CharacterHUDClass;
				//GetWorld()->GetFirstPlayerController()->ShouldShowMouseCursor();
				//UE_LOG(LogTemp, Warning, TEXT("Show mouse cursor is %s "), (GetWorld()->GetFirstPlayerController()->ShouldShowMouseCursor() == GetWorld()->GetFirstPlayerController()->ShouldShowMouseCursor()) ? TEXT("True") : TEXT("False"));
				//GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
			}
		}
	}*/

	//InitializeCharAtt();
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
	//if hold montage is active, prepare for blocking hit
	if (isAI == false && this->GetMesh()->GetAnimInstance()->Montage_IsActive(RPCMultiCastBlockHit) && this->GetMesh()->GetAnimInstance()->Montage_GetPosition(RPCMultiCastBlockHit) >= this->SkillStopTime && this->BlockedHit == true)// && EnableMovement == false
	{
		//UE_LOG(LogTemp, Warning, TEXT("Pause anim "));
		this->GetMesh()->GetAnimInstance()->Montage_Pause(RPCMultiCastBlockHit);
		//UE_LOG(LogTemp, Warning, TEXT("Pause anim : %s "), *RPCMultiCastBlockHit);
	}

	//Detect hold touch input//
	/*if (isAI == false && TouchIsHold == true)
	{
		FTimespan currTimeSpan = UGestureMathLibrary::GetCurrentTime();

		UE_LOG(LogTemp, Warning, TEXT("Touch is hold "));
		if (currTimeSpan.GetTotalSeconds() - startTouch >= 0.1f)
		{
			//TouchIsHold = false;
			FVector2D currTouchLoc;
			//TouchIsHold = false;
			
			playerController->GetInputTouchState(ETouchIndex::Touch1, currTouchLoc.X, currTouchLoc.Y, IsPressed);
			StopDetectTouch(CurrFingerIndex->FingerIndex, currTimeSpan.GetTotalSeconds(), CurrFingerIndex->StartLocation);
			UE_LOG(LogTemp, Warning, TEXT("Hold is ended "));
			
		}
	}*/
	/*if (NearestTarget != nullptr && TargetLocked == true)
	{
		TriggerToggleLockOn();
	}*/
	
	//CheckTraces(HitActor, BoneNames, Location, bBlockingHits);
}

void ATodakBattleArenaCharacter::OnRep_Block()
{
	if (!this->IsLocallyControlled())
	{
		this->CallOpenBlockFunction();
	}
}

//////////////////////////////////// Input //////////////////////////////////////// 
void ATodakBattleArenaCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//check(PlayerInputComponent);
	/*PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);*/

	PlayerInputComponent->BindAxis("MoveForward", this, &ATodakBattleArenaCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATodakBattleArenaCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	/*PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATodakBattleArenaCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATodakBattleArenaCharacter::LookUpAtRate);*/

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ATodakBattleArenaCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ATodakBattleArenaCharacter::TouchStopped);

	// VR headset functionality
	/*PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ATodakBattleArenaCharacter::OnResetVR);*/
}

void ATodakBattleArenaCharacter::CalculateMeshLocation(USceneComponent* Capsule, FVector& FinalLoc)
{
	FVector loc;
	FRotator Rot;
	FHitResult HitRay;

	FVector Start = GetMesh()->GetSocketLocation("pelvis");
	FVector End = Start + FVector(FVector(0, 0, 1.0f)*100.0f);

	FCollisionQueryParams TraceParams;

	//Shoot line from pelvis location to the ground where it's standing
	if (GetWorld()->LineTraceSingleByChannel(HitRay, Start, End, ECollisionChannel::ECC_Visibility, TraceParams))
	{
		//get the new location of the mesh
		CapsuleLocation = HitRay.Location + FVector(0.0f, 0.0f, 100.0f);
	}
	else
		//set the location to original location of the mesh
		CapsuleLocation = GetMesh()->GetSocketLocation("pelvis") + FVector(0.0f, 0.0f, 100.0f);


	MeshLocation = FMath::VInterpTo(MeshLocation, CapsuleLocation, GetWorld()->GetDeltaSeconds(), 30.0f);
	this->GetCapsuleComponent()->SetWorldLocation(MeshLocation, false, nullptr, ETeleportType::None);
	//this->GetCapsuleComponent()->SetWorldRotation(MeshRotation, true, false);
	FinalLoc = MeshLocation;
	//this->OnRep_SetMeshLocation();
	//MeshLocation = FMath::VInterpTo(MeshLocation, CapsuleLocation, GetWorld()->GetDeltaSeconds(), 30.0f);
	//Capsule->SetWorldLocation(MeshLocation, false, false);
	//Interpolate between mesh location and capsule location
	/*if (Capsule == this->GetCapsuleComponent())
	{
		MeshLocation = FMath::VInterpTo(MeshLocation, CapsuleLocation, GetWorld()->GetDeltaSeconds(), 30.0f);
		ServerGetMeshLocation(MeshLocation);
	}*/

	/*if (this->IsLocallyControlled())
	{
		ServerGetMeshLocation(MeshLocation);
	}*/

	//this->OnRep_SetMeshRotation();
}

bool ATodakBattleArenaCharacter::ServerGetMeshLocation_Validate(FVector TempMeshLoc)
{
	return true;
}

void ATodakBattleArenaCharacter::ServerGetMeshLocation_Implementation(FVector TempMeshLoc)
{
	MulticastGetMeshLocation(TempMeshLoc);
}

bool ATodakBattleArenaCharacter::MulticastGetMeshLocation_Validate(FVector TempMeshLoc)
{
	return true;
}

void ATodakBattleArenaCharacter::MulticastGetMeshLocation_Implementation(FVector TempMeshLoc)
{
	FHitResult newResult;

	this->GetCapsuleComponent()->SetWorldLocation(MeshLocation, false, &newResult, ETeleportType::None);
	this->OnRep_SetMeshLocation();
}

void ATodakBattleArenaCharacter::OnRep_SetMeshLocation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		//this->GetCapsuleComponent()->SetWorldLocation(MeshLocation, false, false);
	}
	//MeshLocation = FMath::VInterpTo(MeshLocation, CapsuleLocation, GetWorld()->GetDeltaSeconds(), 30.0f);
	//this->GetCapsuleComponent()->SetWorldLocation(MeshLocation, false, false);
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Orange, FString::Printf(TEXT("Mesh location : %s"), *MeshLocation.ToString()));
}

void ATodakBattleArenaCharacter::OnRep_SetMeshRotation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		//this->GetCapsuleComponent()->SetWorldRotation(MeshRotation, false, false);
	}
}

bool ATodakBattleArenaCharacter::CalculatingFacingLocation(USkeletalMeshComponent* currMesh)
{
	//Get the dot product between the mesh right vector location and right vector location
	//float DotProductResult = UKismetMathLibrary::Dot_VectorVector(UKismetMathLibrary::GetRightVector(currMesh->GetSocketRotation("pelvis")), FVector(0.0f, 0.0f, 1.0f));

	//UE_LOG(LogTemp, Warning, TEXT("DotProduct from c++ is :  : %f"), DotProduct);

	FTransform BoneTransform = UGestureMathLibrary::GetBoneTransform(currMesh, "pelvis");

	//Get the dot product between the mesh right vector location and right vector location
	float val = UKismetMathLibrary::Dot_VectorVector(UKismetMathLibrary::GetRightVector(BoneTransform.Rotator()), FVector(0.0f, 0.0f, 1.0f));

	UE_LOG(LogTemp, Warning, TEXT("DotProduct from c++ is :  : %f"), val);

	//Compare the dot product with the z value, which will be the facing up montage if its more than zero
	return val >= 0.0f;
}

void ATodakBattleArenaCharacter::SetUpGetUpOrientation(USkeletalMeshComponent* currMesh)
{
	FVector TotalVector;
	FRotator NewRot;

	if (IsFacingUp == true)
	{
		TotalVector = currMesh->GetSocketLocation("pelvis") - currMesh->GetSocketLocation("neck_01");
		NewRot = UKismetMathLibrary::MakeRotFromZX(FVector(0.0f, 0.0f, 1.0f), TotalVector);
	}
	if (IsFacingUp == false)
	{
		TotalVector = currMesh->GetSocketLocation("neck_01") - GetMesh()->GetSocketLocation("pelvis");
		NewRot = UKismetMathLibrary::MakeRotFromZX(FVector(0.0f, 0.0f, 1.0f), TotalVector);
	}

	if (this->SetActorTransform(FTransform(NewRot, MeshLocation, FVector(1.0f, 1.0f, 1.0f)), false, nullptr, ETeleportType::None))
	{
		//Transform succesful
	}
}

void ATodakBattleArenaCharacter::SetUpGetUpMontage(USkeletalMeshComponent* currMesh, bool FacingUp)
{
	if (currMesh != nullptr)
	{
		if (FacingUp == true)
		{
			RPCMulticastGetUp = UpMontage;
		}
		else
			RPCMulticastGetUp = DownMontage;
	}
}

void ATodakBattleArenaCharacter::GetSkillAction(FFingerIndex* FingerIndex)
{
	//Used in error reporting
	FString Context;
	//float temp = GetMesh()->GetAnimInstance()->LocoPlayrate;
	//UTBAAnimInstance* AnimInstance = Cast<UTBAAnimInstance>(GetMesh()->GetAnimInstance());

	//UTBAAnimInstance* UAnimInstance = Cast<UTBAAnimInstance>(GetMesh()->GetAnimInstance());
	//UAnimInstance->LocoPlayrate;

	if (InputStyle != EInputStyle::Button)
	{
		//Search the skill available
		for (auto& name : ActionTable->GetRowNames())
		{
			FActionSkill* row = ActionTable->FindRow<FActionSkill>(name, Context);

			if (row)
			{
				
				UE_LOG(LogTemp, Log, TEXT("Touch swipeactions is %s"), (*GETENUMSTRING("EInputType", FingerIndex->SwipeActions)));
				if (row->SwipeActions==FingerIndex->SwipeActions)
				{
					GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Touch index is %s"), (*GETENUMSTRING("ETouchIndex", FingerIndex->FingerIndex))));
					GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Touch swipeactions is %s"), (*GETENUMSTRING("EInputType", FingerIndex->SwipeActions))));
					//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Orange, FString::Printf(TEXT("Equal : %s"), areEqual(row->SwipeActions, InputType, row->SwipeActions.Num(), InputType.Num()) && areEqual(row->BodyParts, InputPart, row->BodyParts.Num(), InputPart.Num()) ? TEXT("True") : TEXT("False")));
					//row->SkillTrigger = true;
					//SkillTriggered = row->SkillTrigger;

					//Execute skill if cooldown is finished
					if (row->CDSkill == false)
					{
						GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Touch index is %s"), (*GETENUMSTRING("ETouchIndex", FingerIndex->FingerIndex))));
						GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Touch swipeactions is %s"), (*GETENUMSTRING("EInputType", FingerIndex->SwipeActions))));
						//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Orange, FString::Printf(TEXT("Equal : %s"), areEqual(row->SwipeActions, InputType, row->SwipeActions.Num(), InputType.Num()) && areEqual(row->BodyParts, InputPart, row->BodyParts.Num(), InputPart.Num()) ? TEXT("True") : TEXT("False")));
						//row->SkillTrigger = true;
						//SkillTriggered = row->SkillTrigger;
						//row->SkillMoveSetRate = SkillPlayrate;
						//RepLocoPlayrate = SkillPlayrate;
						//AnimInstance->LocoPlayrate = SkillPlayrate;
						//temp = SkillPlayrate;

						

						//checks if there is skill triggering and player has enough energy
						if (SkillTriggered == false && (this->playerEnergy >= row->StaminaUsage))
						{
							SkillTriggered = true;

							//checks if there is a player in target lock radius
							if (EnemyElement != nullptr)
							{
								// checks if enemy is blocking the hit
								if (this->EnemyElement->BlockedHit == true)
								{
									if (this->EnemyElement->IsEffectiveBlock == true)
									{
										row->CDSkill = ExecuteAction(SkillTriggered, row->SkillMoveSetRate, row->SkillMovesetTime, row->SkillMoveset, row->HitReactionMoveset, row->BlockActionMoveset, row->CriticalDamage, row->StaminaUsage, row->StaminaDrain, row->CDSkill);
										SkillPlayrate = row->SkillMoveSetRate;

										if (this->IsLocallyControlled())
										{
											ServerSetEnemyMontage(row->CriticalReactionMoveset, row->BlockActionMoveset, row->StaminaDrain, row->StartCritical, row->BlockButtonLength);
										}
									}
									else
									{
										row->CDSkill = ExecuteAction(SkillTriggered, row->SkillMoveSetRate, row->SkillMovesetTime, row->SkillMoveset, row->BlockReactionMoveset, row->BlockActionMoveset, row->Damage, row->StaminaUsage, row->StaminaDrain, row->CDSkill);
										SkillPlayrate = row->SkillMoveSetRate;

										if (this->IsLocallyControlled())
										{
											ServerSetEnemyMontage(row->BlockReactionMoveset, row->BlockActionMoveset, row->StaminaDrain, row->StartBlock, row->BlockButtonLength);
										}
									}
								}

								// check if enemy is not blocking
								else
								{
									row->CDSkill = ExecuteAction(SkillTriggered, row->SkillMoveSetRate, row->SkillMovesetTime, row->SkillMoveset, row->HitReactionMoveset, row->BlockActionMoveset, row->Damage, row->StaminaUsage, row->StaminaDrain, row->CDSkill);
									SkillPlayrate = row->SkillMoveSetRate;

									if (this->IsLocallyControlled())
									{
										ServerSetEnemyMontage(row->HitReactionMoveset, row->BlockActionMoveset, row->StaminaDrain, row->StartHit, row->BlockButtonLength);
									}
								}
							}

							// checks if there is no player in target lock radius
							else
							{
								row->CDSkill = ExecuteAction(SkillTriggered, row->SkillMoveSetRate, row->SkillMovesetTime, row->SkillMoveset, row->HitReactionMoveset, row->BlockActionMoveset, row->Damage, row->StaminaUsage, row->StaminaDrain, row->CDSkill);
								SkillPlayrate = row->SkillMoveSetRate;
							}
						}
					
						FingerIndex->bDo = true;
						CheckForAction(name);
						break;
					}


					// if player doesn't have enough stamina to execute an action
					else if (SkillTriggered == false && (this->playerEnergy < row->StaminaUsage))
					{
						SkillTriggered = true;

						//checks if there is a player in target lock radius
						if (EnemyElement != nullptr)
						{
							// checks if enemy is blocking the hit
							if (this->EnemyElement->BlockedHit == true)
							{
								if (this->EnemyElement->IsEffectiveBlock == true)
								{
									row->CDSkill = ExecuteAction(SkillTriggered, row->SkillMoveSetRate, row->SkillMovesetTime, row->SkillMoveset, row->HitReactionMoveset, row->BlockActionMoveset, row->CriticalDamage, row->StaminaUsage, row->StaminaDrain, row->CDSkill);
									SkillPlayrate = row->SkillMoveSetRate;

									if (this->IsLocallyControlled())
									{
										ServerSetEnemyMontage(row->CriticalReactionMoveset, row->BlockActionMoveset, row->StaminaDrain, row->StartCritical, row->BlockButtonLength);
									}
								}
								else
								{
									row->CDSkill = ExecuteAction(SkillTriggered, row->SkillMoveSetRate, row->SkillMovesetTime, row->SkillMoveset, row->HitReactionMoveset, row->BlockActionMoveset, row->FatigueDamage, row->StaminaUsage, row->StaminaDrain, row->CDSkill);
									SkillPlayrate = row->SkillMoveSetRate;

									if (this->IsLocallyControlled())
									{
										ServerSetEnemyMontage(row->BlockReactionMoveset, row->BlockActionMoveset, row->StaminaDrain, row->StartBlock, row->BlockButtonLength);
									}
								}
							}

							// checks if enemy is not blocking
							else
							{
								row->CDSkill = ExecuteAction(SkillTriggered, row->SkillMoveSetRate, row->SkillMovesetTime, row->SkillMoveset, row->HitReactionMoveset, row->BlockActionMoveset, row->FatigueDamage, row->StaminaUsage, row->StaminaDrain, row->CDSkill);
								SkillPlayrate = row->SkillMoveSetRate;

								if (this->IsLocallyControlled())
								{
									ServerSetEnemyMontage(row->HitReactionMoveset, row->BlockActionMoveset, row->StaminaDrain, row->StartHit, row->BlockButtonLength);
								}
							}
						}

						//checks if there is no player in target lock radius
						else
						{
							row->CDSkill = ExecuteAction(SkillTriggered, row->SkillMoveSetRate, row->SkillMovesetTime, row->SkillMoveset, row->HitReactionMoveset, row->BlockActionMoveset, row->Damage, row->StaminaUsage, row->StaminaDrain, row->CDSkill);
							SkillPlayrate = row->SkillMoveSetRate;
						}
							
						FingerIndex->bDo = true;
						CheckForAction(name);
						break;
					}

				}
			}
		}
	}

}

void ATodakBattleArenaCharacter::GetButtonSkillAction(FName BodyPart, bool IsReleased)
{
	if (!isAI)
	{
		//Used in error reporting
		FString Context;

		if (IsReleased == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("Swipe Detect"));

			if (BlockedHit == false)
			{
				BlockedHit = true;
			}

			FActionSkill* row = ActionTable->FindRow<FActionSkill>(BodyPart, Context);
			if (row)
			{
				//Get random index from section names
				/*FName arr[3] = { "Attack1", "Attack2", "Attack3" };
				RandSection = rand() % 3;
				SectionName = arr[RandSection];*/
				//int random = rand() % 3;

				//SkillHold = row->StartAnimMontage;

				//if current row->StopHoldAnimTime is not empty
				/*if (row->StopHoldAnimTime > 0)
				{
					SkillStopTime = row->StopHoldAnimTime;
					BlockHit = row->SkillBlockHit;
				}*/

				//play animation on press
				this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
				//canMove = false; causes delay input after actionskill montage is played

				if (IsLocallyControlled())
				{
					//ServerSkillStartMontage(SkillHold, row->StartHoldMontageTime, SkillStopTime);
				}
			}
		}
		else if (IsReleased == true)
		{
			FActionSkill* row = ActionTable->FindRow<FActionSkill>(BodyPart, Context);
			if (row)
			{
				row->SkillTrigger = true;
				SkillTriggered = row->SkillTrigger;

				//Execute skill if cooldown is finished
				if (row->CDSkill == false)
				{
					row->SkillTrigger = true;
					SkillTriggered = row->SkillTrigger;
					row->SkillMoveSetRate = SkillPlayrate;
					//RepLocoPlayrate = SkillPlayrate;
					//AnimInstance->LocoPlayrate = SkillPlayrate;
					//temp = SkillPlayrate;
					row->CDSkill = ExecuteAction(row->SkillTrigger, 1.0f, row->SkillMoveSetRate, row->SkillMoveset, row->HitReactionMoveset, row->BlockActionMoveset, row->Damage, row->StaminaUsage, row->StaminaDrain, row->CDSkill);
					/*if (row->StartSwipeMontageTime.Num() > 0)
					{
						row->CDSkill = ExecuteAction(row->SkillTrigger, 1.0f, row->SkillMoveSetRate, row->SkillMoveset, row->Damage, row->CDSkill);
						//row->CDSkill = ExecuteAction(row->SkillTrigger, row->HitTraceLength, row->SkillMoveSetRate, row->StartSwipeMontageTime[RandSection], row->SkillMoveset, row->Damage, row->CDSkill);
					}*/
					CheckForAction(BodyPart);
					if (SkillTriggered == false)
					{
						row->SkillTrigger = false;
					}
				}
			}
		}
	}
}

bool ATodakBattleArenaCharacter::HitEnemyPlayer_Validate(ATodakBattleArenaCharacter* Player, ATodakBattleArenaCharacter* Enemy)
{
	return true;
}

void ATodakBattleArenaCharacter::HitEnemyPlayer_Implementation(ATodakBattleArenaCharacter* Player, ATodakBattleArenaCharacter* Enemy)
{
	if (Player == this && Player != nullptr)
	{
		if (Enemy != nullptr)
		{
			float Dist = this->GetDistanceTo(Enemy);

			if (Dist <= (Radius + 10.0f))
			{
				CameraShake();
				this->DoDamage(Enemy);
				Enemy->HitReactionsMoveset = this->HitReactionsMoveset;
			}
		}
	}

	
}

bool ATodakBattleArenaCharacter::FireTrace_Validate(FVector StartPoint, FVector EndPoint)
{
	return true;
}

void ATodakBattleArenaCharacter::FireTrace_Implementation(FVector StartPoint, FVector EndPoint)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Enter Fire Trace")));
	//Hit result storage
	FHitResult HitRes;

	//DrawDebugSphere(this->GetWorld(), StartPoint, 20.0f, 5, FColor::Purple, false , 1, 0, 1);

	//Ignore self upon colliding
	FCollisionQueryParams CP_LKick;
	CP_LKick.AddIgnoredActor(this);

	//Sphere trace by channel
	bool DetectHit = this->GetWorld()->SweepSingleByChannel(HitRes, StartPoint, EndPoint, FQuat(), ECollisionChannel::ECC_PhysicsBody, FCollisionShape::MakeSphere(20.0f), CP_LKick);

	if (DetectHit)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString::Printf(TEXT("Sweep channel")));
		if (HitRes.Actor != this)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, FString::Printf(TEXT("Is not self")));
			ATodakBattleArenaCharacter* hitChar = Cast<ATodakBattleArenaCharacter>(HitRes.Actor);
			if (hitChar && hitChar->InRagdoll == false)
			{
				if (DoOnce == false)
				{
					//Apply damage
					DoOnce = true;
					//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, FString::Printf(TEXT("Do once false")));
					//hitChar->IsHit = true;
					//hitChar = HitRes.Actor.Get();
					/*if (HitRes.BoneName == "Pelvis" || HitRes.BoneName == "spine_03" || HitRes.BoneName == "spine_01")
					{
						hitChar->BoneName = "spine_02";
						hitChar->HitLocation = hitChar->GetMesh()->GetBoneLocation(hitChar->BoneName);
					}
					else
					{
						hitChar->BoneName = HitRes.BoneName;
						hitChar->HitLocation = HitRes.ImpactNormal;
					}*/
					hitChar->HitLocation = HitRes.Location;
					hitChar->BoneName = HitRes.BoneName;
					hitChar->IsHit = true;


					// check bone name / hit location == head / body then we apply damage & camera shake









					/*hitChar->staminaDrained = this->staminaDrained;
					hitChar->HitReactionsMoveset = this->HitReactionsMoveset;*/
					//DoDamage(hitChar);

					//HitRes.GetComponent();

					//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Mage)
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Bone: %s"), *hitChar->BoneName.ToString()));
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Bone: %s"), *HitRes.GetComponent()->GetName()));
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Impact: %s"), *hitChar->HitLocation.ToString()));
					GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("Blocking hit is %s"), (hitChar->IsHit) ? TEXT("True") : TEXT("False")));
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("You are hitting: %s"), *UKismetSystemLibrary::GetDisplayName(hitChar)));
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, FString::Printf(TEXT("hitchar exist")));
					DoDamage(hitChar);
					this->staminaDrained = 0.0f;
					//Camera Shake
					//GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(DamageCameraShake, 1.0f);
					if (hitChar->InRagdoll == false)
					{
						//UGameplayStatics::PlayWorldCameraShake(hitChar->GetWorld()->GetFirstPlayerController(), DamageCameraShake, hitChar->GetActorLocation(), 0.0f, 300.0f, 1.0f, true);
					}

					
				}
			}
		}
		
	}
	//Sphere trace by channel
	/*if (this->GetWorld()->SweepSingleByChannel(HitRes, StartPoint, EndPoint, FQuat::Identity, ECC_Visibility, SphereKick, CP_LKick))
	{

	}
	else
	{
		DoOnce = false;
		return;
	}*/
}

bool ATodakBattleArenaCharacter::UpdateHealth_Validate(int playerIndex, float HealthChange)
{
	if (this->Health >= this->MaxHealth)
	{
		//return false; // This will disconnect the caller
	}
	return true;
}

void ATodakBattleArenaCharacter::UpdateHealth_Implementation(int playerIndex, float HealthChange)
{
	////if can be accessed by the owning client
	//if (this->IsLocallyControlled())
	//{
	//	//Add pain meter value
	//	float currVal = this->Health - HealthChange;
	//	//this->Health = this->MaxHealth;
	//	//Distribute damage for each progressbar
	//	/*float MainDamage = UGestureMathLibrary::CalculateValueFromPercentage(this->MajorDamage, HealthChange, 100.0f);
	//	float SecDamage = HealthChange - MainDamage;*/

	//	if (currVal >= this->MaxHealth)
	//	{
	//		this->Health = this->MaxHealth;
	//	}
	//	else
	//		this->Health = currVal;

	//	/*float currSecHealth = this->Health + SecDamage;
	//	if (currSecHealth >= this->MaxHealth)
	//	{
	//		this->SecondaryHealth = this->MaxHealth;
	//	}
	//	else
	//		this->SecondaryHealth = currSecHealth;*/
	//	UE_LOG(LogTemp, Warning, TEXT("Health : %f"), this->Health);

	//	//Get the secondary progressbar for pain meter
	//	//const FName locTextControlHealthBar_1 = FName(TEXT("HPBar_1"));
	//	//UProgressBar* healthBar_1 = (UProgressBar*)(this->WidgetHUD->WidgetTree->FindWidget(locTextControlHealthBar_1));

	//	//Update both progress bar for pain meter
	//	this->playerHealth = UGestureInputsFunctions::UpdateProgressBarComponent(this->WidgetHUD, "HPBar", "Health", "HP", "Pain Meter", this->Health, this->MaxHealth);

	//	//this->playerHealth_1 = UGestureMathLibrary::SetProgressBarValue("Pain Meter", healthBar_1, nullptr, nullptr, this->SecondaryHealth, this->MaxHealth);

	//	//Start Pain Meter degeneration
	//	if (GetWorld()->GetTimerManager().IsTimerActive(this->StartHealthTimer) == false && (this->Health > 0.0f) && (this->Health < this->MaxHealth))
	//	{
	//		//For first pain meter progress bar
	//		FTimerDelegate FunctionsName;
	//		//FunctionsName = FTimerDelegate::CreateUObject(this, &ATodakBattleArenaCharacter::UpdateHealthStatusBar, EBarType::PrimaryProgressBar);
	//		FunctionsName = FTimerDelegate::CreateUObject(this, &ATodakBattleArenaCharacter::UpdateCurrentPlayerMainStatusBar, EBarType::PrimaryProgressBar, EMainPlayerStats::PainMeter, this->StartHealthTimer, this->StartSecondaryHealthTimer);
	//		
	//		UE_LOG(LogTemp, Warning, TEXT("TimerHealth has started!"));
	//		GetWorld()->GetTimerManager().SetTimer(this->StartHealthTimer, FunctionsName, MajorHealthRate, true);
	//	}
	//	//For second pain meter progress bar
	//	/*if (GetWorld()->GetTimerManager().IsTimerActive(this->StartSecondaryHealthTimer) == false && ((this->SecondaryHealth > this->Health) && (this->SecondaryHealth > 0.0f)))
	//	{
	//		FTimerDelegate FunctionsNames;
	//		FunctionsNames = FTimerDelegate::CreateUObject(this, &ATodakBattleArenaCharacter::UpdateCurrentPlayerMainStatusBar, EBarType::SecondaryProgressBar, EMainPlayerStats::PainMeter, this->StartHealthTimer, this->StartSecondaryHealthTimer);

	//		UE_LOG(LogTemp, Warning, TEXT("SecondaryHealth has started!"));
	//		GetWorld()->GetTimerManager().SetTimer(this->StartSecondaryHealthTimer, FunctionsNames, MinorHealthRate, true);
	//	}*/
	//}
	//Add pain meter value
	float currVal = this->Health - HealthChange;
	//this->Health = this->MaxHealth;
	//Distribute damage for each progressbar
	/*float MainDamage = UGestureMathLibrary::CalculateValueFromPercentage(this->MajorDamage, HealthChange, 100.0f);
	float SecDamage = HealthChange - MainDamage;*/

	if (currVal >= this->MaxHealth)
	{
		this->Health = this->MaxHealth;
	}
	else
		this->Health = currVal;

	/*float currSecHealth = this->Health + SecDamage;
	if (currSecHealth >= this->MaxHealth)
	{
		this->SecondaryHealth = this->MaxHealth;
	}
	else
		this->SecondaryHealth = currSecHealth;*/
	UE_LOG(LogTemp, Warning, TEXT("Health : %f"), this->Health);

	//Get the secondary progressbar for pain meter
	//const FName locTextControlHealthBar_1 = FName(TEXT("HPBar_1"));
	//UProgressBar* healthBar_1 = (UProgressBar*)(this->WidgetHUD->WidgetTree->FindWidget(locTextControlHealthBar_1));

	//Update both progress bar for pain meter

	OnRep_Health();
	/*this->playerHealth = UGestureInputsFunctions::UpdateProgressBarComponent(this->WidgetHUD, "HPBar", "Health", "HP", "Pain Meter", this->Health, this->MaxHealth);
*/
	//this->playerHealth_1 = UGestureMathLibrary::SetProgressBarValue("Pain Meter", healthBar_1, nullptr, nullptr, this->SecondaryHealth, this->MaxHealth);

	//Start Pain Meter degeneration
	if (this->IsLocallyControlled())
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(this->StartHealthTimer) == false && (this->Health > 0.0f) && (this->Health < this->MaxHealth))
		{
			//Update healthbar using timer
			ServerStartHealthTimer(this->MaxHealth, MajorHealthRate);

			////For first pain meter progress bar
			//FTimerDelegate FunctionsName;
			////FunctionsName = FTimerDelegate::CreateUObject(this, &ATodakBattleArenaCharacter::UpdateHealthStatusBar, EBarType::PrimaryProgressBar);
			//FunctionsName = FTimerDelegate::CreateUObject(this, &ATodakBattleArenaCharacter::UpdateCurrentPlayerMainStatusBar, EBarType::PrimaryProgressBar, EMainPlayerStats::PainMeter, this->StartHealthTimer, this->StartSecondaryHealthTimer);

			//UE_LOG(LogTemp, Warning, TEXT("TimerHealth has started!"));
			//GetWorld()->GetTimerManager().SetTimer(this->StartHealthTimer, FunctionsName, MajorHealthRate, true);
		}
	}
	
	//For second pain meter progress bar
	/*if (GetWorld()->GetTimerManager().IsTimerActive(this->StartSecondaryHealthTimer) == false && ((this->SecondaryHealth > this->Health) && (this->SecondaryHealth > 0.0f)))
	{
		FTimerDelegate FunctionsNames;
		FunctionsNames = FTimerDelegate::CreateUObject(this, &ATodakBattleArenaCharacter::UpdateCurrentPlayerMainStatusBar, EBarType::SecondaryProgressBar, EMainPlayerStats::PainMeter, this->StartHealthTimer, this->StartSecondaryHealthTimer);

		UE_LOG(LogTemp, Warning, TEXT("SecondaryHealth has started!"));
		GetWorld()->GetTimerManager().SetTimer(this->StartSecondaryHealthTimer, FunctionsNames, MinorHealthRate, true);
	}*/
}

bool ATodakBattleArenaCharacter::ServerUpdateHealth_Validate(int playerIndex, float HealthChange)
{
	return true;
}

void ATodakBattleArenaCharacter::ServerUpdateHealth_Implementation(int playerIndex, float HealthChange)
{
	UpdateHealth(playerIndex, HealthChange);
}

bool ATodakBattleArenaCharacter::ServerStartHealthTimer_Validate(int MaxVal, float rate)
{
	return true;
}

void ATodakBattleArenaCharacter::ServerStartHealthTimer_Implementation(int MaxVal, float rate)
{
	//start energy timer on the server
	FTimerDelegate FunctionsNames = FTimerDelegate::CreateUObject(this, &ATodakBattleArenaCharacter::ServerUpdateHealthBar, MaxVal);
	this->GetWorld()->GetTimerManager().SetTimer(this->StartHealthTimer, FunctionsNames, rate, true);
}

bool ATodakBattleArenaCharacter::ServerUpdateHealthBar_Validate(int MaxVal)
{
	return true;
}

void ATodakBattleArenaCharacter::ServerUpdateHealthBar_Implementation(int MaxVal)
{
	if (this->Health < MaxVal)
	{
		ClientUpdateHealthBar(MaxVal);
	}
	else
	{
		//stop timer on server
		ServerTimerHandler(this->StartHealthTimer);
	}
}

bool ATodakBattleArenaCharacter::ClientUpdateHealthBar_Validate(int MaxVal)
{
	return true;
}

void ATodakBattleArenaCharacter::ClientUpdateHealthBar_Implementation(int MaxVal)
{
	float currHP = this->Health + 1.0f;

	if (currHP >= MaxVal)
	{
		this->Health = MaxVal;
		if (this->IsLocallyControlled())
		{
			//stop timer on server
			ServerTimerHandler(this->StartHealthTimer);
		}
	}
	else
		this->Health = currHP;

	OnRep_Health();
	UE_LOG(LogTemp, Log, TEXT("HealthVal : %f"), this->Health);
}

void ATodakBattleArenaCharacter::UpdateDamage(float DamageValue)
{
	//damage = DamageValue;
	// Increase (or decrease) current damage
	if (GetLocalRole() == ROLE_Authority)
	{
		//float DamageFromStrength = UGestureMathLibrary::CalculateValueFromPercentage(10.0f, MaxStrength, 100.0f);

		//Calculate total damage applied from current action with current instigator's maximum strength
		this->damage = DamageValue;
		UE_LOG(LogTemp, Warning, TEXT("Damage : %f"), this->damage);
	}
}

bool ATodakBattleArenaCharacter::ClientUpdateEnergyBar_Validate(float currVal, int MaxVal)
{
	return true;
}

void ATodakBattleArenaCharacter::ClientUpdateEnergyBar_Implementation(float currVal, int MaxVal)
{
	//float val = currVal + 1.0f;
	float currEnergy = this->playerEnergy + 1.0f;

	if (currEnergy >= MaxVal)
	{
		this->playerEnergy = MaxVal;
		if (this->IsLocallyControlled())
		{
			//stop timer on server
			ServerTimerHandler(this->StartEnergyTimer);
		}
	}
	else
		this->playerEnergy = currEnergy;

	OnRep_CurrentEnergy();
	//UE_LOG(LogTemp, Log, TEXT("Energyval : %f"), this->playerEnergy);
}

bool ATodakBattleArenaCharacter::ServerUpdateEnergyBar_Validate(float currVal, int MaxVal)
{
	return true;
}

void ATodakBattleArenaCharacter::ServerUpdateEnergyBar_Implementation(float currVal, int MaxVal)
{
	if (this->playerEnergy < MaxVal)
	{
		ClientUpdateEnergyBar(this->playerEnergy, MaxVal);
	}
	else
	{
		//stop timer on server
		ServerTimerHandler(this->StartEnergyTimer);
	}
}

///////////////////////////////Timer to run on server/////////////////////////////////////////////////////////////////////////////////
bool ATodakBattleArenaCharacter::ServerTimerHandler_Validate(FTimerHandle TimerHandler)
{
	return true;
}

void ATodakBattleArenaCharacter::ServerTimerHandler_Implementation(FTimerHandle TimerHandler)
{
	UE_LOG(LogTemp, Log, TEXT("Timer has ended!"));
	this->GetWorld()->GetTimerManager().ClearTimer(TimerHandler);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ATodakBattleArenaCharacter::ServerSkillMoveset_Validate(UAnimMontage* ServerSkill, UAnimMontage* HitReaction, FBlockActions BlockMovesets, float DamageApplied, float StaminaUsed, float StaminaDrain, float PlayRate, float StartTime, bool SkillFound)
{
	return true;
}

void ATodakBattleArenaCharacter::ServerSkillMoveset_Implementation(UAnimMontage* ServerSkill, UAnimMontage* HitReaction, FBlockActions BlockMovesets, float DamageApplied, float StaminaUsed, float StaminaDrain, float PlayRate, float StartTime, bool SkillFound)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		RPCServerSkill = ServerSkill;
		SkillExecuted = SkillFound;
		//SectionName = MontageSection;
		//OnRep_Blockhit(BlockMovesets);
		
		//damage = DamageApplied;

		MulticastSkillMoveset(RPCServerSkill, HitReaction, BlockMovesets, DamageApplied, StaminaUsed, StaminaDrain, PlayRate, StartTime, SkillFound);
	}
}

bool ATodakBattleArenaCharacter::MulticastSkillMoveset_Validate(UAnimMontage* MulticastSkill, UAnimMontage* HitReaction, FBlockActions BlockMovesets, float DamageApplied, float StaminaUsed, float StaminaDrain, float PlayRate, float StartTime, bool SkillFound)
{
	return true;
}

//Play swipe action anim
void ATodakBattleArenaCharacter::MulticastSkillMoveset_Implementation(UAnimMontage* MulticastSkill, UAnimMontage* HitReaction, FBlockActions BlockMovesets, float DamageApplied, float StaminaUsed, float StaminaDrain, float PlayRate, float StartTime, bool SkillFound)
{
	if (SkillFound == true)
	{
		this->GetWorld()->GetTimerManager().ClearTimer(StartEnergyTimer);
		FTimerHandle Delay;

		RPCMultiCastSkill = MulticastSkill;
		//SectionName = MontageSectiom;

		//Disable movement on Montage Play
		canMove = false;
		
		float MovesetDuration = this->GetMesh()->GetAnimInstance()->Montage_Play(RPCMultiCastSkill, PlayRate, EMontagePlayReturnType::MontageLength, StartTime, true);
		UE_LOG(LogTemp, Warning, TEXT("Montage Name: %s"), *RPCMultiCastSkill->GetFName().ToString());

		this->GetWorld()->GetTimerManager().SetTimer(Delay, this, &ATodakBattleArenaCharacter::ResetMovementMode, MovesetDuration, false);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Timer remaining: %f"), this->GetWorld()->GetTimerManager().GetTimerRemaining(Delay)));

		/* Play montage by section
		this->GetMesh()->GetAnimInstance()->Montage_Play(RPCMultiCastSkill, PlayRate, EMontagePlayReturnType::MontageLength, StartTime, true);
		this->GetMesh()->GetAnimInstance()->Montage_JumpToSection(MontageSection, RPCMultiCastSkill);

		//Get the section length
		SectionLength = RPCMultiCastSkill->GetSectionLength(SectionUUID);
		
		//Wait until Montage finished playing based on Section, then execute ResetMovementMode function
		this->GetWorld()->GetTimerManager().SetTimer(Delay, this, &ATodakBattleArenaCharacter::ResetMovementMode, SectionLength, false);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Timer remaining: %f"), this->GetWorld()->GetTimerManager().GetTimerRemaining(Delay)));*/

		if (LevelName != UGameplayStatics::GetCurrentLevelName(this, true))
		{
			UpdateDamage(DamageApplied);
		}

		//this->GetMesh()->GetAnimInstance()->StopAllMontages(3.0f);
		if (GetWorld()->GetTimerManager().IsTimerActive(StartEnergyTimer) == false)
		{
			//GetMesh()->SetSimulatePhysics(false);

			if (LevelName != UGameplayStatics::GetCurrentLevelName(this, true))
			{
				//EnergySpent(StaminaUsed, 100.0f, durations);
				//ServerEnergySpent(StaminaUsed, 100.0f, durations);
				//if this client has access
				if (this->IsLocallyControlled())
				{
					ServerEnergySpent(StaminaUsed, 100.0f, MovesetDuration);
					this->BlockedHit = false;
					UE_LOG(LogTemp, Warning, TEXT("Energy: %f"), this->playerEnergy);
				}
			}
		}
	}
	
	//*************************old code********************************//
	//if action is found, play new action anim, else stop the current action, else stop the current anim immediately
	/*if (SkillFound == true)
	{
		//If the anim is not currently playing
		FTimerHandle Delay;

		RPCMultiCastSkill = MulticastSkill;

		//get section end length
		//this->GetMesh()->GetAnimInstance()->Montage_JumpToSectionsEnd(SectionName, RPCMultiCastSkill);
		//float endSection = GetMesh()->GetAnimInstance()->Montage_GetPosition(RPCMultiCastSkill);

		//float StartSect = 0.0f;
		//float EndSect = 0.0f;

		//RPCMultiCastSkill->GetSectionStartAndEndTime(RPCMultiCastSkill->GetSectionIndex(SectionName), StartSect, EndSect);

		//Play new anim on client
		float durations = this->GetMesh()->GetAnimInstance()->Montage_Play(RPCMultiCastSkill, PlayRate, EMontagePlayReturnType::MontageLength, StartTime, true);
		//this->GetMesh()->GetAnimInstance()->Montage_JumpToSection(SectionName, RPCMultiCastSkill);
		canMove = false;

		//get current section length
		//float startSection = this->GetMesh()->GetAnimInstance()->Montage_GetPosition(RPCMultiCastSkill);

		//get section length
		//float SectionLength = endSection - startSection;

		//float SectionLength = EndSect - StartTime;

		UE_LOG(LogTemp, Warning, TEXT("Montage Name: %s"), *RPCMultiCastSkill->GetFName().ToString());

		//this->PlayAnimMontage(RPCMultiCastSkill);

		if (LevelName != UGameplayStatics::GetCurrentLevelName(this, true))
		{
			UpdateDamage(DamageApplied, CurrStrength, CurrStamina, CurrAgility);
		}

		//stop current played anim
		this->GetMesh()->GetAnimInstance()->Montage_Stop(3.0f, RPCMultiCastSkillHold);
		
		//this->GetMesh()->GetAnimInstance()->StopAllMontages(3.0f);

		if (GetWorld()->GetTimerManager().IsTimerActive(Delay) == false)
		{
			//GetMesh()->SetSimulatePhysics(false);
			
			// duration to wait for montage finished playing
			this->GetWorld()->GetTimerManager().SetTimer(Delay, this, &ATodakBattleArenaCharacter::ResetMovementMode, durations, false);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Timer remaining: %f"), this->GetWorld()->GetTimerManager().GetTimerRemaining(Delay)));
			//float this->GetWorld()->GetTimerManager().GetTimerRemaining(Delay);

			if (LevelName != UGameplayStatics::GetCurrentLevelName(this, true))
			{
				//if this client has access
				if (this->IsLocallyControlled())
				{
					if (this->BlockedHit == true)
					{
						this->BlockedHit = false;
					}
					//if still in blocked hit state
					//Reduce player energy after action
					EnergySpent(5.0f, 100.0f);
					UE_LOG(LogTemp, Warning, TEXT("Energy: %f"), this->playerEnergy);

					//Update stats after anim is played
					if (GetWorld()->GetTimerManager().IsTimerActive(StartEnergyTimer) == false && (this->playerEnergy <= this->MaxEnergy))
					{
						//Set timer for EnergyBar to regen after action
						FTimerDelegate FunctionsNames;
						FunctionsNames = FTimerDelegate::CreateUObject(this, &ATodakBattleArenaCharacter::UpdateCurrentPlayerMainStatusBar, EBarType::PrimaryProgressBar, EMainPlayerStats::Energy, this->StartEnergyTimer, this->StartEnergyTimer);

						UE_LOG(LogTemp, Warning, TEXT("EnergyTimer has started!"));
						GetWorld()->GetTimerManager().SetTimer(this->StartEnergyTimer, FunctionsNames, EnergyRate, true);

						//UE_LOG(LogTemp, Warning, TEXT("Timer has started!"));
						//GetWorld()->GetTimerManager().SetTimer(StartEnergyTimer, this, &ATodakBattleArenaCharacter::UpdateEnergyStatusBar, 1.5f, true, 2.0f);
					}
				}
			}
			
		}
		//canMove = true;
	}
	else
	{
		//If the anim is not currently playing
		this->StopAnimMontage(RPCMultiCastSkillHold);
		//this->GetMesh()->GetAnimInstance()->StopAllMontages(3.0f);
		this->ResetMovementMode();

		if (IsLocallyControlled())
		{
			if (this->BlockedHit == true)
			{
				this->BlockedHit = false;
			}
		}

		//if still in blocked hit state
	}*/
}

bool ATodakBattleArenaCharacter::ServerSkillStartMontage_Validate(UAnimMontage* ServerSkill, float StartAnimTime, float PauseAnimTime)
{
	return true;
}

void ATodakBattleArenaCharacter::ServerSkillStartMontage_Implementation(UAnimMontage* ServerSkill, float StartAnimTime, float PauseAnimTime)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		RPCServerSkillHold = ServerSkill;
		MulticastSkillStartMontage(RPCServerSkillHold, StartAnimTime, PauseAnimTime);
	}
}

bool ATodakBattleArenaCharacter::MulticastSkillStartMontage_Validate(UAnimMontage* MulticastSkill, float StartAnimTime, float PauseAnimTime)
{
	return true;
}

void ATodakBattleArenaCharacter::MulticastSkillStartMontage_Implementation(UAnimMontage* MulticastSkill, float StartAnimTime, float PauseAnimTime)
{
	//Play anim on touch press/hold
	RPCMultiCastSkillHold = MulticastSkill;
	//SectionName = SectionNames;
	SkillStopTime = PauseAnimTime;
	this->GetMesh()->GetAnimInstance()->Montage_Play(RPCMultiCastSkillHold, 1.0f, EMontagePlayReturnType::Duration, StartAnimTime);
	//this->GetMesh()->GetAnimInstance()->Montage_JumpToSection(SectionName, RPCMultiCastSkillHold);
	//canMove = false;
	UE_LOG(LogTemp, Warning, TEXT("RPCMultiCastSkillHold : %s"), *RPCMultiCastSkillHold->GetFName().ToString());
	//UE_LOG(LogTemp, Warning, TEXT("SectionName : %s"), *SectionName.ToString());
	UE_LOG(LogTemp, Warning, TEXT("SkillStopTime : %f"), SkillStopTime);
}

bool ATodakBattleArenaCharacter::ServerSkillBlockHitMontage_Validate(UAnimMontage* ServerSkill, float StartAnimTime, float PauseAnimTime, bool IsBlocked)
{
	return true;
}

void ATodakBattleArenaCharacter::ServerSkillBlockHitMontage_Implementation(UAnimMontage* ServerSkill, float StartAnimTime, float PauseAnimTime, bool IsBlocked)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		RPCServerBlockHit = ServerSkill;
		MulticastSkillBlockHitMontage(RPCServerBlockHit, StartAnimTime, PauseAnimTime, IsBlocked);
		
	}
}

bool ATodakBattleArenaCharacter::MulticastSkillBlockHitMontage_Validate(UAnimMontage* MulticastSkill, float StartAnimTime, float PauseAnimTime, bool IsBlocked)
{
	return true;
}

void ATodakBattleArenaCharacter::MulticastSkillBlockHitMontage_Implementation(UAnimMontage* MulticastSkill, float StartAnimTime, float PauseAnimTime, bool IsBlocked)
{
	//play block action montage when player presses the block button
	this->BlockedHit = IsBlocked;
	if (IsBlocked == true)
	{
		//Play anim on touch press/hold
		RPCMultiCastBlockHit = MulticastSkill;
		SkillStopTime = PauseAnimTime;
		this->GetMesh()->GetAnimInstance()->Montage_Play(RPCMultiCastBlockHit, 1.0f, EMontagePlayReturnType::Duration, StartAnimTime);

		UE_LOG(LogTemp, Warning, TEXT("RPCMultiCastSkillBlock : %s"), *RPCMultiCastBlockHit->GetFName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("SkillStopTime : %f"), SkillStopTime);
	}
	else
	{
		//If the anim is not currently playing
		this->StopAnimMontage(RPCMultiCastBlockHit);
	}

	// ***Blocked hit montage assigned follows the block action***
	//this->BlockedHit = IsBlocked;

	////checks who is getting hit
	//if (HitActor == this)
	//{
	//	ATodakBattleArenaCharacter* HitChar = Cast<ATodakBattleArenaCharacter>(HitActor);
	//	if (IsBlocked == true && HitChar->IsEffectiveBlock == true)
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("CPP EFFECTIVE BLOCK"));
	//		FTimerHandle Delay;
	//		RPCMultiCastBlockHit = MulticastSkill;

	//		//if (!this->IsLocallyControlled())
	//		//{
	//		//	//APlayerController* const PlayerController = Cast<APlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	//		//	/*ATodakBattleArenaPlayerController* AttControl = Cast<ATodakBattleArenaPlayerController>(Controller);
	//		//	AttControl->DisableInput(GetWorld()->GetFirstPlayerController());*/
	//		//	//Controller->DisableInput(GetPlayerControllers());
	//		//	//Cast<ATodakBattleArenaPlayerController>(GetController())->ToggleOffInput();
	//		//	this->EnemyElement->CanSwipeAction = false;
	//		//	UE_LOG(LogTemp, Warning, TEXT("DISABLE INPUT"));
	//		//}

	//		//Disable input for the other so we can counter attack
	//		this->EnemyElement->CanSwipeAction = false;
	//		UE_LOG(LogTemp, Warning, TEXT("DISABLE INPUT"));
	//		ServerSlowmo(0.2f);
	//		CameraShake();
	//		//this->GetMesh()->GetAnimInstance()->Montage_Play(RPCMultiCastBlockHit, 1.0f, EMontagePlayReturnType::Duration, StartAnimTime);
	//		float Duration = GetMesh()->GetAnimInstance()->Montage_Play(RPCMultiCastBlockHit, 1.0f, EMontagePlayReturnType::MontageLength, StartAnimTime, true);	
	//		this->GetWorld()->GetTimerManager().SetTimer(Delay, this, &ATodakBattleArenaCharacter::EndingEffectiveBlock, Duration, false);

	//	}

	//	else
	//	{
	//		RPCMultiCastBlockHit = MulticastSkill;
	//		//SkillStopTime = PauseAnimTime;
	//		this->GetMesh()->GetAnimInstance()->Montage_Play(RPCMultiCastBlockHit, 1.0f, EMontagePlayReturnType::Duration, StartAnimTime);
	//		UE_LOG(LogTemp, Warning, TEXT("HIT BLOCK REACTION"));
	//		UE_LOG(LogTemp, Warning, TEXT("RPCMultiCastSkillBlock : %s"), *RPCMultiCastBlockHit->GetFName().ToString());
	//		UE_LOG(LogTemp, Warning, TEXT("SkillStopTime : %f"), SkillStopTime);

	//	}
	//}

	////checks who is pressing the block button
	//else if (HitActor != this)
	//{
	//	if (IsBlocked == true && PauseAnimTime > 0.0f)
	//	{
	//		//Play anim on touch press/hold
	//		RPCMultiCastBlockHit = MulticastSkill;
	//		SkillStopTime = PauseAnimTime;
	//		this->GetMesh()->GetAnimInstance()->Montage_Play(RPCMultiCastBlockHit, 1.0f, EMontagePlayReturnType::Duration, StartAnimTime);
	//		UE_LOG(LogTemp, Warning, TEXT("BLOCK ACTION"));
	//		UE_LOG(LogTemp, Warning, TEXT("RPCMultiCastSkillBlock : %s"), *RPCMultiCastBlockHit->GetFName().ToString());
	//		UE_LOG(LogTemp, Warning, TEXT("SkillStopTime : %f"), SkillStopTime);


	//	}

	//	else
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("STOP PLAYING ANIM"));
	//		//If the anim is not currently playing
	//		this->StopAnimMontage(RPCMultiCastBlockHit);
	//		//this->ResetMovementMode();

	//	}
	//}

	//Play anim on block incoming hit
	/*if (GetWorld()->GetTimerManager().IsTimerActive(BlockHitTimer) == false && GetWorld()->GetTimerManager().IsTimerPaused(BlockHitTimer) == false)
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
	}*/
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
	this->GetMesh()->GetAnimInstance()->Montage_Stop(3.0f, MulticastSkill);
	this->GetMesh()->GetAnimInstance()->Montage_Play(RPCMultiCastSkillHold, 2.0f, EMontagePlayReturnType::MontageLength, SkillStopTime, false);
	UE_LOG(LogTemp, Warning, TEXT("Stop block timer!"));	
}

bool ATodakBattleArenaCharacter::ServerPlayMontage_Validate(UAnimInstance* CurrMesh, UAnimMontage* MontageToPlay)
{
	//UAnimBlueprintGeneratedClass* CurrAnimBP
	if (CurrMesh == NULL)
	{
		return false;
	}
	return true;
}

void ATodakBattleArenaCharacter::ServerPlayMontage_Implementation(UAnimInstance* CurrMesh, UAnimMontage* MontageToPlay)
{
	if(GetLocalRole() == ROLE_Authority)
	{
		MulticastPlayMontage(CurrMesh, MontageToPlay);
	}
}

bool ATodakBattleArenaCharacter::MulticastPlayMontage_Validate(UAnimInstance* CurrMesh, UAnimMontage* MontageToPlay)
{
	if (MontageToPlay == NULL)
	{
		return false;
	}
	return true;
}

void ATodakBattleArenaCharacter::MulticastPlayMontage_Implementation(UAnimInstance* CurrMesh, UAnimMontage* MontageToPlay)
{
	//UAnimBlueprintGeneratedClass* CurrAnimBP = Cast<UAnimBlueprintGeneratedClass>(CurrMesh->GetAnimInstance());
	if (CurrMesh != NULL)
	{
		float val = CurrMesh->Montage_Play(MontageToPlay, 1.0f, EMontagePlayReturnType::Duration, 0.0f, true);
	}
}

void ATodakBattleArenaCharacter::UpdateCurrentMontage(UAnimMontage* MulticastSkill, FTimerHandle* TimerUsed)
{
	//if the current anim pos is more than .6 seconds, stop the montage and play the hold skill again
	if (GetWorld()->GetTimerManager().IsTimerActive(*TimerUsed) == true)
	{
		ServerStopBlockHitMontage(MulticastSkill);
	}
}



void ATodakBattleArenaCharacter::ChangeCameraPerspective(int CamPers)
{
	CameraPerspective = CamPers;

	if (CamPers == 0)
	{
		if (TargetLocked)
		{
			FLatentActionInfo LatentInfo = FLatentActionInfo();
			LatentInfo.CallbackTarget = this;
			/*LatentInfo.ExecutionFunction = FName("OnLockedTPPFinished");
			LatentInfo.UUID = GetNextUUID();
			LatentInfo.Linkage = 0;*/
			UKismetSystemLibrary::MoveComponentTo(this->FollowCamera, FVector(200.0f, 195.0f, 0.0f), FRotator(-10.0f, -55.0f, 0.0f), true, true, 1.0f, true, EMoveComponentAction::Type::Move, LatentInfo);
			FarToTPPTimeline->PlayFromStart();
			FarToTPPTimeline->AddInterpFloat(fCurve, Interp_FarToTPP, FName{ TEXT("TL_FarToTPP") });
		}

		else if (!TargetLocked)
		{
			FLatentActionInfo LatentInfo = FLatentActionInfo();
			LatentInfo.CallbackTarget = this;
			UKismetSystemLibrary::MoveComponentTo(this->FollowCamera, FVector(0.0f, 20.0f, 0.0f), FRotator(-10.0f, 0.0f, 0.0f), true, true, 1.0f, true, EMoveComponentAction::Type::Move, LatentInfo);
			FarToTPPTimeline->PlayFromStart();
			FarToTPPTimeline->AddInterpFloat(fCurve, Interp_FarToTPP, FName{ TEXT("TL_FarToTPP") });
		}
	}

	else if (CamPers == 1)
	{
		FLatentActionInfo LatentInfo = FLatentActionInfo();
		LatentInfo.CallbackTarget = this;
		LatentInfo.ExecutionFunction = FName("OnFPPCameraFinished");
		LatentInfo.UUID = GetNextUUID();
		LatentInfo.Linkage = 0;
		//this->FollowCamera->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, "head");
		//UKismetSystemLibrary::MoveComponentTo(this->FollowCamera, FVector(this->GetMesh()->GetSocketLocation("head")), FRotator(0.0f, 0.0f, 0.0f), true, true, 1.0f, true, EMoveComponentAction::Type::Move, LatentInfo);
		UKismetSystemLibrary::MoveComponentTo(this->FollowCamera, FVector(310.0f, 0.0f, -15.0f), FRotator(0.0f, 0.0f, 0.0f), true, true, 1.0f, true, EMoveComponentAction::Type::Move, LatentInfo);
		//UKismetSystemLibrary::Delay(this, 2.0f, LatentInfo);
		
	}

	else if (CamPers == 2)
	{
		if (IsLocked)
		{
			FLatentActionInfo LatentInfo = FLatentActionInfo();
			LatentInfo.CallbackTarget = this;
			this->FollowCamera->AttachToComponent(this->CameraBoom, FAttachmentTransformRules::KeepWorldTransform, "NONE");
			UKismetSystemLibrary::MoveComponentTo(this->FollowCamera, FVector(360.0f, 360.0f, 30.0f), FRotator(-15.0f, -65.0f, 0.0f), true, true, 1.0f, true, EMoveComponentAction::Type::Move, LatentInfo);
			this->FollowCamera->bUsePawnControlRotation = false;
			this->FollowCamera->bLockToHmd = false;
			FPPToFarTimeline->PlayFromStart();
			FPPToFarTimeline->AddInterpFloat(fCurve2, Interp_FPPToFar, FName{ TEXT("TL_FPPToFar") });
		}
		
		else if (!IsLocked)
		{
			FLatentActionInfo LatentInfo = FLatentActionInfo();
			LatentInfo.CallbackTarget = this;
			this->FollowCamera->AttachToComponent(this->CameraBoom, FAttachmentTransformRules::KeepWorldTransform, "NONE");
			UKismetSystemLibrary::MoveComponentTo(this->FollowCamera, FVector(0.0f, 20.0f, 50.0f), FRotator(-10.0f, 0.0f, 0.0f), true, true, 1.0f, true, EMoveComponentAction::Type::Move, LatentInfo);
			this->FollowCamera->bUsePawnControlRotation = false;
			this->FollowCamera->bLockToHmd = false;
			FPPToFarTimeline->PlayFromStart();
			FPPToFarTimeline->AddInterpFloat(fCurve2, Interp_FPPToFar, FName{ TEXT("TL_FPPToFar") });
		}
	}
}

bool ATodakBattleArenaCharacter::ServerSlowmo_Validate(float TimeDilation)
{
	return true;
}

void ATodakBattleArenaCharacter::ServerSlowmo_Implementation(float TimeDilation)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		MulticastSlowmo(TimeDilation);
	}
}

bool ATodakBattleArenaCharacter::MulticastSlowmo_Validate(float TimeDilation)
{
	return true;
}

void ATodakBattleArenaCharacter::MulticastSlowmo_Implementation(float TimeDilation)
{
	UGameplayStatics::SetGlobalTimeDilation(this->GetWorld(), TimeDilation);
}

void ATodakBattleArenaCharacter::OnLockedTPPFinished()
{
	FarToTPPTimeline->PlayFromStart();
	FarToTPPTimeline->AddInterpFloat(fCurve, Interp_FarToTPP, FName{ TEXT("TL_FarToTPP") });
}

void ATodakBattleArenaCharacter::OnFPPCameraFinished()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, FString::Printf(TEXT("ON FPP CAMERA FINISHED")));
	this->FollowCamera->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, "head");
	this->FollowCamera->bUsePawnControlRotation = true;
	this->FollowCamera->bLockToHmd = true;
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
							TargetLocked = true;

							if (TargetLocked == true)
							{
								//Forces player to enter ready stance
								/*this->IsLocked = true;
								this->EnemyElement->IsLocked = true;*/
								RepIsMoving = true;
								RepWalkSpeed = 20.0f;
								this->GetCharacterMovement()->MaxWalkSpeed = RepWalkSpeed;

								if (CameraPerspective == 0)
								{
									//Sets player camera nearer TPP
									this->FollowCamera->SetFieldOfView(90.0f);
									FLatentActionInfo LatentInfo = FLatentActionInfo();
									LatentInfo.CallbackTarget = this;
									UKismetSystemLibrary::MoveComponentTo(this->FollowCamera, FVector(225.0f, 226.0f, -60.0f), FRotator(0.0f, -55.0f, 0.0f), true, true, 3.0f, true, EMoveComponentAction::Type::Move, LatentInfo);
									
								}

								if (CameraPerspective == 2)
								{
									//Sets player camera nearer FAR
									FLatentActionInfo LatentInfo = FLatentActionInfo();
									LatentInfo.CallbackTarget = this;
									UKismetSystemLibrary::MoveComponentTo(this->FollowCamera, FVector(360.0f, 360.0f, 30.0f), FRotator(-15.0f, -65.0f, 0.0f), true, true, 2.0f, true, EMoveComponentAction::Type::Move, LatentInfo);
								}

							}
						}
					}
				}
			}
			else if (EnemyChar && isAI == true)
			{
				AICanAttack = true;

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
							TargetLocked = true;

							if (TargetLocked == true)
							{
								//Forces player to enter ready stance
								/*this->EnemyElement->IsLocked = true;*/
								RepIsMoving = true;
							}
						}
					}
				}
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
							//this->IsLocked = false;

							/*if (this->EnemyElement != nullptr)
							{
								this->EnemyElement->IsLocked = false;
							}*/
							
							//EnemyElement->RepIsMoving = false;
							RepWalkSpeed = 40.0f;
							this->GetCharacterMovement()->MaxWalkSpeed = RepWalkSpeed;

							if (CameraPerspective == 0)
							{
								//Sets player camera further 
								FLatentActionInfo LatentInfo = FLatentActionInfo();
								LatentInfo.CallbackTarget = this;
								UKismetSystemLibrary::MoveComponentTo(this->FollowCamera, FVector(0.0f, 60.0f, 0.0f), FRotator(-10.0f, 0.0f, 0.0f), true, true, 3.0f, true, EMoveComponentAction::Type::Move, LatentInfo);
								this->FollowCamera->SetFieldOfView(75.0f);
							}

							if (CameraPerspective == 2)
							{
								//Sets player camera nearer
								FLatentActionInfo LatentInfo = FLatentActionInfo();
								LatentInfo.CallbackTarget = this;
								UKismetSystemLibrary::MoveComponentTo(this->FollowCamera, FVector(0.0f, 20.0f, 0.0f), FRotator(-10.0f, 0.0f, 0.0f), true, true, 2.0f, true, EMoveComponentAction::Type::Move, LatentInfo);
							}
						}

						//Stop target lock timer	//buat untuk ai juga
						GetWorld()->GetTimerManager().ClearTimer(ToggleTimer);
						if (this->RightVal == 0.0f && this->GetCharacterMovement()->Velocity.Size() == 0.0f)
						{
							UTBAAnimInstance* AnimInst = Cast<UTBAAnimInstance>(this->GetMesh()->GetAnimInstance());
							AnimInst->TurnRight = false;
							AnimInst->TurnLeft = false;
						}

						if (this->EnemyElement != nullptr)
						{
							if (this->EnemyElement->RightVal == 0.0f && this->EnemyElement->GetCharacterMovement()->Velocity.Size() == 0.0f)
							{
								UTBAAnimInstance* AnimEnemInst = Cast<UTBAAnimInstance>(this->EnemyElement->GetMesh()->GetAnimInstance());
								AnimEnemInst->TurnRight = false;
								AnimEnemInst->TurnLeft = false;
							}
						}
						
						
					}
				}
			}
			else if (EnemyChar && isAI == true)
			{
				AICanAttack = false;
				CanBeTargeted = false;
				if (TargetLocked == true)
				{
					TargetLocked = false;
					//if (this->EnemyElement != nullptr)
					//{
					//	//Allows Idle Timer
					//	this->EnemyElement->IsLocked = false;
					//	//EnemyElement->RepIsMoving = false;
					//}
					
				}
			}
		}
		this->EnemyElement = nullptr;
	}
}

bool ATodakBattleArenaCharacter::ExecuteAction(bool SkillTrigger, float AnimRate, float AnimStartTime, UAnimMontage* SkillMovesets, UAnimMontage* HitMovesets, FBlockActions BlockMovesets, float DealDamage, float StaminaUsed, float StaminaDrain, bool& CDSkill)
{
	if (SkillTrigger == true)
	{
		//Emptying arrays
		if (SwipeActions.IsValidIndex(0))
		{
			SwipeActions.Empty();
		}
		BodyParts.Empty();

		//Set all the attribute to the current vars of player
		//HitTraceLength = HitTraceLengths;
		//this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		
		
		//this->GetCharacterMovement()->StopMovementImmediately();
		CDSkill = true;

		//Get the Montage to be play
		SkillMoveset = SkillMovesets;

		//// Set Montage Section Name on Jab Spam
		//if (JabCounts > 1)
		//{
		//	SectionName = "Spam";
		//	SectionUUID = 1;
		//	AnimStartTime = 0.5f;

		//}

		//else if (JabCounts <= 1)
		//{
		//	SectionName = "Default";
		//	SectionUUID = 0;
	
		//}

		//Server
		if (this->IsLocallyControlled())
		{
			ServerSkillMoveset(SkillMoveset, HitMovesets, BlockMovesets, DealDamage, StaminaUsed, StaminaDrain, AnimRate, AnimStartTime, SkillTrigger);
		}
		
		
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

void ATodakBattleArenaCharacter::InitializeCharAtt()
{
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

	//this->Stamina = this->MaxStamina;
	//this->Strength = this->MaxStrength;
	//this->Agility = this->MaxAgility;

	//this->StaminaPercentage = 1.0f;
	//this->StrengthPercentage = 1.0f;
	//this->AgilityPercentage = 1.0f;

	this->MaxHealth = 1000;
	this->MaxEnergy = 1000;

	//Health
	/*Health = UGestureMathLibrary::CalculateValueFromPercentage(10.0f, MaxStrength, 100.0f) + MaxHealth;
	MaxHealth = Health;
	playerHealth = 1.0f;
	UE_LOG(LogTemp, Warning, TEXT("MaxHealth : %d"), MaxHealth);*/

	Health = MaxHealth;
	playerHealth = 1.0f;
	UE_LOG(LogTemp, Warning, TEXT("MaxHealth : %d"), MaxHealth);

	//Energy
	/*this->playerEnergy = UGestureMathLibrary::CalculateValueFromPercentage(10.0f, this->MaxStamina, 100.0f) + UGestureMathLibrary::CalculateValueFromPercentage(10.0f, this->MaxStrength, 100.0f) + this->MaxEnergy;
	this->MaxEnergy = this->playerEnergy;
	this->EnergyPercentage = 1.0f;
	UE_LOG(LogTemp, Warning, TEXT("Energy : %f"), this->playerEnergy);*/

	this->playerEnergy = this->MaxEnergy;
	this->EnergyPercentage = 1.0f;
	UE_LOG(LogTemp, Warning, TEXT("Energy : %f"), this->playerEnergy);

	//Get vitality status from current fitness level when game starts
	//TotalVitalityFromFitness(0.7f, 0.2f, 0.1f);
	if (!isAI)
	{
		if (LevelName == UGameplayStatics::GetCurrentLevelName(this, true))
		{
			//SkillNames.AddUnique("Move");
			//SkillNames.AddUnique("Rotate");

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
		/*if (this->WidgetHUD)
		{
			if (WidgetHUD->IsVisible())
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
				//locUIChar->WidgetHUD = CharacterHUDClass;
				//GetWorld()->GetFirstPlayerController()->ShouldShowMouseCursor();
				//UE_LOG(LogTemp, Warning, TEXT("Show mouse cursor is %s "), (GetWorld()->GetFirstPlayerController()->ShouldShowMouseCursor() == GetWorld()->GetFirstPlayerController()->ShouldShowMouseCursor()) ? TEXT("True") : TEXT("False"));
				//GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("InitIsLoad.")));
			}
		}*/
		GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
		GetWorld()->GetFirstPlayerController()->ShouldShowMouseCursor();
	}
}

void ATodakBattleArenaCharacter::FindRowBlockAction(bool holdBlock, bool FaceBlock, bool RightBlock, FBlockActions& outValue)
{
	FString Context;

	//Search the skill available
	for (auto& name : BlockActions->GetRowNames())
	{
		FBlockActions* row = BlockActions->FindRow<FBlockActions>(name, Context);
		if (row)
		{
			if (row->HoldBlock == holdBlock && row->IsFaceBlock == FaceBlock && row->IsRightBlock == RightBlock && row->PauseAnimTime <= 0.0f)
			{
				outValue = *row;
			}
		}
	}
	return;
}

void ATodakBattleArenaCharacter::StartBlockHit(bool faceBlock, bool HoldBlock, float& ReturnLength)
{
	FBlockActions outValue;
	

	//check if the hit location on player body is on the right side
	bool RightBlock = UGestureMathLibrary::IsRightAngle(this->GetCapsuleComponent()->GetComponentLocation(), this->HitLocation);

	//get block montage from data table
	FindRowBlockAction(HoldBlock, faceBlock, RightBlock, outValue);

	//player block montage
	ServerSkillBlockHitMontage(outValue.BlockMoveset, outValue.StartAnimTime, outValue.PauseAnimTime, outValue.HoldBlock);
	//BlockMontageLength = outValue.BlockMoveset->GetPlayLength();
	//ReturnLength = outValue.BlockMovesetLength;

	//UE_LOG(LogTemp, Warning, TEXT("outValue : %s"), *outValue.BlockMoveset->GetFName().ToString());
}

bool ATodakBattleArenaCharacter::ServerSetEnemyMontage_Validate(UAnimMontage* HitReaction, FBlockActions BlockMovesets, float StaminaDrain, float StartPlay, float BlockVisibility)
{
	return true;
}

void ATodakBattleArenaCharacter::ServerSetEnemyMontage_Implementation(UAnimMontage* HitReaction, FBlockActions BlockMovesets, float StaminaDrain, float StartPlay, float BlockVisibility)
{
	MulticastSetEnemyMontage(HitReaction, BlockMovesets, StaminaDrain, StartPlay, BlockVisibility);
}

bool ATodakBattleArenaCharacter::MulticastSetEnemyMontage_Validate(UAnimMontage* HitReaction, FBlockActions BlockMovesets, float StaminaDrain, float StartPlay, float BlockVisibility)
{
	return true;
}

void ATodakBattleArenaCharacter::MulticastSetEnemyMontage_Implementation(UAnimMontage* HitReaction, FBlockActions BlockMovesets, float StaminaDrain, float StartPlay, float BlockVisibility)
{
	if (this->EnemyElement != nullptr)
	{
		this->EnemyElement->BlockHit = BlockMovesets;
		this->EnemyElement->HitReactionsMoveset = HitReaction;
		this->EnemyElement->staminaDrained = StaminaDrain;
		this->EnemyElement->ReactionStartTime = StartPlay;
		this->EnemyElement->BlockButtonLength = BlockVisibility; //how long the block button will be visible

		//SET BLOCK BUTTON VISIBLE
		OnRep_Block();
		//this->EnemyElement->WidgetHUD->CallShowButton();
		////ServerAssignBlockHit(BlockMovesets, HitReaction, StaminaDrain);
		//UE_LOG(LogTemp, Warning, TEXT("Name : %s"), *this->GetFName().ToString());
	}
}

void ATodakBattleArenaCharacter::GetDamageFromPhysicsAssetShapeName(FName ShapeName, float& MajorDamageDealt, float& MinorDamageDealt, bool& IsUpperBody, UAnimMontage* DamageMovesets)
{
	FKBoxElem boxElem;
	FKSphylElem capsuleElem;
	bool isFound = false;

	UE_LOG(LogTemp, Warning, TEXT("%s Bone name is : %s"), *ShapeName.ToString(), *ShapeName.ToString());

	//if capsule physics asset is valid
	if (GetMesh()->GetBodyInstance(ShapeName)->BodySetup.Get()->AggGeom.SphylElems.IsValidIndex(0))
	{ 
		//iterate through aggregate physics body
		for (FKSphylElem caps : GetMesh()->GetBodyInstance(ShapeName)->BodySetup.Get()->AggGeom.SphylElems)
		{
			//UE_LOG(LogTemp, Warning, TEXT("%s Capsule name is : %s"), *ShapeName.ToString(), *caps.GetName().ToString());
			capsuleElem = caps;
			isFound = true;
			break;
		}

		UE_LOG(LogTemp, Warning, TEXT("%s Sphere name is : %s"), *ShapeName.ToString(), *ShapeName.ToString());

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
				if (row->DamageBlockMoveset != NULL)
				{
					//DamageMovesets = row->DamageMoveset;
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
				if (row->DamageBlockMoveset != NULL)
				{
					//DamageMovesets = row->DamageMoveset;
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
	//reset the bool so sweep trace can be executed again
	//DoOnce = false;
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
	canMove = true;
	SkillTriggered = false;

	////Update stats after anim is played
	//if (GetWorld()->GetTimerManager().IsTimerActive(StartEnergyTimer) == false && (this->playerEnergy <= this->MaxEnergy))
	//{
	//	//Set timer for EnergyBar to regen after action
	//	FTimerDelegate FunctionsNames;
	//	FunctionsNames = FTimerDelegate::CreateUObject(this, &ATodakBattleArenaCharacter::UpdateCurrentPlayerMainStatusBar, EBarType::PrimaryProgressBar, EMainPlayerStats::Energy, this->StartEnergyTimer, this->StartEnergyTimer);

	//	UE_LOG(LogTemp, Warning, TEXT("EnergyTimer has started!"));
	//	GetWorld()->GetTimerManager().SetTimer(this->StartEnergyTimer, FunctionsNames, EnergyRate, true);

	//	//UE_LOG(LogTemp, Warning, TEXT("Timer has started!"));
	//	//GetWorld()->GetTimerManager().SetTimer(StartEnergyTimer, this, &ATodakBattleArenaCharacter::UpdateEnergyStatusBar, 1.5f, true, 2.0f);
	//}
}

void ATodakBattleArenaCharacter::SimulatePhysicRagdoll(AActor* RagdolledActor)
{
	//UE_LOG(LogTemp, Warning, TEXT("Timer Starts"));
	this->GetWorldTimerManager().ClearTimer(FallTimerHandle);

	//Disable hair physics
	this->Hair->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);

	//Start simulate body
	this->GetMesh()->SetAllBodiesBelowSimulatePhysics("pelvis", true, false);
	//this->GetMesh()->SetAllBodiesBelowSimulatePhysics("pelvis", true, false);

	//Stop any movement
	this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	//Delay before simulate physics
	this->GetWorldTimerManager().SetTimer(FallTimerHandle, 3.0f, false);

	float currHp = UGestureMathLibrary::CalculatePercentageFromValue(this->Health, this->MaxHealth, 100.0f);

	if (currHp < 100.0f)
	{
		if (currHp < 80.0f)
		{
			if (this->IsLocallyControlled())
			{
				this->CallGetUpFunction(RagdolledActor, this->GetMesh());
			}
		}
		else
		{
			if (this->IsLocallyControlled())
			{
				this->CallEventTimerFunction();
			}
		}
	}
	else
	{
		if (this->IsLocallyControlled())
		{
			this->CallEventLoseFunction();
		}
	}
}

void ATodakBattleArenaCharacter::CallFallRagdoll(AActor* RagdolledActor, bool IsLookingAtTarget)
{
	if (RagdolledActor == this)
	{
		//Disable input
		this->DisableInput(UGameplayStatics::GetPlayerController(this, 0));

		//Stop movement
		this->GetCharacterMovement()->StopMovementImmediately();

		//Start ragdoll
		if (this->IsLocallyControlled())
		{
			if (IsLookingAtTarget == true)
			{
				this->ServerFallRagdoll(RagdolledActor, FallBackAnimChar);
			}
			else
				this->ServerFallRagdoll(RagdolledActor, FallFrontAnimChar);
		}
	}
}

void ATodakBattleArenaCharacter::RagdollCamera()
{
	if (CameraPerspective == 0)
	{
		//Sets player to topview on ragdoll
		this->FollowCamera->SetFieldOfView(120.0f);
		FLatentActionInfo LatentInfo = FLatentActionInfo();
		LatentInfo.CallbackTarget = this;
		UKismetSystemLibrary::MoveComponentTo(this->FollowCamera, FVector(300.0f, 0.0f, 300.0f), FRotator(-90.0f, -55.0f, 0.0f), true, true, 3.0f, true, EMoveComponentAction::Type::Move, LatentInfo);

	}
}

bool ATodakBattleArenaCharacter::ServerFallRagdoll_Validate(AActor* RagdolledActor, UAnimSequenceBase* FallAnims)
{
	return true;
}

void ATodakBattleArenaCharacter::ServerFallRagdoll_Implementation(AActor* RagdolledActor, UAnimSequenceBase* FallAnims)
{
	if (this->GetLocalRole() == ROLE_Authority)
	{
		this->MulticastFallRagdoll(RagdolledActor, FallAnims);
	}
}

bool ATodakBattleArenaCharacter::MulticastFallRagdoll_Validate(AActor* RagdolledActor, UAnimSequenceBase* FallAnims)
{
	return true;
}

void ATodakBattleArenaCharacter::MulticastFallRagdoll_Implementation(AActor* RagdolledActor, UAnimSequenceBase* FallAnims)
{
	if (this->GetLocalRole() == ROLE_Authority)
	{
		if (IsRunningDedicatedServer() == true)
		{
			return;
		}
		else
		{
			goto Fall;
		}
	}
	else
	{
		Fall:
		UTBAAnimInstance* currAnimInst = Cast<UTBAAnimInstance>(this->GetMesh()->GetAnimInstance());
		currAnimInst->FallAnim = FallAnims;
		if (currAnimInst->FallAnim != nullptr)
		{
			currAnimInst->RagdollMode = true;
			if (currAnimInst->RagdollMode == true)
			{
				FTimerDelegate RagdollDel = FTimerDelegate::CreateUObject(this, &ATodakBattleArenaCharacter::SimulatePhysicRagdoll, RagdolledActor);
				this->GetWorld()->GetTimerManager().SetTimer(FallTimerHandle, RagdollDel, currAnimInst->FallAnim->SequenceLength, false);
				UE_LOG(LogTemp, Warning, TEXT("Timer Starts"));
			}
		}
	}
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

bool ATodakBattleArenaCharacter::DoDamage_Validate(AActor* HitActor)
{
	return true;
}

void ATodakBattleArenaCharacter::DoDamage_Implementation(AActor* HitActor)
{
	if (this != HitActor)
	{
		//ApplyDamage
		this->damage = UGameplayStatics::ApplyDamage(HitActor, this->damage, nullptr, this, nullptr);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Damage Applied: %f"), this->damage));
		UE_LOG(LogTemp, Warning, TEXT("Damage Applied: %f"), this->damage);
		//DrawDebugSphere(GetWorld(), Start, SphereKick.GetSphereRadius(), 2, FColor::Purple, false, 1, 0, 1);
		//reset the bool so sweep trace can be executed again
		//DoOnce = false;
		//LeftKickColActivate = false;
	}
}

bool ATodakBattleArenaCharacter::ServerEnergySpent_Validate(float ValDecrement, float PercentageLimit, float MontageDuration)
{
	return true;
}

void ATodakBattleArenaCharacter::ServerEnergySpent_Implementation(float ValDecrement, float PercentageLimit, float MontageDuration)
{
	EnergySpent(ValDecrement, PercentageLimit, MontageDuration);
}

bool ATodakBattleArenaCharacter::EnergySpent_Validate(float ValDecrement, float PercentageLimit, float MontageDuration)
{
	return true;
}

void ATodakBattleArenaCharacter::EnergySpent_Implementation(float ValDecrement, float PercentageLimit, float MontageDuration)
{
	FTimerHandle Delay;

	//Reduce energy from current energy
	float tempEnergy = this->playerEnergy - ValDecrement;

	if (tempEnergy <= 0.0f)
	{
		this->playerEnergy = 0.0f;
	}
	else
		this->playerEnergy = tempEnergy;

	OnRep_CurrentEnergy();

	//UE_LOG(LogTemp, Warning, TEXT("Energy Remains: %f"), this->playerEnergy);

	//this->EnergyPercentage = UGestureInputsFunctions::UpdateProgressBarComponent(this->WidgetHUD, "EnergyBar", "EnergyText", "Energy", "Energy", this->playerEnergy, this->MaxEnergy);

	if (MontageDuration > 0.0f)
	{
		this->GetWorld()->GetTimerManager().ClearTimer(Energystart);

		/*FTimerDelegate FunctionsNames;
		FunctionsNames = FTimerDelegate::CreateUObject(this, &ATodakBattleArenaCharacter::EnergyStatusDelay, this->playerEnergy, this->MaxEnergy);
*/
		this->GetWorld()->GetTimerManager().SetTimer(Energystart, this, &ATodakBattleArenaCharacter::EnergyStatusDelay, MontageDuration + 1.0f, false);

		//UpdateProgressBarValue(this->playerEnergy, this->MaxEnergy);


		//UpdateProgressBarValue(this, this->playerEnergy, this->MaxEnergy);

	}
	else if (MontageDuration <= 0.0f)
	{
		this->GetWorld()->GetTimerManager().ClearTimer(Energystart);

		/*FTimerDelegate FunctionsNames;
		FunctionsNames = FTimerDelegate::CreateUObject(this, &ATodakBattleArenaCharacter::EnergyStatusDelay, this->playerEnergy, this->MaxEnergy);
		*/

		this->GetWorld()->GetTimerManager().SetTimer(Energystart, this, &ATodakBattleArenaCharacter::EnergyStatusDelay, MontageDuration + 1.0f, false);
		//UpdateProgressBarValue(this, this->playerEnergy, this->MaxEnergy);
	}


	//if current player is doing the action
	if (this->IsLocallyControlled())
	{
		
		//else if (MontageDuration <= 0.0f)
		//{
		//	//Update stats after anim is played
		//	if (GetWorld()->GetTimerManager().IsTimerActive(StartEnergyTimer) == false && (this->playerEnergy <= this->MaxEnergy))
		//	{
		//		//Set timer for EnergyBar to regen after action
		//		/*FTimerDelegate FunctionsNames;
		//		FunctionsNames = FTimerDelegate::CreateUObject(this, &ATodakBattleArenaCharacter::UpdateCurrentPlayerMainStatusBar, EBarType::PrimaryProgressBar, EMainPlayerStats::Energy, this->StartEnergyTimer, this->StartEnergyTimer);*/
		//		EnergyStatusDelay();
		//		//UE_LOG(LogTemp, Warning, TEXT("EnergyTimer has started!"));
		//		//GetWorld()->GetTimerManager().SetTimer(this->StartEnergyTimer, FunctionsNames, EnergyRate, true);
		//		//UpdateProgressBarValue(this, this->playerEnergy, this->MaxEnergy);

		//		//UE_LOG(LogTemp, Warning, TEXT("Timer has started!"));
		//		//GetWorld()->GetTimerManager().SetTimer(StartEnergyTimer, this, &ATodakBattleArenaCharacter::UpdateEnergyStatusBar, 1.5f, true, 2.0f);
		//	}
		//}
	}
	// duration to wait for montage finished playing
	/*this->GetWorld()->GetTimerManager().SetTimer(Delay, this, &ATodakBattleArenaCharacter::ResetMovementMode, MontageDuration, false);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Timer remaining: %f"), this->GetWorld()->GetTimerManager().GetTimerRemaining(Delay)));*/

	////Update stats after anim is played
	//if (GetWorld()->GetTimerManager().IsTimerActive(StartEnergyTimer) == false && (this->playerEnergy <= this->MaxEnergy))
	//{
	//	//Set timer for EnergyBar to regen after action
	//	FTimerDelegate FunctionsNames;
	//	FunctionsNames = FTimerDelegate::CreateUObject(this, &ATodakBattleArenaCharacter::UpdateCurrentPlayerMainStatusBar, EBarType::PrimaryProgressBar, EMainPlayerStats::Energy, this->StartEnergyTimer, this->StartEnergyTimer);

	//	UE_LOG(LogTemp, Warning, TEXT("EnergyTimer has started!"));
	//	GetWorld()->GetTimerManager().SetTimer(this->StartEnergyTimer, FunctionsNames, EnergyRate, true);

	//	//UE_LOG(LogTemp, Warning, TEXT("Timer has started!"));
	//	//GetWorld()->GetTimerManager().SetTimer(StartEnergyTimer, this, &ATodakBattleArenaCharacter::UpdateEnergyStatusBar, 1.5f, true, 2.0f);
	//}
	//OnRep_CurrentEnergy();

	//Update energy after action on progress bar
	/*this->WidgetHUD->ChangeProgressBarValue(this, this->playerEnergy, this->MaxEnergy, this->EnergyPercentage);
	UE_LOG(LogTemp, Warning, TEXT("Energy Remains: %f"), this->playerEnergy);*/
	/*UE_LOG(LogTemp, Warning, TEXT("Energy Remains: %f"), this->playerEnergy);
	this->EnergyPercentage = UGestureInputsFunctions::UpdateProgressBarComponent(this->WidgetHUD, "EnergyBar", "EnergyText", "Energy", "Energy", this->playerEnergy, this->MaxEnergy);
	this->UpdateProgressBarValue(this, this->playerEnergy, this->MaxEnergy);*/

	/*if (this->WidgetHUD->GetOwningPlayer() != this->GetPlayerControllers())
	{
		EnergyPercentage = UGestureInputsFunctions::UpdateProgressBarComponent(WidgetHUD, "EnergyBar_1", "EnergyText_1", "Energy", "Energy", playerEnergy, MaxEnergy);
	}*/
	//EnemyElement->EnergyPercentage = UGestureInputsFunctions::UpdateProgressBarComponent(this->WidgetHUD, "EnergyBar_1", "EnergyText_1", "Energy", "Energy", EnemyElement->playerEnergy, EnemyElement->MaxEnergy);

	/*else if(this == EnemyElement)
	{
		this->EnergyPercentage = UGestureInputsFunctions::UpdateProgressBarComponent(this->WidgetHUD, "EnergyBar_1", "EnergyText_1", "Energy", "Energy", this->playerEnergy, this->MaxEnergy);
	}*/
	
	////Update energy after action on progress bar
	//UE_LOG(LogTemp, Warning, TEXT("Energy Remains: %f"), this->playerEnergy);
	//this->EnergyPercentage = UGestureInputsFunctions::UpdateProgressBarComponent(this->WidgetHUD, "EnergyBar", "EnergyText", "Energy", "Energy", this->playerEnergy, this->MaxEnergy);

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
	//UE_LOG(LogTemp, Warning, TEXT("Check for action"));
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

void ATodakBattleArenaCharacter::DetectInputTouch(ETouchIndex::Type FingerIndex, FVector Location, ETouchInputType Branches, ETouchIndex::Type IsTouchIndex)
{
	float newTimer = 0.0f;
	FVector2D newLoc;
	if (Branches == ETouchInputType::Pressed)
	{
		EnableTouch = true;
		if (CanSwipeAction == true && playerEnergy > 0.0f)
		{
			if (FingerIndex == IsTouchIndex)
			{
				newLoc = FVector2D(Location.X, Location.Y);
				StartDetectSwipe(FingerIndex, newLoc, newTimer);
				//RBranches = Branches;
			}
		}
	}
	else if (Branches == ETouchInputType::Released)
	{
		EnableTouch = false;
		if (CanSwipeAction == true && playerEnergy > 0.0f)
		{
			if (FingerIndex == IsTouchIndex)
			{
				newLoc = FVector2D(Location.X, Location.Y);
				StopDetectTouch(FingerIndex, newTimer, newLoc);
				//RBranches = Branches;
			}
		}
	}
	else if (Branches == ETouchInputType::Moved)
	{
		if (CanSwipeAction == true && playerEnergy > 0.0f)
		{
			if (FingerIndex == IsTouchIndex)
			{
				newLoc = FVector2D(Location.X, Location.Y);
				DetectTouchMovement(FingerIndex, newLoc);
				//RBranches = Branches;
			}
		}
	}
}

bool ATodakBattleArenaCharacter::ServerStartEnergyTimer_Validate(float currVal, int MaxVal, float rate)
{
	return true;
}

void ATodakBattleArenaCharacter::ServerStartEnergyTimer_Implementation(float currVal, int MaxVal, float rate)
{
	//start energy timer on the server
	FTimerDelegate FunctionsNames = FTimerDelegate::CreateUObject(this, &ATodakBattleArenaCharacter::ServerUpdateEnergyBar, this->playerEnergy, MaxVal);
	this->GetWorld()->GetTimerManager().SetTimer(this->StartEnergyTimer, FunctionsNames, rate, true);
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

void ATodakBattleArenaCharacter::OnRep_CurrentEnergy()
{
	//UE_LOG(LogTemp, Warning, TEXT("Energy Remains: %f"), this->playerEnergy);
	if (this->IsLocallyControlled() && isAI == false)
	{
		this->EnergyPercentage = UGestureInputsFunctions::UpdateProgressBarComponent(this->WidgetHUD, "EnergyBar", "EnergyText", "Energy", "Energy", this->playerEnergy, this->MaxEnergy);
	}
	//UpdateProgressBarValue(this->playerEnergy, this->MaxEnergy);

	if (!this->IsLocallyControlled())
	{
		float EnergyPercentage1 = UGestureInputsFunctions::UpdateProgressBarComponent(this->WidgetHUD, "EnergyBar_1", "EnergyText_1", "Energy", "Energy", this->playerEnergy, this->MaxEnergy);

		/*float EnergyPercentage1 = 0.0f;*/
		//float EnergyPercentage1 = UGestureInputsFunctions::UpdateProgressBarComponent(this->WidgetHUD, "EnergyBar_1", "EnergyText_1", "Energy", "Energy", tempEnergy, this->MaxEnergy);
		//UBaseCharacterWidget::ChangeProgressBarValue(this->WidgetHUD, tempEnergy, this->MaxEnergy, EnergyPercentage1);
		//UpdateProgressBarValue(this, tempEnergy, this->MaxEnergy);
	}
	else if (this->IsLocallyControlled() && isAI == true)
	{
		this->EnergyPercentage = UGestureInputsFunctions::UpdateProgressBarComponent(this->WidgetHUD, "EnergyBar_1", "EnergyText_1", "Energy", "Energy", this->playerEnergy, this->MaxEnergy);
	}
}

void ATodakBattleArenaCharacter::OnRep_Health()
{
	if (this->IsLocallyControlled() && isAI == false)
	{
		this->playerHealth = UGestureInputsFunctions::UpdateProgressBarComponent(this->WidgetHUD, "HPBar", "Health", "HP", "Pain Meter", this->Health, this->MaxHealth);
	}
	if (!this->IsLocallyControlled())
	{
		float playerHealth1 = UGestureInputsFunctions::UpdateProgressBarComponent(this->WidgetHUD, "HPBarMain_1", "Health_1", "HP", "Pain Meter", this->Health, this->MaxHealth);
	}
	else if (this->IsLocallyControlled() && isAI == true)
	{
		this->playerHealth = UGestureInputsFunctions::UpdateProgressBarComponent(this->WidgetHUD, "HPBarMain_1", "Health_1", "HP", "Pain Meter", this->Health, this->MaxHealth);
	}
}

//void ATodakBattleArenaCharacter::OnRep_CurrentEnergy()
//{
//	//Update energy after action on progress bar
//	UE_LOG(LogTemp, Warning, TEXT("Energy Remains: %f"), this->playerEnergy);
//	this->EnergyPercentage = UGestureInputsFunctions::UpdateProgressBarComponent(this->WidgetHUD, "EnergyBar", "EnergyText", "Energy", "Energy", this->playerEnergy, this->MaxEnergy);
//}


/***********************************************************************END_STATUS*******************************************************************************************************************/

void ATodakBattleArenaCharacter::ReduceDamageTaken(float damageValue, float CurrStrength, float CurrStamina, float CurrAgility)
{
	this->damageAfterReduction = damageValue - ((CurrStrength + CurrStamina + CurrAgility) / 15000)*(CurrStrength*(CurrStrength / 1000.0f));
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
	//UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
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
	}
	else
		return;
}

void ATodakBattleArenaCharacter::RemoveElementFromArrayTimer()
{
	if (BodyParts.IsValidIndex(0) && GetWorld()->GetTimerManager().IsTimerActive(IterateArray))
	{
		//UE_LOG(LogTemp, Warning, TEXT("%s is removed"), *KeyName[0].KeyInput.ToString());
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("%s is removed"), *KeyName[0].KeyInput.ToString()));

		//Remove key at index 0;
		//SwipeActions.RemoveAt(0);
		BodyParts.RemoveAt(0);

		//(SwipeActions.Num() < 1 || BodyParts.Num() < 1)

		//if array is empty and timer still active, clear the timer
		if (GetWorld()->GetTimerManager().IsTimerActive(IterateArray) == true && BodyParts.Num() < 1)
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

void ATodakBattleArenaCharacter::EnergyStatusDelay()
{
	//Update stats after anim is played
	if (this->IsLocallyControlled())
	{
		//if energy is lower than max, init server timer
		if (this->playerEnergy < this->MaxEnergy)
		{
			//initialize timer on the server
			ServerStartEnergyTimer(this->playerEnergy, this->MaxEnergy, this->EnergyRate);
		}
	}

	//	//Set timer for EnergyBar to regen after action
	//	/*FTimerDelegate FunctionsNames;
	//	FunctionsNames = FTimerDelegate::CreateUObject(this, &ATodakBattleArenaCharacter::UpdateCurrentPlayerMainStatusBar, EBarType::PrimaryProgressBar, EMainPlayerStats::Energy, this->StartEnergyTimer, this->StartEnergyTimer);

	//	UE_LOG(LogTemp, Warning, TEXT("EnergyTimer has started!"));
	//	GetWorld()->GetTimerManager().SetTimer(this->StartEnergyTimer, FunctionsNames, EnergyRate, true);*/

	//	/*UE_LOG(LogTemp, Warning, TEXT("Timer has started!"));
	//	GetWorld()->GetTimerManager().SetTimer(StartEnergyTimer, this, &ATodakBattleArenaCharacter::UpdateEnergyStatusBar, 1.5f, true, 2.0f);*/
	//}
}

void ATodakBattleArenaCharacter::StartDetectSwipe(ETouchIndex::Type FingerIndex, FVector2D Locations, float& StartPressTime)
{
	if (!isAI)
	{
		FTimespan currTimeSpan = UGestureMathLibrary::GetCurrentTime();

		//Get current touch start time in seconds
		startTouch = currTimeSpan.GetTotalSeconds();

		UE_LOG(LogTemp, Warning, TEXT("Swipe Detect"));
		//Temp var
		FFingerIndex NewIndex;

		NewIndex.StartLocation = Locations;
		NewIndex.IsPressed = true;
		NewIndex.FingerIndex = FingerIndex;
		NewIndex.SwipeActions = EInputType::Pressed;
		NewIndex.bDo = false;

		//Assign to global val
		CurrFingerIndex = &NewIndex;

		if (InputTouch.IsValidIndex(0) == false)
		{
			//if current touch index does not exist, add it to array
			InputTouch.Add(NewIndex);

			int Index = InputTouch.Find(NewIndex);
			//if (InputTouch[Index].IsPressed == true)
			//{
			//	if (InputStyle == EInputStyle::Default)
			//	{
			//		//Checks for touch within the input area
			//		UGestureInputsFunctions::CircleSwipeArea(this, &InputTouch[Index], InputTouch[Index].StartLocation);
			//	}
			//	else if (InputStyle == EInputStyle::LeftJoystick)
			//	{
			//		//Checks for touch within the input area
			//		UGestureInputsFunctions::RightSwipeArea(this, &InputTouch[Index], InputTouch[Index].StartLocation);
			//	}
			//	//TArray<EBodyPart>& InputPart = BodyParts;

			//	//Used in error reporting
			//	FString Context;

			//	RowNames = ActionTable->GetRowNames();

			//	//iterate through datatable
			//	for (auto& name : RowNames)
			//	{
			//		FActionSkill* row = ActionTable->FindRow<FActionSkill>(name, Context);
			//		if (row)
			//		{
			//			//Check if the input is same as the input needed to execute the skill
			//			if (row->BodyParts.Contains(InputTouch[Index].BodyParts))
			//			{
			//				SwipeParts = InputTouch[Index].BodyParts;
			//				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Touch index is %s"), (*GETENUMSTRING("ETouchIndex", InputTouch[Index].FingerIndex))));
			//				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Touch swipeactions is %s"), (*GETENUMSTRING("EInputType", InputTouch[Index].SwipeActions))));
			//				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Emerald, FString::Printf(TEXT("Touch bdo is %s"), (InputTouch[Index].bDo) ? TEXT("True") : TEXT("False")));
			//				//SkillHold = row->StartAnimMontage;

			//				//if current row->StopHoldAnimTime is not empty
			//				//SkillStopTime = row->StopHoldAnimTime;
			//				//BlockHit = row->SkillBlockHit;
			//				/*if (row->StopHoldAnimTime.Num() > 0)
			//				{
			//					SkillStopTime = row->StopHoldAnimTime;
			//					BlockHit = row->SkillBlockHit;
			//				}*/

			//				//play animation on press
			//				//this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
			//				//canMove = false; //causes delay input after actionskill montage is played
			//				
			//				//TouchIsHold = true;

			//				if (IsLocallyControlled())
			//				{
			//					//ServerSkillStartMontage(SkillHold, row->StartHoldMontageTime, SkillStopTime);
			//				}
			//				//ServerSkillBlockHitMontage(row->SkillBlockHit);
			//				break;
			//			}
			//		}
			//	}
			//}
		}
		else
		{
			InputTouch.Empty();
		}
	}
}

void ATodakBattleArenaCharacter::DetectTouchMovement(ETouchIndex::Type FingerIndex, FVector2D Locations)
{
	if (InputTouch.IsValidIndex(0) == true)
	{
		EInputType Branches = EInputType::Pressed;
		//float currSwipeTime = FPlatformTime::Seconds();

		//SwipeStartTime = FPlatformTime::Seconds();
		TArray<FFingerIndex>& Touches = InputTouch;

		for (FFingerIndex& TouchIndex : Touches)
		{
			if (TouchIndex.FingerIndex == FingerIndex)
			{
				if (TouchIndex.IsPressed == true)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, FString::Printf(TEXT("Touch bdo is %s"), (TouchIndex.bDo) ? TEXT("True") : TEXT("False")));

					//Update finger position every    
					if (TouchIndex.bDo == false)
					{
						//if the current finger position is more than 0 units from starting point
						if (((TouchIndex.StartLocation - Locations).Size() > 50.0f) && ((TouchIndex.StartLocation - Locations).Size() < 1000.0f))
						{
							float sample = (TouchIndex.StartLocation - Locations).Size();
							UE_LOG(LogTemp, Warning, TEXT("current dist : %f"), sample);
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
								if (InputStyle == EInputStyle::Default)
								{
									if (UGestureInputsFunctions::DetectLinearSwipe(TouchIndex.StartLocation, Locations, TouchIndex.SwipeActions, TouchIndex.bDo, TouchIndex.Points) == true)
									{
										GetSkillAction(&TouchIndex);
										//FingerIndex->bDo = true;
										TouchIndex.StartLocation = FVector2D(0, 0);
										//PlayerChar->SwipeActions.Add(Branches);
										//BodyParts.Add(EBodyPart::LeftHand);
										//TouchIndex.SwipeActions = Branches;
										//RemoveFromArray();
										TouchIndex.Points.Empty();
										return;
										//if ((TouchIndex.StartLocation - Locations).Size() > 50.0f)
										//{
										//	GetSkillAction(&TouchIndex);
										//	//FingerIndex->bDo = true;
										//	TouchIndex.StartLocation = FVector2D(0, 0);
										//	//PlayerChar->SwipeActions.Add(Branches);
										//	//BodyParts.Add(EBodyPart::LeftHand);
										//	//TouchIndex.SwipeActions = Branches;
										//	//RemoveFromArray();
										//	TouchIndex.Points.Empty();
										//	return;
										//}
									}

									//UGestureInputsFunctions::CircleSwipeArea(this, &TouchIndex, Locations);
								}
								else if (InputStyle == EInputStyle::LeftJoystick)
								{
									//UGestureInputsFunctions::RightSwipeArea(this, &TouchIndex, Locations);
								}
								//SwipeDir = TouchIndex.SwipeActions;
								if (EnableMovement == true)
								{
									EnableMovement = false;
								}
								//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, FString::Printf(TEXT("Touch bdo is true")));

								//Get skill combos
								//GetSkillAction(&TouchIndex);
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

void ATodakBattleArenaCharacter::StopDetectTouch(ETouchIndex::Type FingerIndex, float StartPressTime, FVector2D Locations)
{
	if (!isAI)
	{
		FFingerIndex NewIndex;
		NewIndex.FingerIndex = FingerIndex;

		UE_LOG(LogTemp, Warning, TEXT("Swipe Released"));
		UE_LOG(LogTemp, Warning, TEXT("Location : %s") , *Locations.ToString());
		FTimespan currTimeSpan = UGestureMathLibrary::GetCurrentTime();

		//Get current touch start time in seconds
		double stopTouch = currTimeSpan.GetTotalSeconds();
		//DoOnce = false;

		if (InputTouch.IsValidIndex(0) == true)
		{
			if (InputTouch.Contains(NewIndex))
			{
				//FVector2D currTouchLoc;
				//playerController->GetInputTouchState(NewIndex.FingerIndex, currTouchLoc.X, currTouchLoc.Y, IsPressed);
				// && ((InputTouch[Index].StartLocation - NewIndex.StartLocation).Size() < 50.0f)
				int32 Index = InputTouch.Find(NewIndex);
				NewIndex.StartLocation = Locations;
				//if (stopTouch - startTouch < 0.2f)
				//{
				//	if (InputStyle == EInputStyle::Default)
				//	{
				//		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, FString::Printf(TEXT("Tap")));
				//		UGestureInputsFunctions::CircleSwipeArea(this, &NewIndex, NewIndex.StartLocation);
				//	}
				//	else if (InputStyle == EInputStyle::LeftJoystick)
				//	{
				//		UGestureInputsFunctions::RightSwipeArea(this, &NewIndex, NewIndex.StartLocation);
				//	}
				//	NewIndex.SwipeActions = EInputType::Tap;
				//	//SwipeDir = TouchIndex.SwipeActions;
				//	if (EnableMovement == true)
				//	{
				//		EnableMovement = false;
				//	}
				//	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, FString::Printf(TEXT("Touch bdo is true")));

				//	//Get skill combos
				//	GetSkillAction(&NewIndex);
				//}
				if (BlockedHit == true)
				{
					BlockedHit = false;
				}
				/*if (GetMesh()->GetAnimInstance()->Montage_IsActive(SkillHold) == true)
				{
					//Stop current active anim
					if (IsLocallyControlled())
					{
						ServerSkillMoveset(SkillHold, this->damage, this->MaxStrength, this->MaxStamina, this->MaxAgility, 1.0f, 0.0f, false);
						//ServerSkillMoveset(SkillHold, this->damage, this->MaxStrength, this->MaxStamina, this->MaxAgility, 1.0f, 0.0f, false, FName(""));
					}
				}*/
				//if touch index is found, remove from array
				InputTouch.RemoveAt(Index);
			}
			InputTouch.Empty();
		}
		TouchIsHold = false;
		startTouch = 0.0f;
		/*bool IsFound = false;
		EnableMovement = false;
		RightFoot = false;
		LeftFoot = false;
		BlockedHit = false;*/
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
	//UE_LOG(LogTemp, Warning, TEXT("%f"), Rate);
	// calculate delta for this frame from the rate information
	//AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());

	//if (AActor::GetInputAxisKeyValue(InputAxisKey)
	//UE_LOG(LogTemp, Warning, TEXT("%f"), Rate);
	//float Value = InputComponent->AActor::GetAxisKeyValue(InputAxisKey);
	//float Value = AActor::GetInputAxisKeyValue(Rate);
	//float Value = InputComponent->GetInputAxisKeyValue(Rate);


	//if ((Controller != NULL) && (Rate != 0.0f))
	//{
	//	if (Rate != 0.0f)
	//	{
	//		if (Rate > 0.0f)
	//		{
	//			// turn right
	//			TurnRight = true;
	//			TurnLeft = false;
	//			UE_LOG(LogTemp, Warning, TEXT("TurnRight: %s"), TurnRight ? TEXT("true") : TEXT("false"));
	//			UE_LOG(LogTemp, Warning, TEXT("Is Moving: %s"), IsMoving ? TEXT("true") : TEXT("false"));

	//		}

	//		else
	//		{
	//			// turn left
	//			TurnLeft = true;
	//			TurnRight = false;
	//			UE_LOG(LogTemp, Warning, TEXT("TurnLeft: %s"), TurnLeft ? TEXT("true") : TEXT("false"));

	//		}
	//	}
	//	
	//	else
	//	{
	//		// idle
	//		TurnRight = false;
	//		TurnLeft = false;
	//	}

	//}
	



	/*if (Rate > 0.0)
	{
		TurnLeft = false;
		TurnRight = true;
	}

	else
	{
		if (Rate < 0.0)
		{
			TurnRight = false;
			TurnLeft = true;
		}
		
		else if (Rate == 0.0)
		{
			TurnRight = false;
			TurnLeft = false;
		}
		
	}*/

	
}

void ATodakBattleArenaCharacter::LookUpAtRate(float Rate)
{
	//UE_LOG(LogTemp, Warning, TEXT("%f"), Rate);
	// calculate delta for this frame from the rate information
	//AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATodakBattleArenaCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		if (canMove)
		{
			//// find out which way is forward
			//const FRotator Rotation = Controller->GetControlRotation();
			//const FRotator YawRotation(0, Rotation.Yaw, 0);

			//// get forward vector
			//const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			//// add movement in that direction
			//AddMovementInput(Direction, Value);

			const FRotator Rotation = Controller->GetControlRotation();
			const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);

			AddMovementInput(Direction, Value);
		}
		
	}
}

void ATodakBattleArenaCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{		
		if (canMove)
		{
			//// find out which way is right
			//const FRotator Rotation = Controller->GetControlRotation();
			//const FRotator YawRotation(0, Rotation.Yaw, 0);

			//// get right vector 
			//const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			//// add movement in that direction
			//AddMovementInput(Direction, Value);

			const FRotator Rotation = Controller->GetControlRotation();
			const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);

			AddMovementInput(Direction, Value);
		}
	}
	RightVal = Value;
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

void ATodakBattleArenaCharacter::OnCombatColl(UCapsuleComponent* CombatColl)
{
	CombatColl->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CombatColl->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

void ATodakBattleArenaCharacter::OffCombatColl(UCapsuleComponent * CombatColl)
{
	CombatColl->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DoOnce = false;
}

void ATodakBattleArenaCharacter::FPPToFarFloatReturn(float val)
{
	ChangingCamera = val;
	this->CameraBoom->TargetArmLength = ChangingCamera;
}

void ATodakBattleArenaCharacter::FarToTPPFloatReturn(float val)
{
	ChangingCamera = val;
	this->CameraBoom->TargetArmLength = ChangingCamera;
}

void ATodakBattleArenaCharacter::CameraShake()
{
	UGameplayStatics::PlayWorldCameraShake(this->GetWorld()->GetFirstPlayerController(), DamageCameraShake, this->GetActorLocation(), 0.0f, 300.0f, 1.0f, false);
}

bool ATodakBattleArenaCharacter::ServerEffectiveBlockTimer_Validate()
{
	return true;
}
void ATodakBattleArenaCharacter::ServerEffectiveBlockTimer_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		MulticastEffectiveBlockTimer();

	}
	
}

bool ATodakBattleArenaCharacter::MulticastEffectiveBlockTimer_Validate()
{
	return true;
}

void ATodakBattleArenaCharacter::MulticastEffectiveBlockTimer_Implementation()
{
	FTimerHandle Delay;
	this->IsEffectiveBlock = true;

	// Delay 0.5 seconds then run EffectiveBlockTimer()
	this->GetWorld()->GetTimerManager().SetTimer(Delay, this, &ATodakBattleArenaCharacter::ServerToggleEffectiveBlock, 0.1f, false);
	UE_LOG(LogTemp, Warning, TEXT("EFFECTIVE BLOCK TIMER"));
}

bool ATodakBattleArenaCharacter::ServerToggleEffectiveBlock_Validate()
{
	return true;
}

void ATodakBattleArenaCharacter::ServerToggleEffectiveBlock_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		MulticastToggleEffectiveBlock();

	}
	
}

bool ATodakBattleArenaCharacter::MulticastToggleEffectiveBlock_Validate()
{
	return true;
}

void ATodakBattleArenaCharacter::MulticastToggleEffectiveBlock_Implementation()
{
	// Hit Block Reaction during Effective Blocking
	if (this->IsEffectiveBlock == true)
	{

		this->IsEffectiveBlock = false;
	}
}

void ATodakBattleArenaCharacter::EndingEffectiveBlock()
{
	if (IsLocallyControlled()) 
	{
		ServerSlowmo(1.0f);
		//Cast<ATodakBattleArenaPlayerController>(GetController())->ToggleOffInput();
		//DisableInput(GetWorld()->GetFirstPlayerController());
	}

	this->CanSwipeAction = true;

	if (this->EnemyElement != nullptr)
	{
		this->EnemyElement->CanSwipeAction = true;
	}
}

bool ATodakBattleArenaCharacter::ServerBlockReaction_Validate(UAnimMontage * ServerSkill, bool bEffBlock)
{
	return true;
}

void ATodakBattleArenaCharacter::ServerBlockReaction_Implementation(UAnimMontage * ServerSkill, bool bEffBlock)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		RPCServerBlockReaction = ServerSkill;
		MulticastBlockReaction(RPCServerBlockReaction, bEffBlock);

	}
}

bool ATodakBattleArenaCharacter::MulticastBlockReaction_Validate(UAnimMontage * MulticastSkill, bool bEffBlock)
{
	return true;
}

void ATodakBattleArenaCharacter::MulticastBlockReaction_Implementation(UAnimMontage * MulticastSkill, bool bEffBlock)
{
	this->IsEffectiveBlock = bEffBlock;

	//checks player is in effective block or not
	if (IsEffectiveBlock == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("CPP EFFECTIVE BLOCK"));

		FTimerHandle Delay;
		RPCMultiCastBlockReaction = MulticastSkill;

		//Disable input for the other so we can counter attack
		if (EnemyElement != nullptr)
		{
			this->EnemyElement->CanSwipeAction = false;
			UE_LOG(LogTemp, Warning, TEXT("DISABLE ATTACKER INPUT"));
		}
		
		// Enter slow motion effect
		ServerSlowmo(0.2f);
		//CameraShake();

		//Play Block Reaction Montage and get the return duration for TimerHandler
		float Duration = GetMesh()->GetAnimInstance()->Montage_Play(RPCMultiCastBlockReaction, 1.0f, EMontagePlayReturnType::MontageLength, ReactionStartTime, true);
		UE_LOG(LogTemp, Warning, TEXT("RPCMultiCastBlockReaction : %s"), *RPCMultiCastBlockReaction->GetFName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("Duration in Slowmo: %f"), Duration);

		// Return montage length to execute EndingEffectBlock function
		this->GetWorld()->GetTimerManager().SetTimer(Delay, this, &ATodakBattleArenaCharacter::EndingEffectiveBlock, Duration, false);

	}

	//checks player is not on effective block
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HIT BLOCK REACTION"));

		RPCMultiCastBlockReaction = MulticastSkill;

		//Play Block Reaction Montage
		this->GetMesh()->GetAnimInstance()->Montage_Play(RPCMultiCastBlockReaction, 1.0f, EMontagePlayReturnType::Duration, ReactionStartTime);
		UE_LOG(LogTemp, Warning, TEXT("RPCMultiCastBlockReaction : %s"), *RPCMultiCastBlockReaction->GetFName().ToString());

	}
}
