// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "TBACameraActor.generated.h"

class USkeletalMesh;
class UItem;
class ATodakGameStateBase;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class TODAKBATTLEARENA_API ATBACameraActor : public ACameraActor
{
	GENERATED_BODY()

public:

	ATBACameraActor();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* SkinMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Arms;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Skin;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Top;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Bot;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Face;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Hair;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Feet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DefaultMesh")
	USkeletalMesh* DArms;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DefaultMesh")
	USkeletalMesh* DSkin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DefaultMesh")
	USkeletalMesh* DTop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DefaultMesh")
	USkeletalMesh* DBot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DefaultMesh")
	USkeletalMesh* DFace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DefaultMesh")
	USkeletalMesh* DHair;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DefaultMesh")
	USkeletalMesh* DFeet;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Equip")
	ATodakGameStateBase* GameState;

	//FUNCTIONS
	UFUNCTION(BlueprintCallable, Category = "Equip")
	void CheckValidityMesh(USkeletalMesh* iarms, USkeletalMesh* iskin, USkeletalMesh* itop, USkeletalMesh* ibot, USkeletalMesh* iface, USkeletalMesh* ihair, USkeletalMesh* ifeet);

	void SetSkeletalMeshes(USkeletalMeshComponent* currMeshComp, USkeletalMesh* currMeshObj, USkeletalMesh* DefObj);

	UFUNCTION(BlueprintCallable, Category = "Equip")
	void SaveCurrentOutfit(UItem* item, bool IsItFem, int ItemIndex);

	UFUNCTION(BlueprintCallable, Category = "SetupMesh")
	void ChangeGenderAsset(ATodakGameStateBase* GS, bool IsItFem);
};
