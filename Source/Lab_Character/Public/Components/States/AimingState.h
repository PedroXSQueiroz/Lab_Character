// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/States/LabCharacterWithLocomotionState.h"

#include "Components/EntityState.h"

#include <Components/AnimInstance/BaseAnimInstance.h>

#include "AimingState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class LAB_CHARACTER_API UAimingState : public ULabCharacterWithLocomotionState
{
	GENERATED_BODY()

public:

	virtual bool CharacterInit(ABaseCharacters* charac) override;

	virtual void StatefullTick(AActor* actor, float DeltaTime) override;

	UFUNCTION()
	void StopAiming(const FInputActionValue& aim);

	UPROPERTY()
	AActor* CurrentTarget;

private:

	UPROPERTY()
	UBaseAnimInstance* CachedAnimInstance;
	
};
