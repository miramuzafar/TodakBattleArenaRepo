// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseCharacterWidget.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class TODAKBATTLEARENA_API UBaseCharacterWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	void OnSynchronizeProperties();

	virtual void SynchronizeProperties() override;
	// End of UWidget interface

	// Optionally override the Blueprint "Event Construct" event
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable)
		void ChangeProgressBarValue(AActor* currPlayer, float currVal, int MaxVal, float& currPercentage);
};
