// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterWidget.h"
#include "TodakBattleArenaCharacter.h"

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

//void UBaseCharacterWidget::ChangeProgressBarValue(AActor* currPlayer, float currVal, int MaxVal, float& currPercentage)
//{
//	//Update energy after action on progress bar
//	//UE_LOG(LogTemp, Warning, TEXT("Energy Remains: %f"), currPlayer->playerEnergy);
//
//	//currPlayer->EnergyPercentage = UGestureInputsFunctions::UpdateProgressBarComponent(this, "EnergyBar", "EnergyText", "Energy", "Energy", currPlayer->playerEnergy, currPlayer->MaxEnergy);
//
//	/*if (currWidget->GetOwningPlayerPawn() == currPlayer)
//	{
//		currPercentage = UGestureInputsFunctions::UpdateProgressBarComponent(currWidget, "EnergyBar", "EnergyText", "Energy", "Energy", currVal, MaxVal);
//		UE_LOG(LogTemp, Warning, TEXT("Energy Remains: %f"), currPercentage);
//	}*/
//	
//	if (GetOwningPlayer() != currPlayer)
//	{
//		currPercentage = UGestureInputsFunctions::UpdateProgressBarComponent(this, "EnergyBar_1", "EnergyText_1", "Energy", "Energy", currVal, MaxVal);
//		UE_LOG(LogTemp, Warning, TEXT(".......Energy Remains: %f"), currPercentage);
//	}
//}
