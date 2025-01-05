// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LabAnimParams.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class LAB_CHARACTER_API ULabAnimParams : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	FName ParamName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	bool Enabled;
	
};

USTRUCT(BlueprintType, Blueprintable)
struct LAB_CHARACTER_API FLabAninmState
{
	GENERATED_BODY()

public:

	FLabAninmState() : IsNone(true) {}

	FLabAninmState(bool isNone) : IsNone(isNone) {}

	UPROPERTY(BlueprintReadOnly)
	bool IsNone;

private:

};
