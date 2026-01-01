// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Data/LabAnimParams.h"

#include "TurnInPlaceAnim.generated.h"

UENUM(BlueprintType)
enum class EIKForTurnInPlace : uint8 {

	COMPLETE = 0,
	CLEAN = 1,
	NONE = 3
};


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
	float PlayRate { 1 };

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool TransitionOnFinish;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EIKForTurnInPlace IKUsage { EIKForTurnInPlace::COMPLETE };

};

USTRUCT(BlueprintType, Blueprintable)
struct LAB_CHARACTER_API FTurnInPlaceState : public FLabAninmState
{
	
	GENERATED_BODY()

public:

	FTurnInPlaceState():FLabAninmState() {}
	FTurnInPlaceState(
			UAnimSequence* anim
		,	float progression = 0
		,	bool transit = false
		,	EAxis::Type axis = EAxis::Type::None
		,	float playRate = 1
		,	bool  posDirection = false
		,	EIKForTurnInPlace ikUsage = EIKForTurnInPlace::COMPLETE
	) :FLabAninmState(false)
	, TurnAnim(anim)
	, Progression(progression)
	, TransitionOnFinishTurn(transit)
	, PlayRate(playRate)
	, Axis(axis)
	, PositiveDirection(posDirection)
	, IKUsage(ikUsage)
	{}

	UPROPERTY(BlueprintReadOnly)
	UAnimSequence* TurnAnim;

	UPROPERTY(BlueprintReadOnly)
	float Progression;

	UPROPERTY(BlueprintReadOnly)
	bool TransitionOnFinishTurn;

	UPROPERTY(BlueprintReadOnly)
	float PlayRate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EAxis::Type> Axis;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool PositiveDirection;

	UPROPERTY(BlueprintReadOnly)
	EIKForTurnInPlace IKUsage;
};