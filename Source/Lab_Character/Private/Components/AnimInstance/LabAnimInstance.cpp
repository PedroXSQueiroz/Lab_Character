// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AnimInstance/LabAnimInstance.h"

#include "Data/IKParams.h"

FRotator ULabAnimInstance::GetRelativeRotationFromIKState(FIKState ik)
{
	return ik.EffectorRelativeTransform.GetRotation().Rotator();
}
