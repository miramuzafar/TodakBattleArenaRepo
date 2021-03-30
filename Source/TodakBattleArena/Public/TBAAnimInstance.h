// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Net/UnrealNetwork.h"
#include "TBAAnimInstance.generated.h"

/**
 * 
 */
UCLASS(Transient, Blueprintable, HideCategories=AnimInstance, BlueprintType)
class TODAKBATTLEARENA_API UTBAAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	//Replicated Network setup
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool TurnRight = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool TurnLeft = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LocoPlayrate;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Ragdoll")
	UAnimSequenceBase* FallAnim;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Ragdoll")
	bool RagdollMode = false;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Ragdoll")
	bool IsStopped = false;

	
protected:
	//variables

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsMoving = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Direction = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int IdleAnimToPlay = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Switcher = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool SwitchSide = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DefendMode = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RightFootEffectorLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LeftFootEffectorLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector JointTargetLocationRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector JointTargetLocationLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AlphaLeft = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AlphaRight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HipDisplacement = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RotationFixLeftFoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RotationFixRightFoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IKEnable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float IKMultiplier = 0.0f;

	//Functions

	UFUNCTION(BlueprintCallable, Category = "Ragdoll")
	void PlayerIsRagdoll();

	UFUNCTION(BlueprintCallable, Category = "Ragdoll")
	void MeshIsStopped();

	UFUNCTION(BlueprintCallable, Category = "Ragdoll")
	void ReviveMesh();


};
