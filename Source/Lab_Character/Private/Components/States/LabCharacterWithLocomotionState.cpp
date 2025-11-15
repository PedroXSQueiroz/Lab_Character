// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/States/LabCharacterWithLocomotionState.h"

#include <InputActionValue.h>

#include "Entities/Characters/BaseCharacters.h"

bool ULabCharacterWithLocomotionState::CharacterInit(ABaseCharacters* charac)
{
	
	charac->GetBinding(FName("move"))->InputBindingTrigger.AddDynamic(this, &ULabCharacterWithLocomotionState::Move);
	charac->GetBinding(FName("look"))->InputBindingTrigger.AddDynamic(this, &ULabCharacterWithLocomotionState::Look);

	return true;
}

void ULabCharacterWithLocomotionState::StatefullTick(AActor* actor, float DeltaTime)
{
	//DOES NOTHING
}

#pragma optimize("", off)
void ULabCharacterWithLocomotionState::Move(const FInputActionValue& moveAction)
{
	FVector2D movement = moveAction.Get<FVector2D>();
	this->ApplyMovement(movement);
}

void ULabCharacterWithLocomotionState::CalculateMovement(ABaseCharacters* charac, FVector2D movement, FVector& direction, float& intensity)
{
	FRotator currentHorizontalRotation = charac->GetControlRotation();
	currentHorizontalRotation.Pitch = 0;
	FVector forward = currentHorizontalRotation.Quaternion().GetForwardVector() * movement.Y;
	FVector right = currentHorizontalRotation.Quaternion().GetRightVector() * movement.X;

	intensity = FVector(movement.X, movement.Y, 0).Length();

	direction = forward + right;
}

void ULabCharacterWithLocomotionState::ApplyMovement(FVector2D movement)
{
	UE_LOG(LogTemp, Log, TEXT("move input => x: %.2f, y: %.2f"), movement.X, movement.Y);

	ABaseCharacters* charac = this->GetCharacter();

	float intensity;
	FVector movementDirection;

	this->CalculateMovement(charac, movement, movementDirection, intensity);

	if (intensity > this->MinVelocity)
	{
		movementDirection.Normalize();

		this->GetCharacter()->AddMovementInput(
			FVector(
				movementDirection.X,
				movementDirection.Y,
				0
			),
			//intensity
			FMath::Clamp(
				intensity,
				0.0f,
				1 - charac->GetClampVelocityInput()
			)
		);
	}
}

#pragma optimize("", on)

void ULabCharacterWithLocomotionState::Look(const FInputActionValue& lookAction)
{
	FVector2D look = lookAction.Get<FVector2D>();
	this->ApplyLook(look);
}

void ULabCharacterWithLocomotionState::ApplyLook(FVector2D look)
{
	this->GetCharacter()->AddControllerYawInput(look.X);
	this->GetCharacter()->AddControllerPitchInput(look.Y);
}

