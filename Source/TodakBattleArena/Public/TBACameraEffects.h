// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShake.h"
#include "TBACameraEffects.generated.h"

/**
 * 
 */
UCLASS()
class TODAKBATTLEARENA_API UTBACameraEffects : public UCameraShake
{
	GENERATED_BODY()
	
	public:
	
		UFUNCTION(BlueprintCallable, Category = "Damage")
		void CameraShake();

};
