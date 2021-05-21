// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterWidget.h"
#include "TodakBattleArenaCharacter.h"
#include "Components/Border.h"

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

void UBaseCharacterWidget::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBaseCharacterWidget, Player);
}

void UBaseCharacterWidget::ChangeProgressBarValue(UBaseCharacterWidget* currWidget, float currVal, int MaxVal, float& currPercentage)
{
	currPercentage = UGestureInputsFunctions::UpdateProgressBarComponent(currWidget, "EnergyBar_1", "EnergyText_1", "Energy", "Energy", currVal, MaxVal);
	UE_LOG(LogTemp, Warning, TEXT(".......Energy Remains: %f"), currPercentage);

	//Update energy after action on progress bar
	//UE_LOG(LogTemp, Warning, TEXT("Energy Remains: %f"), currPlayer->playerEnergy);

	//currPlayer->EnergyPercentage = UGestureInputsFunctions::UpdateProgressBarComponent(this, "EnergyBar", "EnergyText", "Energy", "Energy", currPlayer->playerEnergy, currPlayer->MaxEnergy);

	/*if (currWidget->GetOwningPlayerPawn() == currPlayer)
	{
		currPercentage = UGestureInputsFunctions::UpdateProgressBarComponent(currWidget, "EnergyBar", "EnergyText", "Energy", "Energy", currVal, MaxVal);
		UE_LOG(LogTemp, Warning, TEXT("Energy Remains: %f"), currPercentage);
	}*/
	
	/*if (GetOwningPlayer() != currPlayer)
	{
		currPercentage = UGestureInputsFunctions::UpdateProgressBarComponent(this, "EnergyBar_1", "EnergyText_1", "Energy", "Energy", currVal, MaxVal);
		UE_LOG(LogTemp, Warning, TEXT(".......Energy Remains: %f"), currPercentage);
	}*/
}

void UBaseCharacterWidget::SetButtonVisibility(UBorder* button, bool IsCurrentlyVisible, float& VisibilityDuration)
{
	if (!IsCurrentlyVisible)
	{
		button->SetVisibility(ESlateVisibility::Visible);
		UE_LOG(LogTemp, Warning, TEXT("BaseCharacterWidget::Button is visible"));
	}
	else if (IsCurrentlyVisible)
	{
		if (IsPressed)
		{
			//button->SetPressMethod(EButtonPressMethod::ButtonRelease);
			IsPressed = false;
			this->CallReleasedButton();
		}
		button->SetVisibility(ESlateVisibility::Hidden);
		UE_LOG(LogTemp, Warning, TEXT("BaseCharacterWidget::Button is hidden"));
	}
}

