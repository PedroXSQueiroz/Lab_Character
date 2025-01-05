// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/AnimInstance/LabAnimInstance.h"
#include "LabMovementAnimInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStoppingEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartingEvent);

/**
 * 
 */
UCLASS()
class LAB_CHARACTER_API ULabMovementAnimInstance : public ULabAnimInstance
{
	GENERATED_BODY()
	
public:

	/*UFUNCTION(BlueprintCallable)
	FStoppingEvent& GetStoppingEvent() 
	{
		return this->StoppingEvent;
	}*/
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnStoppingEvent OnStoppingEvent;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnStartingEvent OnStartingEvent;

private:


};
