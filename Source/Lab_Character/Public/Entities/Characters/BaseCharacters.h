// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"

#include "Entities/StatefullCharacter.h"
#include <EnhancedInputSubsystemInterface.h>

#include "BaseCharacters.generated.h"

UCLASS()
class LAB_CHARACTER_API ABaseCharacters : public AStatefullCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacters();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInputMappingContext* DefaultMappingContext;

	void CleanAnimStatesCache();

	UFUNCTION(BlueprintCallable)
	FString GetCurrentStateName();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	bool IsAIControlled;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
