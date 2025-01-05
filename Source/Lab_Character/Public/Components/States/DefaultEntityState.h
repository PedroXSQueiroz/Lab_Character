// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <InputActionValue.h>

#include "Components/EntityState.h"
#include "Components/States/LabCharacterWithLocomotionState.h"

#include "DefaultEntityState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class LAB_CHARACTER_API UDefaultEntityState : public ULabCharacterWithLocomotionState
{
	GENERATED_BODY()

public:

	virtual bool CharacterInit(ABaseCharacters* charac) override;

	UFUNCTION()
	void StartAim(const FInputActionValue& aim);

};
