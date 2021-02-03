// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable, EditInLineNew, DefaultToInstanced)
class TODAKBATTLEARENA_API UItem : public UObject
{
	GENERATED_BODY()

public:
	UItem();

	virtual class UWorld* GetWorld() const { return World; };

	UPROPERTY(Transient)
	class UWorld* World;

	//The Item ID
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	FText ItemID;
	
	//The text for using the item, (Equip, Eat, etc)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	FText UseActionText;

	//The mesh to display for this items pickup
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	class USKeletalMesh* PickupMesh;

	//The thumbnail for this item
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	class UTexture2D* Thumbnail;

	//The display name for this item in inventory
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	FText ItemDisplayName;

	//An optional description for the item
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta = (MultiLine = true))
	FText ItemDescription;

	//The equipped part of the item exp: 0 is top body etc
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta = (ClampMin = 0))
	int Index;

	//The inventory that owns the item
	UPROPERTY()
	class UInventoryComponent* OwningInventory;

	virtual void Use(class AActor* Character);

	//Blueprint version of vritual Use function
	UFUNCTION(BlueprintImplementableEvent)
	void OnUse(class AActor* Character);
};
