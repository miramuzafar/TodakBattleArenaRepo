// Fill out your copyright notice in the Description page of Project Settings.


#include "TBACameraShake.h"

UTBACameraShake::UTBACameraShake()
{
	OscillationDuration = 0.25f;
	OscillationBlendInTime = 0.05f;
	OscillationBlendOutTime = 0.05f;

	RotOscillation.Pitch.Amplitude = FMath::RandRange(1.0f, 3.0f);
	RotOscillation.Pitch.Frequency = FMath::RandRange(15.0f, 20.0f);

	RotOscillation.Yaw.Amplitude = FMath::RandRange(1.0f, 3.0f);
	RotOscillation.Yaw.Frequency = FMath::RandRange(15.0f, 20.0f);
}
