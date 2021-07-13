// Fill out your copyright notice in the Description page of Project Settings.


#include "ClothItem.h"
#include "TBACameraActor.h"
#include "Components/SkeletalMeshComponent.h"
//#include "Kismet/KismetMathLibrary.h"

void UClothItem::EquipCharacter(AActor * Character)
{
	CamActor = Cast<ATBACameraActor>(Character);
	if (CamActor)
	{
		switch (Index)
		{
		case 0:
			if (PickupMesh != CamActor->Top->SkeletalMesh)
			{
				CamActor->Top->SetSkeletalMesh(PickupMesh, false);
			}
			break;
		case 1:
			if (PickupMesh != CamActor->Bot->SkeletalMesh)
			{
				CamActor->Bot->SetSkeletalMesh(PickupMesh, false);
			}
			break;
		case 2:
			if (PickupMesh != CamActor->Hair->SkeletalMesh)
			{
				CamActor->Hair->SetSkeletalMesh(PickupMesh, false);
			}
			break;
		case 3:
			if (PickupMesh != CamActor->Skin->SkeletalMesh)
			{
				CamActor->Skin->SetSkeletalMesh(PickupMesh, false);
			}
			break;
		default:
			break;
		}
	}
}

void UClothItem::Use(AActor* Character)
{
	if (Character)
	{
		//Do character attribute logic here
		//Do cloth logic here
		EquipCharacter(Character);
	}
}
