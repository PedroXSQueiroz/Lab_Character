// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "WeightProvider.generated.h"

class UBaseAnimInstance;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class LAB_CHARACTER_API UWeightProvider : public UObject
{
	GENERATED_BODY()
	
public:

	virtual float GetCurrentWeight() PURE_VIRTUAL(TEXT("NOT IMPLEMENTED YET"), return 0; );

	void Init(UBaseAnimInstance* parentAnimInstance);

protected:

	UBaseAnimInstance* ParentAnimInstance;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class LAB_CHARACTER_API UWeightByAnimCurveProvider : public UWeightProvider 
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName CurveName;

	virtual float GetCurrentWeight() override;
};
