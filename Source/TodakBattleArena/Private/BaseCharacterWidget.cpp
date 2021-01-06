// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterWidget.h"

void UBaseCharacterWidget::SynchronizeProperties()
{
	//Sync properties
	Super::SynchronizeProperties();
}

void UBaseCharacterWidget::NativeConstruct()
{
	// Call the Blueprint "Event Construct" node
	Super::NativeConstruct();
}
