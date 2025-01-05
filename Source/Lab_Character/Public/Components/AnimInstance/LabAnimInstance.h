// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "LabAnimInstance.generated.h"

struct FIKState;

/**
 * 
 */
UCLASS()
class LAB_CHARACTER_API ULabAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure=true)
	FRotator GetRelativeRotationFromIKState(FIKState ik);


	
};
