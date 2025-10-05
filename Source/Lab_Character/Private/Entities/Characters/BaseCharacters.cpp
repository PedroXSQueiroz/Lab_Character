// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/Characters/BaseCharacters.h"

#include <Components/States/DefaultEntityState.h>
#include <Components/AnimInstance/BaseAnimInstance.h>
#include <GameFramework/CharacterMovementComponent.h>

#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>

// Sets default values
ABaseCharacters::ABaseCharacters()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ABaseCharacters::CleanAnimStatesCache()
{
	UAnimInstance* animInstance = this->GetMesh()->GetAnimInstance();
	UBaseAnimInstance* labAnimInstance = Cast<UBaseAnimInstance>(animInstance);
	labAnimInstance->ClearCaches();

}

UEntityState* ABaseCharacters::GetCurrentEntityState()
{
	TSet<UActorComponent*> components = this->GetComponents();

	for (UActorComponent* component : components)
	{
		if (component->IsA(ULabCharacterState::StaticClass()))
		{
			ULabCharacterState* state = Cast<ULabCharacterState>(component);
			if (state->GetIsStateActive())
			{
				return state;
			}
		}
	}
	
	return NULL;
}

FString ABaseCharacters::GetCurrentStateName()
{
	UEntityState* currentState = this->GetCurrentEntityState();

	if (currentState) 
	{
		return currentState->EntityStateName.ToString();
	}
	
	return FString();
}

void ABaseCharacters::SetClampVelocityInput(float clamp)
{
	//this->ClampVelocityInput = clamp;

	this->GetCharacterMovement()->MaxWalkSpeed = this->OriginalMaxWalkSpeed * ( 1 - clamp );
	
}

//float ABaseCharacters::GetClampVelocityInput()
//{
//	return this->ClampVelocityInput;
//}

// Called when the game starts or when spawned
void ABaseCharacters::BeginPlay()
{
	Super::BeginPlay();
	
	this->OriginalMaxWalkSpeed = this->GetCharacterMovement()->MaxWalkSpeed;
	if (Cast<APlayerController>(GetController())) {
		this->IsAIControlled = false;
	}
	else {
		this->IsAIControlled = true;

		TArray<FName> actionNames;

		this->Actions.GetKeys(actionNames);

		for (FName currentActionName : actionNames)
		{
			UInputAction* currentAction = this->Actions[currentActionName];
			UActionStatefullBinding* binding = NewObject<UActionStatefullBinding>();
			
			this->Bindings.Add(currentActionName, binding);
		}
	}

	TSet<UActorComponent*> components = this->GetComponents();

	for (UActorComponent* component : components) 
	{
		if (component->IsA(ULabCharacterState::StaticClass())) 
		{
			ULabCharacterState* state = Cast<ULabCharacterState>(component);
			if (state->IsInitial) 
			{
				state->Init(this, true);
			}
		}
	}

}

// Called every frame
void ABaseCharacters::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacters::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetWorld()->GetFirstLocalPlayerFromController()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	UEnhancedInputComponent* input = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	TArray<FName> actionNames;

	this->Actions.GetKeys(actionNames);

	for (FName currentActionName: actionNames) 
	{
		UInputAction* currentAction = this->Actions[currentActionName];
		UActionStatefullBinding* binding = NewObject<UActionStatefullBinding>();
		
		input->BindAction(currentAction, ETriggerEvent::Triggered, binding, &UActionStatefullBinding::OnInputTrigger);

		this->Bindings.Add(currentActionName, binding);

	}

}

