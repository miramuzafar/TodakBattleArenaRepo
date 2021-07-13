// Fill out your copyright notice in the Description page of Project Settings.


#include "TBACameraActor.h"
#include "Engine.h"
#include "Components/SkeletalMeshComponent.h"
#include "Item.h"
#include "Kismet/GameplayStatics.h"
#include "TodakGameStateBase.h"

ATBACameraActor::ATBACameraActor()
{
	//SkinMesh
	SkinMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkinMesh"));
	SkinMesh->SetupAttachment(RootComponent);
	SkinMesh->SetRelativeLocation(FVector(210.0f, -0.000013f, -80.0f));
	SkinMesh->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	SkinMesh->bEditableWhenInherited = true;

	//Arms
	Arms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arms"));
	Arms->SetupAttachment(SkinMesh);
	Arms->SetRelativeRotation(FRotator(0.0f, 0.0f, -0.000034f));
	Arms->bEditableWhenInherited = true;

	//Skin
	Skin = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skin"));
	Skin->SetupAttachment(SkinMesh);
	Skin->SetRelativeRotation(FRotator(0.0f, -0.000048f, 0.0f));
	Skin->bEditableWhenInherited = true;

	//Top
	Top = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Top"));
	Top->SetupAttachment(SkinMesh);
	Top->SetRelativeRotation(FRotator(0.0f, -0.000061f, 0.0f));
	Top->bEditableWhenInherited = true;

	//Bot
	Bot = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Bot"));
	Bot->SetupAttachment(SkinMesh);
	Bot->SetRelativeRotation(FRotator(0.0f, -0.000061f, 0.0f));
	Bot->bEditableWhenInherited = true;

	//Face
	Face = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Face"));
	Face->SetupAttachment(SkinMesh);
	Face->SetRelativeRotation(FRotator(0.0f, -0.000034f, 0.0f));
	Face->bEditableWhenInherited = true;

	//Feet
	Feet = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Feet"));
	Feet->SetupAttachment(SkinMesh);
	Feet->SetRelativeRotation(FRotator(0.0f, -0.000034f, 0.0f));
	Feet->bEditableWhenInherited = true;

	//Hair
	Hair = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hair"));
	Hair->SetupAttachment(Face, "head");
	Hair->SetRelativeLocation(FVector(-163.999969f, 5.999954f, 0.0f));
	Hair->SetRelativeRotation(FRotator(-86.999313f, 89.999023f, -89.998894f));
	Hair->bEditableWhenInherited = true;
}

void ATBACameraActor::BeginPlay()
{
	Super::BeginPlay();
	GameState = Cast<ATodakGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
}

void ATBACameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATBACameraActor::CheckValidityMesh(USkeletalMesh* iarms, USkeletalMesh* iskin, USkeletalMesh* itop, USkeletalMesh* ibot, USkeletalMesh* iface, USkeletalMesh* ihair, USkeletalMesh* ifeet)
{
	SetSkeletalMeshes(Arms, iarms, DArms);
	SetSkeletalMeshes(Skin, iskin, DSkin);
	SetSkeletalMeshes(Top, itop, DTop);
	SetSkeletalMeshes(Bot, ibot, DBot);
	SetSkeletalMeshes(Face, iface, DFace);
	SetSkeletalMeshes(Hair, ihair, DHair);
	SetSkeletalMeshes(Feet, ifeet, DFeet);
}

void ATBACameraActor::SetSkeletalMeshes(USkeletalMeshComponent* currMeshComp, USkeletalMesh* currMeshObj, USkeletalMesh* DefObj)
{
	if (currMeshObj != nullptr)
	{
		currMeshComp->SetSkeletalMesh(currMeshObj, false);
	}
	else
		currMeshComp->SetSkeletalMesh(DefObj, false);
}

void ATBACameraActor::SaveCurrentOutfit(UItem* item, bool IsItFem, int ItemIndex)
{
	if (item != nullptr)
	{
		switch (ItemIndex)
		{
		case 0:
			if (IsItFem)
			{
				GameState->FemTopItem = item;
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Top: %s"), *GameState->FemTopItem->GetName()));
			}
			else
			{
				GameState->MalTopItem = item;
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Top: %s"), *GameState->MalTopItem->GetName()));
			}
			break;
		case 1:
			if (IsItFem)
			{
				GameState->FemBotItem = item;
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Bot: %s"), *GameState->FemBotItem->GetName()));
			}
			else
			{
				GameState->MalBotItem = item;
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Bot: %s"), *GameState->MalBotItem->GetName()));
			}
			break;
		case 2:
			if (IsItFem)
			{
				GameState->FemHairItem = item;
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Hair: %s"), *GameState->FemHairItem->GetName()));
			}
			else
			{
				GameState->MalHairItem = item;
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Hair: %s"), *GameState->MalHairItem->GetName()));
			}
			break;
		case 3:
			if (IsItFem)
			{
				GameState->SkinFemItem = item;
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Skin: %s"), *GameState->SkinFemItem->GetName()));
			}
			else
			{
				GameState->SkinMalItem = item;
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Skin: %s"), *GameState->SkinMalItem->GetName()));
			}
			break;
		default:
			break;
		}
	}
	else
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("NO ITEM SELECTED!!")));
}
