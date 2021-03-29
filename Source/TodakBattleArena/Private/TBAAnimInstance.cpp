// Fill out your copyright notice in the Description page of Project Settings.


#include "TBAAnimInstance.h"
#include "TodakBattleArenaCharacter.h"

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


