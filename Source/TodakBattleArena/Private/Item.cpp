// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

UItem::UItem()
{
	Index = 0;
	ItemID = FText::FromString("Item_ID");
	ItemDisplayName = FText::FromString("Item");
	UseActionText = FText::FromString("Use");
}

void UItem::Use(AActor* Character)
{

}
