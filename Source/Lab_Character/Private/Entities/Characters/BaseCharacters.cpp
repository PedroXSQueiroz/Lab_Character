// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/Characters/BaseCharacters.h"
#include "Components/States/DefaultEntityState.h"
#include <Components/AnimInstance/BaseAnimInstance.h>

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

// Called when the game starts or when spawned
void ABaseCharacters::BeginPlay()
{
	Super::BeginPlay();

	UDefaultEntityState* initialState = this->GetComponentByClass<UDefaultEntityState>();
	initialState->Init(this, true);

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
		//TSharedPtr<UActionStatefullBinding> bindingPointer = MakeShareable<UActionStatefullBinding>(binding);

		input->BindAction(currentAction, ETriggerEvent::Triggered, binding, &UActionStatefullBinding::OnInputTrigger);

		this->Bindings.Add(currentActionName, binding);

	}

}

