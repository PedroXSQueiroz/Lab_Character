// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/EntityState.h"
#include "Data/LabAnimParams.h"
#include "LabCharacterState.generated.h"

class ABaseCharacters;
class ULabAnimParams;

/**
 * 
 */
UCLASS(Abstract)
class LAB_CHARACTER_API ULabCharacterState : public UEntityState
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FName> ActiveAnimParams;

	virtual bool Init(AActor* actor, bool first = false) override;

	virtual bool Exit(AActor* actor) override;
	
	virtual bool CharacterExit(ABaseCharacters* charac);

	virtual bool CharacterInit(ABaseCharacters* charac);

	ABaseCharacters* GetCharacter();

protected:

	template<typename AnimParam>
	void UpdateAnimParams(TArray<AnimParam*> params);

};
