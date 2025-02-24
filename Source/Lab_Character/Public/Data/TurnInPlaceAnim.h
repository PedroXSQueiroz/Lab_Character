// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Data/LabAnimParams.h"

#include "TurnInPlaceAnim.generated.h"
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class LAB_CHARACTER_API UTurnInPlaceParams : public ULabAnimParams
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MinDeviation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EAxis::Type> DeviationAxis;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MinVelocity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxVelocity { -1 };

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAnimSequence* TurnAnim;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool TransitionOnFinish;

};

USTRUCT(BlueprintType, Blueprintable)
struct LAB_CHARACTER_API FTurnInPlaceState : public FLabAninmState
{
	
	GENERATED_BODY()

public:

	FTurnInPlaceState():FLabAninmState() {}
	FTurnInPlaceState(UAnimSequence* anim, float progression = 0, bool transit = false):FLabAninmState(false)
	, TurnAnim(anim)
	, Progression(progression)
	, TransitionOnFinishTurn(transit)
	{}

	UPROPERTY(BlueprintReadOnly)
	UAnimSequence* TurnAnim;

	UPROPERTY(BlueprintReadOnly)
	float Progression;

	UPROPERTY(BlueprintReadOnly)
	bool TransitionOnFinishTurn;
};