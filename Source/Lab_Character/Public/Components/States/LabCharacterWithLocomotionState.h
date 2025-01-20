// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/States/LabCharacterState.h"

#include "LabCharacterWithLocomotionState.generated.h"

/**
 * 
 */
UCLASS()
class LAB_CHARACTER_API ULabCharacterWithLocomotionState : public ULabCharacterState
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MinVelocity;

	virtual bool CharacterInit(ABaseCharacters* charac) override;

	virtual void StatefullTick(AActor* actor, float DeltaTime) override;

	UFUNCTION()
	void Move(const FInputActionValue& moveAction);

	virtual void ApplyMovement(FVector2D movement);

	UFUNCTION()
	void Look(const FInputActionValue& lookAction);

	virtual void ApplyLook(FVector2D look);

protected:
	
	void CalculateMovement(ABaseCharacters* charac, FVector2D movement, FVector& direction, float& intensity);

};
