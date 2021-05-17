// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "BaseCharacterWidget.generated.h"

class ATodakBattleArenaCharacter;
class UButton;
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

	//Replicated Network setup
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, Category = "Player")
	ATodakBattleArenaCharacter* Player;

	UFUNCTION(BlueprintImplementableEvent, Category = "BlockHit")
	void CallReleasedButton();

	UFUNCTION(BlueprintImplementableEvent, Category = "BlockHit")
	void CallShowButton();

	UFUNCTION(BlueprintCallable)
	void ChangeProgressBarValue(UBaseCharacterWidget* currWidget, float currVal, int MaxVal, float& currPercentage);

	UFUNCTION(BlueprintCallable)
	void SetButtonVisibility(UButton* button, bool IsCurrentlyVisible, float& VisibilityDuration);

};
