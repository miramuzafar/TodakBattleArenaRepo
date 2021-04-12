// Fill out your copyright notice in the Description page of Project Settings.


#include "TBAAnimInstance.h"
#include "TodakBattleArenaCharacter.h"

void UTBAAnimInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTBAAnimInstance, RagdollMode);
	DOREPLIFETIME(UTBAAnimInstance, FallAnim);
	DOREPLIFETIME(UTBAAnimInstance, IsStopped);
	DOREPLIFETIME(UTBAAnimInstance, TurnLeft);
	DOREPLIFETIME(UTBAAnimInstance, TurnRight);
}

void UTBAAnimInstance::OnRep_TurnRight()
{

}

void UTBAAnimInstance::OnRep_TurnLeft()
{
}

void UTBAAnimInstance::PlayerIsRagdoll()
{
	if (RagdollMode == false)
	{
		RagdollMode = true;
	}
}

void UTBAAnimInstance::MeshIsStopped()
{
	if (IsStopped == false)
	{
		IsStopped = true;
	}
}

void UTBAAnimInstance::ReviveMesh()
{
	if (RagdollMode && IsStopped)
	{
		RagdollMode = false;
		IsStopped = false;
	}
}


