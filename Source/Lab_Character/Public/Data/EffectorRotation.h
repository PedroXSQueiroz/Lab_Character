// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EffectorRotation.generated.h"

/**
 * 
 */
UINTERFACE()
class UEffectorRotation : public UInterface
{
	GENERATED_BODY()
};


class LAB_CHARACTER_API IEffectorRotation {

	GENERATED_BODY()

public:

	FRotator GetRotationForEffector(UAnimInstance* anim, FName effector);

};