// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/States/DefaultEntityState.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Entities/Characters/BaseCharacters.h"

bool UDefaultEntityState::CharacterInit(ABaseCharacters* charac)
{
	if (Super::CharacterInit(charac)) 
	{
		
		UActionStatefullBinding* aimBinding = charac->GetBinding(FName("aim"));
		if (aimBinding) 
		{
			//aimBinding->InputBindingTrigger.AddDynamic(this, &UDefaultEntityState::StartAim);
		}

		charac->bUseControllerRotationYaw = false;
		charac->GetCharacterMovement()->bOrientRotationToMovement = true;
	
		return true;
	}
	
	return false;
}

void UDefaultEntityState::StartAim(const FInputActionValue& aim)
{
	this->ChangeTo(FName("aim"));
}
