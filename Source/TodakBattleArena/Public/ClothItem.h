// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "ClothItem.generated.h"

/**
 * 
 */
UCLASS()
class TODAKBATTLEARENA_API UClothItem : public UItem
{
	GENERATED_BODY()

public:
	//Check if item is for female character
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	bool IsFem = true;

protected:

	virtual void Use(class AActor* Character) override;
	
};
