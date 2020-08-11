// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WidgetFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TODAKBATTLEARENA_API UWidgetFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	//index from 0 to 4, where 4 is the highest quality possible
	UFUNCTION(BlueprintCallable, Category = "Graphics|Widget Utils")
	static void UpdateSetGraphicSettingsWidget(int32 GraphicsQualityIndex);
};
